#!/bin/bash
# Copyright (c) 2018 Nexell Co., Ltd.
# Author: Junghyun, Kim <jhkim@nexell.co.kr>

BASEDIR="$(cd "$(dirname "$0")" && pwd)"
RESULTDIR="$BASEDIR/../../result"

PARTMAP_FILE=
PARTMAP_CONTEXT=()
PARTMAP_TARGETS=()

function usage () {
	echo "usage: `basename $0` -f [partmap file] <targets> <options>"
	echo ""
	echo "[OPTIONS]"
	echo "  -d : image path for fastboot, default: `readlink -e -n $RESULTDIR`"
	echo "  -i : partmap info"
	echo "  -l : listup target in partmap list"
	echo "  -r : send reboot command after end of fastboot"
	echo ""
	echo "Partmap struct:"
	echo "  fash=<device>,<device number>:<target>:<device area>:<start:hex>,<size:hex>:<image>\""
	echo "  device      : support 'mmc','spi'"
	echo "  device area : for 'mmc' = 'bootsector', 'raw', 'partition', 'gpt', 'mbr'"
	echo "              : for 'spi' = 'raw'"
	echo ""
	echo "Fastboot command:"
	echo "  1. sudo fastboot flash partmap <partmap.txt>"
	echo "  2. sudo fastboot flash <target> <image>"
	echo ""
}

function parse_targets () {
	local value=$1	# $1 = store the value
	local params=("${@}")
	local images=("${params[@]:1}")	 # $3 = search array

	for i in "${images[@]}"
	do
		local val="$(echo $i| cut -d':' -f 2)"
		eval "${value}+=(\"${val}\")"
	done
}

function do_fastboot () {
	partmap_images=()

	for i in "${PARTMAP_TARGETS[@]}"
	do
		image=""
		for n in "${PARTMAP_CONTEXT[@]}"
		do
			local v="$(echo $n| cut -d':' -f 2)"
			if [ "$i" == "$v" ]; then
				image="$(echo $(echo $n| cut -d':' -f 5)| cut -d';' -f 1)"
				[ -z "$image" ] && continue;
				image="$RESULTDIR/$image"
				break;
			fi
		done

		[ -z "$image" ] && continue;

		if [ ! -f "$image" ]; then
			image=./$(basename $image)
			if [ ! -f "$image" ]; then
				echo -e "\033[47;31m Not found '$i': $image\033[0m"
				continue
			fi
		fi
		partmap_images+=("$i:`readlink -e -n "$image"`");
	done

	echo -e "\033[0;33m Partmap: $PARTMAP_FILE\033[0m"
	sudo fastboot flash partmap $PARTMAP_FILE
	[ $? -ne 0 ] && exit 1;

	for i in ${partmap_images[@]}
	do
		target=$(echo $i| cut -d':' -f 1)
		image=$(echo $i| cut -d':' -f 2)

		echo -e "\033[0;33m $target: $image\033[0m"
		sudo fastboot flash $target $image
		[ $? -ne 0 ] && exit 1;
	done
}

SEND_REBOOT=false

case "$1" in
	-f )
		PARTMAP_FILE=$2
		partmap_lists=()
		args=$# options=0 counts=0

		if [ ! -f $PARTMAP_FILE ]; then
			echo -e "\033[47;31m No such to partmap: $PARTMAP_FILE \033[0m"
			exit 1;
		fi

		while read line;
		do
			if [[ "$line" == *"#"* ]];then
				continue
			fi

			PARTMAP_CONTEXT+=($line)
		done < $PARTMAP_FILE

		parse_targets partmap_lists "${PARTMAP_CONTEXT[@]}"

		while [ "$#" -gt 2 ]; do
			# argc
			for i in "${partmap_lists[@]}"
			do
				if [ "$i" == "$3" ]; then
					PARTMAP_TARGETS+=("$i");
					shift 1
					break
				fi
			done

			case "$3" in
			-d )	RESULTDIR=$4; ((options+=2)); shift 2;;
			-r ) 	SEND_REBOOT=true; ((options+=1)); shift 1;;
			-l )
				echo -e "\033[0;33m------------------------------------------------------------------ \033[0m"
				echo -en " Partmap targets: "
				for i in "${partmap_lists[@]}"
				do
					echo -n "$i "
				done
				echo -e "\n\033[0;33m------------------------------------------------------------------ \033[0m"
				exit 0;;
			-i )
				echo -e "\033[0;33m------------------------------------------------------------------ \033[0m"
				for i in "${PARTMAP_CONTEXT[@]}"
				do
					KB=$((1024)) MB=$((1024 * 1024)) GB=$((1024 * 1024 * 1024))
					val="$(echo "$(echo "$i" | cut -d':' -f4)" | cut -d',' -f2)"

					if [[ $val -ge "$GB" ]]; then
						len="$((val/$GB)) GB"
					elif [[ $val -ge "$MB" ]]; then
						len="$((val/$MB)) MB"
					else
						len="$((val/$KB)) KB"
					fi

					echo -e "$i [$len]"
				done
				echo -e "\033[0;33m------------------------------------------------------------------ \033[0m"
				exit 0;;
			-e )
				vim $PARTMAP_FILE
				exit 0;;
			-h )	usage;	exit 1;;
			* )
				if [ $((counts+=1)) -gt $args ]; then
					break;
				fi
				;;
			esac
		done

		((args-=2))
		num=${#PARTMAP_TARGETS[@]}
		num=$((args-num-options))

		if [ $num -ne 0 ]; then
			echo -e "\033[47;31m Unknown target: $PARTMAP_FILE\033[0m"
			echo -en " Check targets: "
			for i in "${partmap_lists[@]}"
			do
				echo -n "$i "
			done
			echo ""
			exit 1
		fi

		if [ ${#PARTMAP_TARGETS[@]} -eq 0 ]; then
			PARTMAP_TARGETS=(${partmap_lists[@]})
		fi

		do_fastboot
		;;
	-r )
		SEND_REBOOT=true; shift 1;;
	-h | * )
		usage;
		exit 1;;
esac

if [ $SEND_REBOOT == true ]; then
	echo -e "\033[47;30m send reboot command...\033[0m"
	sudo fastboot reboot
fi
