#!/bin/bash
# Copyright (c) 2017 The Swipp developers
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

SWIPP_BINARY=src/swippd
ARGS="-debug -testnet -datadir="
TMP_TEMPLATE=/tmp/swipp.XXXXXXX

uuid() {
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1
}

waitpid() {
	tail --pid=$1 -f /dev/null
}

pidexists() {
	kill -0 $1 &> /dev/null
}

start_swipp_exe () {
	dir=$(mktemp -d $TMP_TEMPLATE)

	user=$(uuid)
	echo $user > $dir/.user

	password=$(uuid)
	echo $password > $dir/.password

	extraargs="-rpcuser=$user -rpcpassword=$password "
	extraargs+="-externalip=$2 -bind=$2:$3 -rpcport=$4"
	$1$dir $extraargs &
	pid=$!

	echo $pid > $dir/.pid
	sleep 2

	if ! pidexists $pid; then
		echo Failed to start swipp instance "$1$dir $extrargs" Aborting startup... > /dev/null
		stop
		exit 1
	fi
}

start () {
	echo Starting Swipp instances... > /dev/null
	start_swipp_exe "$SWIPP_BINARY $ARGS" 127.0.10.1 18065 15075
	start_swipp_exe "$SWIPP_BINARY $ARGS" 127.0.10.2 18065 15076
}

stop () {
	echo Stopping all running Swipp instances... > /dev/null


	if ls /tmp/swipp.* 1> /dev/null 2>&1; then
		for i in /tmp/swipp.*/.pid; do
			kill $(<$i) &> /dev/null # We don't care if the kill fails
			waitpid $(<$i)
		done
	fi

	echo Deleting data directories... > /dev/null
	rm -rf /tmp/swipp.*
}

run () {
	echo "Running..." > /dev/null
}

set -o xtrace

case $1 in
	start)
	start
	;;

	stop)
	stop
	;;

	run)
	run
	;;

	# unknown option
	*)
	echo Please specify one of the following; start/stop/run. Exiting...
	exit 1
	;;
esac

exit 0
