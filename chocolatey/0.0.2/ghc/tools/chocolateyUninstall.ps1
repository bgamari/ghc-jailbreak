$version     = '0.0.1'
$packageName = 'ghc-jailbreak'

$binRoot         = $(Split-Path -parent $MyInvocation.MyCommand.Definition)
$packageFullName = Join-Path $binRoot ($packageName + '-' + $version)
$binPackageDir   = Join-Path $packageFullName "bin"

if ($env:ChocolateyForceX86 -eq $true) {
    $invArch = "i686"
} else {
    $invArch = "x86_64"
}

# Would have loved to use $env:ChocolateyToolsLocation but
# that seems to only return C:\. Even after a call to Get-ToolsLocation
$invTools = Join-Path $env:ChocolateyPackageFolder "tools"
$invToolsBin = Join-Path $invTools $invArch
$patcher = Join-Path $invToolsBin "iat-patcher.exe"
Write-Host "Hiding shims for `'$invTools`'."
$files = get-childitem $invTools -include *.exe -recurse

foreach ($file in $files) {
    #generate an ignore file
    New-Item "$file.ignore" -type file -force | Out-Null
}

function Use-PatchGHC {
    param( [string] $name
         , [string] $patchtool
         , [string] $redistdir
         , [bool] $install = $true )

    $proc = Execute-Command $name '--info' -PassThru
    if ($proc.ExitCode -ne 0) {
        throw ("Command `'$name --info`' did not complete successfully. ExitCode: " + $proc.ExitCode)
    }

    $infoTable = $proc.stdout
    $paths = $infoTable -split '[\r\n]'`
           | Select-String "C compiler command" -SimpleMatch `
           | ForEach-Object { $_.ToString().Split('"') } `
           | Where-Object { $_ -notlike $null} `
           | ForEach-Object { Split-Path $_ -Resolve -ErrorAction Ignore }

    if ($install -eq $true) {
      $action = "install"
    } else {
      $action = "uninstall"
    }

    foreach ($path in $paths) {
        $lst1 = Get-ChildItem "$path/.." -Filter *.exe -Recurse
        $files = $lst1
        Write-Debug "Inspecting `'$path`' and `'$path2`'."
        $i=0
        $ix=100 / $files.Count
        foreach ($file in $files) {
            Write-Progress -Activity Patching -Status 'Restoring...' `
                           -PercentComplete $i -CurrentOperation $file.FullName
            $proc = Execute-Command $patchtool @($action, $file.FullName)
            $toolOutput = $proc.stdout
            Write-Debug $toolOutput
            if ($proc.ExitCode -ne 0) {
                throw ("Could not patch `'$file`'. ExitCode: " + $proc.ExitCode)
            }

            Write-Debug ("Restored executable. Deleting runtimes.")
            Remove-Item -Path $file.Directory.FullName `
                        -Include muxcrt.dll, phxcrt.dll -Force
            $i+=$ix
            Write-Progress -Activity Patching -Status 'Restoring...' `
                           -PercentComplete $i -CurrentOperation $file.FullName
        }
    }

    Write-Host "Done patching GHC's Mingw-w64 distribution. Good to go."
}
Function Execute-Command ($commandPath, $commandArguments)
{
  Try {
    $pinfo = New-Object System.Diagnostics.ProcessStartInfo
    $pinfo.FileName = $commandPath
    $pinfo.RedirectStandardError = $false
    $pinfo.RedirectStandardOutput = $true
    $pinfo.UseShellExecute = $false
    $pinfo.WindowStyle = 'Hidden'
    $pinfo.CreateNoWindow = $true
    $pinfo.Arguments = $commandArguments
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $pinfo
    $p.Start() | Out-Null
    [pscustomobject]@{
        stdout = $p.StandardOutput.ReadToEnd()
        ExitCode = $p.ExitCode
    }
    $p.WaitForExit()
  }
  Catch {
     exit
  }
}

# Make sure GHC is on the path.
refreshenv
Use-PatchGHC ghc $patcher $invToolsBin $false