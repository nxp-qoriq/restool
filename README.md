# Restool

restool is a user space application providing the ability to dynamically
create and manage DPAA2 containers and objects from Linux.

## Prerequisites

restool interacts with the DPAA2 Management Complex (MC).  It uses an ioctl to
send MC commands, and thus requires a Linux kernel driver providing the needed
ioctl support.

## Building

restool is designed for use with ARMv8 SoCs and has been tested with ARMv8
cross and native compilers.

```
# cross-build
export CROSS_COMPILE=[name-of-cross-compiler]
make

# native build
make

# building big-endian
make EXTRA_CFLAGS=-mbig-endian
```

## Installing

```
make install
```
...will install restool into $DESTDIR/sbin and the scripts into
$DESTDIR/usr/bin.

## Getting Help

restool has complete built-in help, including examples.

```
# see global help, including all objects supported
restool --help

# see the commands available for a given object
restool [object] help
	(e.g. restool dpni help)

# see the help for a specific object/command combination
restool [object] [command] --help
	(e.g. restool dpni create --help)
```

## Wrapper Scripts

The ./scripts directory contains a set of wrapper scripts that can
be used to perform complicated operations that could take a number
of restool invocations:

```
   ls-addni   # adds a network interface
   ls-addmux  # adds a dpdmux
   ls-addsw   # adds an l2switch
   ls-listmac # lists MACs and their connections
   ls-listni  # lists network interfaces and their connections
```
