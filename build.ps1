param (
    [string]$specific = ""
)

function execute {
    param( [string] $message
         , [string] $msysBashShell
         , [string] $osBitness
         , [string] $command
         , [bool] $ignoreExitCode = $false
         )
    # Set the APPDATA path which does not get inherited during these invokes
    # and set MSYSTEM to make sure we're using the right system
    $envdata = "export APPDATA=""" + $Env:AppData + """" +`
                " && export MSYSTEM=MINGW${osBitness}" + `
                " && export PATH=/mingw${osBitness}/bin:`$PATH" + `
                " && export CFLAGS=""-DONLY_CHOCOLATEY""" + `
                # " && export CFLAGS=""-DUSE_BACKUPS=0""" + `
                " && "

    Write-Host "$message with '$command'..."
    $proc = Start-Process -NoNewWindow -UseNewEnvironment -Wait $msysBashShell -ArgumentList '--login', '-c', "'$envdata $command'" -PassThru

    if ((-not $ignoreExitCode) -and ($proc.ExitCode -ne 0)) {
        throw ("Command `'$command`' did not complete successfully. ExitCode: " + $proc.ExitCode)
    }
}

$shell_x64 = "e:\msys64\usr\bin\bash.exe"
$shell_x86 = "e:\msys32\usr\bin\bash.exe"
$shell_args = "make -w -C ""${pwd}"""

# First build the distributions
execute "Building x86_64 patcher" $shell_x64 64 $shell_args
execute "Building i686 patcher" $shell_x86 32 $shell_args

$path = Split-Path $MyInvocation.MyCommand.Path -Parent
$bin  = Join-Path $path "bin"
mkdir -Force $bin
Write-Host "$bin"
ls ghc*.nuspec -recurse -File | ForEach-Object {
            $dir = (Split-Path $_.FullName -Parent)
            if ($dir.Contains($specific) -or ($specific -eq "")) {
                Write-Host "Compiling $_"
                cd $dir
                choco pack
                mv *.nupkg -fo "$bin"
            }
        }

cd $path

Write-Host "Done."