# GHC Jail-Break
Chocolatey sources for GHC Jail-Break

This repository contains the sources for the GHC-JailBreak Chocolatey packages.

To use these get Chocolatey https://chocolatey.org/

and then just install the version of GHC you want, but it needs to be at least GHC version 8.6.4.  After that install this tool ghc-jailbreak.

    cinst ghc-jailbreak

for the latest version

    cinst ghc-jailbreak -pre

for the latest pre-release version

    cinst ghc-jailbreak -version 0.0.1

for  specific version, e.g. `0.0.1`

The installer will automatically pick the right bitness for your OS, but if you would
like to force it to get `x86` on `x86_64` you can:

    cinst ghc-jailbreak -x86

The installer also requires write permissions to the ghc folder, as such it needs administrative rights.

uninstalling can be done with

    cuninst ghc-jailbreak

If more than one version of `GHC-JailBreak` is present then you will be presented with prompt on which version you
would like to uninstall.

     Note: You need to run `refreshenv` or restart your console before ghc is available.


== Module Def creation routine ==

The following regexpr are used to generate the base def files

```
^([_\?@$\w]+)$
$1=msvcrt.$1

 ==
=msvcrt.

^([_\w]+)\s+DATA$
$1=msvcrt.$1 DATA
```

Use the `build.ps1` script to build the proper packages.
