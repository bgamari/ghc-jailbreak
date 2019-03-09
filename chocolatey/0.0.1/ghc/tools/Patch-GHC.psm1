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
            Write-Progress -Activity Patching -Status 'Jailbreaking...' `
                           -PercentComplete $i -CurrentOperation $file.FullName
            $proc = Execute-Command $patchtool @($action, $file.FullName)
            $toolOutput = $proc.stdout
            Write-Debug $toolOutput
            if ($proc.ExitCode -ne 0) {
                throw ("Could not patch `'$file`'. ExitCode: " + $proc.ExitCode)
            }
            $i+=$ix
            Write-Progress -Activity Patching -Status 'Jailbreaking...' `
                           -PercentComplete $i -CurrentOperation $file.FullName
        }
        if ($files.Count -gt 0) {
            Write-Debug ("Patched " + $files.Count + " executables. Copying runtimes.")
            Get-ChildItem -Path $redistdir -Filter *.dll `
            | ForEach-Object { Copy-Item -Path $_.FullName -Destination $path `
                                         -Force; }
            Write-Debug ("installed new CRT in `'$path`' and `'$path2`'.")
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

Export-ModuleMember -Function "Use-PatchGHC"