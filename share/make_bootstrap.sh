#!/bin/bash
# Copyright (c) 2017-2019 The Swipp developers <info@swippcoin.com>
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

PID=$(ps -eo pid,fname | grep swipp)
PID=${PID% *}
BLOCKFILE_SOURCE_PATTERN=~/.swipp/blk
BLOCKFILE_DEST=/var/www/html/binaries/bootstrap

if [ -z "$PID" ]; then
	exit 1;
else
	kill -TSTP $PID
	sync
	cat $BLOCKFILE_SOURCE_PATTERN* >| ${BLOCKFILE_DEST}.dat.tmp
	kill -CONT $PID
fi

xz -T0 --check=crc32 --lzma2=dict=16MiB,nice=273,depth=512 ${BLOCKFILE_DEST}.dat.tmp

HASH=$(sha256sum ${BLOCKFILE_DEST}.dat.tmp.xz)
HASH=${HASH% *}

if [ -z "$PID" ]; then
	exit 1;
else
	cat <(printf $HASH) ${BLOCKFILE_DEST}.dat.tmp.xz > $BLOCKFILE_DEST.bsa
	rm ${BLOCKFILE_DEST}.dat.tmp.xz
fi

