% Restool(1) restool 1.0.0
% Cosmin-Florin Aluchenesei
% January 2021

# NAME
restool - create and manage DPAA2 containers and objects.

# OVERVIEW
To see the commands each object suppports:
: restool `<object-type> --help`

For object type and command-specific help:
: restool `<object-type> <command> --help`


# SYNOPSIS
**restool** `[<global-opts>] <object-type> <command> <object-name> [ARGS...]`

# DESCRIPTION
**restool** is a user space application providing the ability to dynamically
create and manage DPAA2 containers and objects from Linux.

Restool is designed for use with ARMv8 SoCs and has been tested with ARMv8 cross and native compilers.

# OPTIONS
Valid `<global-opts>` are:

**`-v`**, **`--version`**
: Displays tool version `info`

**`-m`**, **`--mc-version`**
: Displays mc firmware version

**`-h`**, **`-?`**, **`--help`**
: Displays general help `info`

**`-s`**, **`--script`**
: Display script friendly output

**`--rescan`**
: Issues a rescan of fsl-mc bus before exiting

**`--root=[dprc]`**
: Specifies root container name

Valid commands vary for each object type. Most objects support the following commands:
: help,
: `info`,
: create,
: destroy

`<object-name>` is a string containing object type and ID (e.g. dpni.7)

Valid `<object-type>` values are:
: `<dprc|dpni|dpio|dpsw|dpbp|dpci|dpcon|dpseci|dpdmux|dpmcp|dpmac|dpdcei|dpaiop|dprtc|dpdmai>`

# NOTE

> For each valid object-type the info and destroy commands are the same.

> For info command:

>> Usage: restool `<object-type>` info `<object-type-object>` [OPTIONS]

>> OPTIONS:

>>> `--verbose`

>>>> Shows extended/verbose information about the object

>> EXAMPLE:

>>> Display information about dpni.5:

>>>> $ restool dpni info dpni.5

> For destroy command:

>> Usage: restool `<object-type>` destroy `<object-type-object>`

>>> e.g. restool dpni destroy dpni.9

>>> e.g. restool dpdmux destroy dpdmux.9

# DPRC
Usage: restool dprc `<command> [--help] [ARGS...]`, where `<command>` can be:

**sync**
: synchronize the objects in MC with MC bus.

> Usage: restool dprc sync

**list**
: lists all containers (DPRC objects) in the system.

> Usage: restool dprc list [OPTIONS]

> OPTIONS:

>> `--full-path`

>>> prints the dprc list in a full-path

>>> format like: dprc.1/dprc.2

**show**
: displays the object contents of a DPRC object.

> Usage: restool dprc show `<container>`

**`info`**
: displays detailed `information` about a DPRC object.

**create**
: creates a new child DPRC under the specified parent.

> Usage: restool dprc create `<parent-container> [--options=<options-mask>]`

>>> `[--label=<label-string>]`

> OPTIONS:

>> `--options=<options-mask>`

>>> Where <options-mask> is a comma or space separated list of DPRC options:

>>>> DPRC_CFG_OPT_SPAWN_ALLOWED

>>>> DPRC_CFG_OPT_ALLOC_ALLOWED

>>>> DPRC_CFG_OPT_OBJ_CREATE_ALLOWED

>>>> DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED

>>>> DPRC_CFG_OPT_AIOP

>>>> DPRC_CFG_OPT_IRQ_CFG_ALLOWED

>>>> DPRC_CFG_OPT_PL_ALLOWED

>> `--label=<label-string>`

>>> Where `<label-string>` is a string of up to 15 chars to label

>>> the container

> EXAMPLES:

>> Create a child DPRC under parent dprc.1 with default options:

>>> $ restool dprc create dprc.1

>> Create a child DPRC under parent dprc.1 with default options and a label string:

>>> $ restool dprc create dprc.1 --label="nadk's dprc"

**destroy**
: destroys a child DPRC under the specified parent.

**assign**
: moves an object from a parent container to a child container.

> change an object's plugged state.

> To move an object from parent to child:

> Usage: restool dprc assign `<container> --child=<child-container>`

>>> `--object=<object>`

> To move an object and set the plugged state of an object:

> Usage: restool dprc assign `<container> --child=<child-container>`

>> `--object=<object> --plugged=<state>`

> To set the plugged state of an object:

> Usage: restool dprc assign `<container> --object=<object> --plugged=<state>`

>> `<container>`

>>> Specifies the source container for the operation.

>> `--child=<child-container>`

>>> Specifies the destination container for the operation.

>> `--object=<object>`

>>> Specifies the object to move from parent container to child container

>> `--plugged=<state>`

>>> Specifies the plugged state of the object (valid values are 0 or 1)

> NOTES:

>> -It is not possible to assign DPRC objects.

>> -It is not possible (and unnecessary) to change the plugged state of a DPRC

>> -It is not possible to move plugged objects (i.e. plugged=1)

>> -The assign operation may be restricted by the permissions granted in

>>> the container attributes.

> EXAMPLES:

>> To move dpni.2 from dprc.1 to dprc.4 (leaving the plugged state unchanged):

>>> $ restool dprc assign dprc.1 --child=dprc.4 --object=dpni.2

>> To move dpni.2 from dprc.1 to dprc.4 and set dpni.2 to be plugged:

>>> $ restool dprc assign dprc.1 --child=dprc.4 --object=dpni.2 --plugged=1

>> To set dpni.2 in container dprc.1 to be plugged:

>>> $ restool dprc assign dprc.1 --object=dprc.2 --plugged=1

unassign
: moves an object from a child container to a parent container.

> Usage: restool dprc unassign `<parent-container> --child=<child-container>`

>>> `--object=<object>`

>> `<parent-container>`

>>> Container that is the destination of the operation.

>> `--child=<child-container>`

>>> Container that is the source of the operation.

>> `--object=<object>`

>>> Specifies the object to move from parent to child.

> NOTES:

>> -It is not possible to unassign dprc objects

> EXAMPLE:

>> To move dpni.2 from dprc.4 (child) to dprc.1 (parent):

>>> $ restool dprc unassign dprc.1 `--child=dprc.4 --object=dpni.2`

**set-label**
: sets label/alias for any objects except root container.

> Usage: restool dprc set-label `<object> --label=<label>`

>> `--label=<label>`

>>> Specifies a string of up to 15 characters to label the object.

> NOTE:

>> `-<object>` cannot be the root container

> EXAMPLE:

>> To set the label "mountain" on dpni.1:

>>> $ restool dprc set-label dpni.1 `--label="mountain"`

**set-locked**
: lock/unlock a child container.

> Usage: restool dprc set-locked `<child_container> --locked=<state>`

>> `--locked=<state>`

>>> state = 1; child container is locked

>>> state = 0; child container is unlocked

> NOTE:

>> `<child_container>`

>>> Must be one of the child containers of the container executing the command

> EXAMPLE:

>> To lock the container dprc.2 that is the child container of dprc.1:

>>> $ restool dprc set-locked dprc.2 `--locked=1`

**connect**
: connects 2 objects, `creating` a link between them.

> Usage: restool dprc connect `<parent-container> --endpoint1=<object>`

>>> `--endpoint2=<object>` [OPTIONS]

>> `<parent-container>`

>>> Specifies the parent-container.

>> `--endpoint1=<object>`

>>> Specifies an endpoint object.

>> `--endpoint2=<object>`

>>> Specifies an endpoint object.

> OPTIONS:

>> `--committed-rate=<number>`

>>> Committed rate (Mbits/s). Must be provided alongside max-rate.

>> `--max-rate=<number>`

>>> Maximum rate (Mbits/s). Must be provided alongside committed-rate.

> NOTES:

>> `-<parent-container>` must be a common ancestor of both `<object>` arguments

>>> (for example the root container)

>> `-endpoint1` and endpoint2 do not need to be in the same container.

>> `-endpoints` must be disconnected prior to being connected

>>> (use 'dprc disconnect' to disconnect endpoints)

>> `-multi-port` objects such as a dpsw or dpdmux use the following naming

>>> convention to specify endpoints: `<object>.<id>.<port>`

> EXAMPLE:

>> To connect dpni.8 to dpsw.0.0:

>>> $ restool dprc connect dprc.1 `--endpoint1=dpsw.0.0 --endpoint2=dpni.8`

**`disconnect`**
: removes the link between two objects. Either endpoint can be specified as the target of the operation.

> Usage: restool dprc disconnect `<parent-container> --endpoint=<object>`

>> `<parent-container>`

>>> Specifies the parent-container.

>> `--endpoint=<object>`

>>> Specifies either endpoint of a connection.

> NOTES:

>> `-<parent-container>` must be an ancestor of the `<object>`

>>> (for example the root container)

> EXAMPLE:

>> To disconnect dpni.8:

>>> $ restool dprc disconnect dprc.1 `--endpoint=dpni.8`

**generate-dpl**
: generate DPL syntax for the specified container.

> Usage: restool dprc generate-dpl <container>

>> `<container>` specifies the name of the container

> NOTES:

>> Generates the DPL syntax for the specified container to stdout,

>> including all child and decendant containers.

> EXAMPLE:

>> Generate a DPL for dprc.1:

>>> $ restool dprc generate-dpl dprc.1

# DPNI
Usage: restool dpni `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPNI object.

**create**
: creates a new child DPNI under the root DPRC.

> Usage: restool dpni create [OPTIONS]

> OPTIONS:

>> `--options=<options-mask>`

>>> Where `<options-mask>` is a comma or space separated list of DPNI options:

>>>> DPNI_OPT_TX_FRM_RELEASE

>>>> DPNI_OPT_NO_MAC_FILTER

>>>> DPNI_OPT_HAS_POLICING

>>>> DPNI_OPT_SHARED_CONGESTION

>>>> DPNI_OPT_HAS_KEY_MASKING

>>>> DPNI_OPT_NO_FS

>>>> DPNI_OPT_HAS_OPR

>>>> DPNI_OPT_OPR_PER_TC

>>>> DPNI_OPT_SINGLE_SENDER

>>>> DPNI_OPT_CUSTOM_CG

>>>> DPNI_OPT_SHARED_HASH_KEY

>>>> DPNI_OPT_SHARED_FS

>>>> DPNI_OPT_STASHING_DIS

>> `--num-queues=<number>`

>>> Where `<number>` is the number of TX/RX queues use for traffic distribution.

>>> Used to distribute traffic to multiple GPP cores.

>>> Default is one queue. Maximim supported value is 16.

>> `--num-tcs=<number>`

>>> where `<number>` is the number of traffic classes (TCs), reserved for the DPNI.

>>> Defaults to one TC. Maximum supported value is 16.

>>> There are maximum 16 TCs for Tx and 8 TCs for Rx.

>>> When num_tcs > 8, Tx will use this value but Rx will have

>>> only 8 traffic classes.

>> `--mac-filter-entries=<number>`

>>> Where `<number>` is the number of entries in the MAC address filtering table.

>>> Allows both unicast and multicast entries.

>>> By default, there are 80 entries.Maximum supported value is 80.

>>> This option is the former `--mac-entries` which can still be used.

>>> The change was made to align with the options passed through DPL.

>> `--vlan-filter-entries=<number>`

>>> Number of entries in the VLAN address filtering table

>>> By default, VLAN filtering is disabled. Maximum values is 16

>>> This option is the former `--vlan-entries` which can still be used.

>>> The change was made to align with the options passed through DPL.

>> `--qos-entries=<number>`

>>> Where `<number>` is the number of entries in the QoS classification table.

>>> Ignored of DPNI has a single TC. By default, set to 64.

>> `--fs-entries=<number>`

>>> where `<number>` is the number of entries in the flow steering table.

>>> Defaults to 64. Maximum value is 1024

>> `--num-cgs=<number>`

>>> Where `<number>` is the number of congestion groups (CGs), reserved for the DPNI.

>>> Defaults to one per TC. Maximum supported value is 128.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> `--dist-key-size=<number>`

>>> Where `<number>` is the maximum key size for the distribution.

>>> `'0'` will be treated as `'24'` which enough for IPv4 5-tuple.

>> `--num-channels=<number>`

>>> Where `<number>` is the number of egress channels used by this dpni object.

>>> Valid range is [1-32]. Defaults to 1.

>> `--num-opr=<number>`

>>> Where `<number>` is the desired custom number of order point records when DPNI_OPT_CUSTOM_OPR is set.

>>> Maximum supported value is num_tcs * num_queues, which is also the default value.

**destroy**
: destroys a child DPNI under the root DPRC.

**update**
: update attributes of already created DPNI.

> Usage: restool dpni update dpni.X [OPTIONS]

> OPTIONS:

>> `--mac-addr=<addr>`

>> Where `<addr>` is a string specifying primary MAC address

>>> (e.g. 00:00:05:00:00:05).

# DPIO
Usage: restool dpio `<command> [--help] [ARGS...]`, where `<command>` can be:

`info`
: displays detailed information about a DPIO object.

**create**
: creates a new child DPIO under the root DPRC.

> Usage: restool dpio create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPIO by default options

>> `--channel-mode=<mode>`

>>> Where `<mode>` is one of:

>>>> DPIO_LOCAL_CHANNEL

>>>> DPIO_NO_CHANNEL

>>> Default value is DPIO_LOCAL_CHANNEL

>> `--num-priorities=<number>`

>>> Valid values for `<number>` are 1-8. Default value is 8.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLE:

>> Create a DPIO object with all default options:

>>> $ restool dpio create

**destroy**
: destroys a child DPIO under the root DPRC.

# DPSW
Usage: restool dpsw `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPSW object.

**create**
: creates a new child DPSW under the root DPRC.

> Usage: restool dpsw create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPSW by default options

>> `--num-ifs=<number>`

>>> Where `<number>` is the number of external and internal interfaces.

>> `--options=<options-mask>`

>>> Where `<options-mask>` is a comma separated list of DPSW options:

>>>> DPSW_OPT_FLOODING_DIS

>>>> DPSW_OPT_MULTICAST_DIS

>>>> DPSW_OPT_CTRL_IF_DIS

>>>> DPSW_OPT_FLOODING_METERING_DIS

>>>> DPSW_OPT_METERING_EN

>>>> DPSW_OPT_LAG_DIS

>> `--max-vlans=<number>`

>>> Where `<number>` is the maximum number of VLAN's. Default is 16.

>> `--max-fdbs=<number>`

>>> Where `<number>` is the maximum Number of FDB's. Default is 16.

>> `--max-fdb-entries=<number>`

>>> Where `<number>` is the number of FDB entries. Default is 1024.

>> `--fdb-aging-time=<number>`

>>> where `<number>` is the default FDB aging time in seconds. Default is 300 seconds.

>> `--max-fdb-mc-groups=<number>`

>>> Where `<number>` is the number of multicast groups in each FDB table. Default is 32.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLE:

>> Create a DPSW object with all default options:

>>> $ restool dpsw create

**destroy**
: destroys a child DPSW under the root DPRC.

**update**
: configure a child DPSW under the root DPRC.

> Usage: restool dpsw update `<dpsw-object> [OPTIONS]`

> OPTIONS:

>> `--if-id=<number>`

>>> Specifies the number of the interface to configure.

>>> If not specified, configure all interfaces.

>> `--taildrop=<0|1>`

>>> Used to enable or disable the feature.

>> `--units=<units>`

>>> Specifies taildrop units: `BYTES\FRAMES\BUFFERS`

>> `--threshold=<number>`

>>> Specifies taildrop threshold

# DPBP
Usage: restool dpbp `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPBP object.

**create**
: creates a new child DPBP under the root DPRC.

> Usage: restool dpbp create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPBP by default options

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

**destroy**
: destroys a child DPBP under the root DPRC.

# DPCI
Usage: restool dpci `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPCI object.

**create**
: creates a new child DPCI under the root DPRC.

> Usage: restool dpci create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPCI by default options

>> `--options=<options-mask>`

>>> Where `<options-mask>` is a comma or space separated list of DPCI options:

>>>> DPCI_OPT_HAS_OPR

>>>> DPCI_OPT_OPR_SHARED

>> `--num-priorities=<number>`

>>> Specifies the number of priorities

>>> Valid values are 1-2. Default value is 1

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLES:

>> Create a DPCI object with all default options:

>>> $ restool dpci create

>> Create a DPCI object with 2 priorities:

>>> $ restool dpci create `--num-priorities=2`

**destroy**
: destroys a child DPCI under the root DPRC.

# DPCON
Usage: restool dpcon `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPCON object.

**create**
: creates a new child DPCON under the root DPRC.

> Usage: restool dpcon create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPCON by default options

>> `--num-priorities=<number>`

>>> Valid values for `<number>` are 1-8. Default value is 2.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLES:

>> Create a DPCON object with all default options:

>>> $ restool dpcon create

>> Create a DPCON with 2 priorities:

>>> $ restool dpcon create `--num-priorities=2`

**destroy**
: destroys a child DPCON under the root DPRC.

# DPSECI
Usage: restool dpseci `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPSECI object.

**create**
: creates a new child DPSECI under the root DPRC.

> Usage: restool dpseci create `--num-queues=<count> --priorities=<pri1,pri2,...>` [OPTIONS]

>> `--num-queues=<number of rx/tx queues>`, ranges from 1 to 16

>> `--priorities=<priority1,priority2, ...,priority8>`

>>> DPSECI supports num-queues priorities that can be individually set.

>> if `--num-queues=3`, then `--priorities=X,Y,Z`

>>> Valid values for `<priorityN>` are 1-8.

>> `--num-queues and --priorities` must both be specified

> OPTIONS:

>> if options are not specified, create DPSECI by default options

>> `--options=<optionm-mask>`

>>> Where `<options-mask>` is a comma or space separated list of DPSECI options:

>>>> DPSECI_OPT_OPR_SHARED

>>>> DPSECI_OPT_HAS_OPR

>>>> DPSECI_OPT_HAS_CG

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLE:

>> Create a DPSECI with 2 rx/tx queues, 2,4 priorities:

>>> $ restool dpseci create `--num-queues=2 --priorities=2,4`

**destroy**
: destroys a child DPSECI under the root DPRC.

# DPDMUX
Usage: restool dpdmux `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPDMUX object.

**create**
: creates a new child DPDMUX under the root DPRC.

> Usage: restool dpdmux create `--num-ifs=<number>` [OPTIONS]

>> `--num-ifs=<number>`

>>>  Number of virtual interfaces (excluding the uplink interface)

> OPTIONS:

>> `--method=<dmat-method>`

>>> Where `<dmat-method>` defines the method of the DPDMUX address table.

>>> A valid value is one of the following:

>>>> DPDMUX_METHOD_NONE

>>>> DPDMUX_METHOD_C_VLAN_MAC

>>>> DPDMUX_METHOD_MAC

>>>> DPDMUX_METHOD_C_VLAN

>>>> DPDMUX_METHOD_CUSTOM

>>>> Default is DPDMUX_METHOD_C_VLAN_MAC

>> `--manip=<manip>`

>>> Where <manip> defines the DPDMUX required manipulation operation.

>>> A valid value is one of the following:

>>>> DPDMUX_MANIP_NONE

>>>> Default is DPDMUX_MANIP_NONE

>> `--options=<options-mask>`

>>> Where <options-mask> is a comma separated list of DPDMUX options:

>>>> DPDMUX_OPT_BRIDGE_EN

>>>> DPDMUX_OPT_CLS_MASK_SUPPORT

>>>> Default is 0

>> `--default-if=<if-id-number>`

>>> Desired default interface ID. Default is 0 (no default interface). Maximum num_ifs.

>> `--max-dmat-entries=<number>`

>>> Maximum entries in DPDMUX address table. Default is 64.

>> `--max-mc-groups=<number>`

>>> Number of multicast groups in DPDMUX address table. Default is 32 groups.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>> If it is not specified, the new object will be created under the default dprc.

**destroy**
: destroys a child DPDMUX under the root DPRC.

# DPMCP
Usage: restool dpmcp `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPMCP object.

**create**
: creates a new child DPMCP under the root DPRC.

> Usage: restool dpmcp create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPMCP by default options

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> If it is not specified, the new object will be created under the default dprc.

**destroy**
: destroys a child DPMCP under the root DPRC.

# DPMAC
Usage: restool dpmac `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPMAC object.

**create**
: creates a new child DPMAC under the root DPRC.

> Usage: restool dpmac create `--mac-id=<number>` [OPTIONS]

>> `--mac-id=<number>`

>>> Where `<number>` specifies the id of the hardware MAC associated with the DPMAC object.

> OPTIONS:

>> if options are not specified, create DPMAC by default options

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>>  If it is not specified, the new object will be created under the default dprc.

> EXAMPLE:

>> create a DPMAC with specified mac id:

>>> $ restool dpmac create --mac-id=4

**destroy**
: destroys a child DPMAC under the root DPRC.

# DPDCEI
Usage: restool dpcei `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPDCEI object.

**create**
: creates a new child DPDCEI under the root DPRC.

> Usage: restool dpdcei create `--engine=<engine> --priority=<number>` [OPTIONS]

>> `--engine=<engine>`

>>> compression or decompression engine to be selected.

>>> A valid value is one of the following:

>>>> DPDCEI_ENGINE_COMPRESSION, DPDCEI_ENGINE_DECOMPRESSION

>> `--priority=<number>`

>>> Priority for DCE hardware processing (valid values 1-8)

> OPTIONS:

>> if options are not specified, create DPDCEI by default options

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> If it is not specified, the new object will be created under the default dprc.

**destroy**
: destroys a child DPDCEI under the root DPRC.

# DPAIOP
Usage: restool dpaiop `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPAIOP object.

**create**
: creates a new child DPAIOP under the root DPRC.

> Usage: restool dpaiop create `--aiop-container=<container-name>` [OPTIONS]

>> `--aiop-container=<container-name>`

>>> Specifies the AIOP container name, e.g. dprc.3, dprc.4, etc.

> OPTIONS:

>> if options are not specified, create DPAIOP by default options.

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLE:

>> create a DPAIOP

>>> $ restool dpaiop create `--aiop-container=dprc.3`

**destroy**
: destroys a child DPAIOP under the root DPRC.

# DPRTC
Usage: restool dprtc `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPRTC object.

**create**
: creates a new child DPRTC under the root DPRC.

> Usage: restool dprtc create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPRTC by default options

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> If it is not specified, the new object will be created under the default dprc.

**destroy**
: destroys a child DPRTC under the root DPRC.

# DPDMAI
Usage: restool dpdmai `<command> [--help] [ARGS...]`, where `<command>` can be:

**`info`**
: displays detailed information about a DPDMAI object.

**create**
: creates a new child DPDMAI under the root DPRC.

> Usage: restool dpdmai create [OPTIONS]

> OPTIONS:

>> if options are not specified, create DPDMAI by default options

>>> default is: restool dpdmai create `--priorities=1,2`

>> `--priorities=<priority1,priority2>`

>>> Valid values for `<priorityN>` are 1-8.

>> `--num-queues=<number>`

>>> Valid values are [1, #num-cores]

>> `--container=<container-name>`

>>> Specifies the parent container name. e.g. dprc.2, dprc.3 etc.

>> If it is not specified, the new object will be created under the default dprc.

> EXAMPLES:

>> create a DPDMAI object with all default options:

>>> $ restool dpdmai create

>> create a DPDMAI with 2,4 priorities:

>>> $ restool dpdmai create `--priorities=2,4`


**destroy**
: destroys a child DPDMAI under the root DPRC.
