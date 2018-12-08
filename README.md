== Module Def creation routine ==

The following regexpr are used

```
^([_\w]+)$
$1=msvcrt.$1

 ==
=msvcrt.

^([_\w]+)\s+DATA$
$1=msvcrt.$1 DATA
```
