# U-Boot applets

This repository contains the source code of a set of applets which must be built
against the [U-Boot Exports API](https://github.com/u-boot/u-boot/blob/master/include/exports.h)
to be running from U-Boot.

To build a specific applet, enter the following command in the top-level U-Boot directory.
Note that the **trailing slash** must be presented.

```
$ make <path to the applet source tree>/
```

Enter the following command to clean up the artifacts of a built applet:

```
$ make -f scripts/Makefile.clean obj=<path to the applet source tree>
```
