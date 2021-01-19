#!/bin/bash

# Copyright 2021 NXP

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the above-listed copyright holders nor the
# names of any contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.


# ALTERNATIVELY, this software may be distributed under the terms of the
# GNU General Public License ("GPL") as published by the Free Software
# Foundation, either version 2 of that License or (at your option) any
# later version.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

restool_completion()
{
	local dpaa2_objects=$(restool --help\
			      | grep "Valid <object-type> values: " -A1\
			      | grep -o dp[a-z]*)
	local dpaa2_object=${COMP_WORDS[1]}

	case ${#COMP_WORDS[@]} in
		2)
			COMPREPLY=($(compgen -W "$dpaa2_objects"\
				     "${COMP_WORDS[1]}"))
			;;
		3)
			local commands=$(restool $dpaa2_object --help\
					 | grep info -A2\
					 | awk '{ print $1 }')
			if [ "$dpaa2_object" == "dpni" ]; then
				commands+="	update"
			fi
			COMPREPLY=($(compgen -W "$commands" "${COMP_WORDS[2]}"))
			;;
		4)
			if [[ "${COMP_WORDS[2]}" != "info" ]]; then
				return
			fi
			instances=$(ls /sys/bus/fsl-mc/devices\
				    | grep $dpaa2_object)
			COMPREPLY=($(compgen -W "$instances"\
				     "${COMP_WORDS[3]}"))
			;;
		*)
			return
			;;
	esac
}

ls-delete_completion()
{
	if [ "${#COMP_WORDS[@]}" != "2" ]; then
		return
	fi

	local objects=("dpni" "dpsw")
	local instances=("all")

	for i in ${objects[@]}
	do
		instances+=($(ls /sys/bus/fsl-mc/devices | grep $i))
	done

	COMPREPLY=($(compgen -W "${instances[*]}" "${COMP_WORDS[1]}"))
}

instances=()
get_instances()
{
	local object=$1
	local object_instances=$(ls /sys/bus/fsl-mc/devices | grep $object)
	for i in $object_instances
	do
		endpoint=$(restool $object info "$i"\
			   | grep "endpoint:"\
			   | grep -o "No object associated")
		if [[ ! -z "$endpoint" ]]; then
			instances+=("$i")
		fi
	done
}

ls-addni_completion()
{
	if [ "${#COMP_WORDS[@]}" != "2" ]; then
		return
	fi

	instances=()
	get_instances "dpmac"
	get_instances "dpni"

	COMPREPLY=($(compgen -W "${instances[*]}" "${COMP_WORDS[1]}"))
}

ls-addmux_completion()
{
	if [[ "${COMP_WORDS[-2]}" =~ dpmac.[1-8] ]]; then
		return
	fi

	instances=()
	get_instances "dpmac"

	COMPREPLY=($(compgen -W "${instances[*]}" "${COMP_WORDS[-1]}"))
}

ls-addsw_completion()
{
	instances=()
	get_instances "dpmac"
	get_instances "dpni"

	COMPREPLY=($(compgen -W "${instances[*]}" "${COMP_WORDS[-1]}"))
}

complete -F restool_completion restool
complete -F ls-delete_completion ls-delete
complete -F ls-addni_completion ls-addni
complete -F ls-addmux_completion ls-addmux
complete -F ls-addsw_completion ls-addsw
