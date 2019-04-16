#!/bin/bash
# Copyright (c) 2017-2019 The Swipp developers <info@swippcoin.com>
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING.daemon or http://www.opensource.org/licenses/mit-license.php.

RES=$(ps -eo pid,fname | grep swipp)
PID=${RES% *}
BLOCKFILE_SOURCE_PATTERN=~/.swipp/blk
BLOCKFILE_DEST=/var/www/html/binaries/bootstrap.dat

if [ -z "$PID" ]; then
	exit 1;
else
	kill -TSTP $PID
	sync
	cat $BLOCKFILE_SOURCE_PATTERN* >| ${BLOCKFILE_DEST}.tmp
	kill -CONT $PID
fi

bzip2 -9 ${BLOCKFILE_DEST}.tmp

RES=$(sha256sum ${BLOCKFILE_DEST}.tmp.bz2)
HASH=${RES% *}

if [ -z "$PID" ]; then
	exit 1;
else
	cat <(printf $HASH) ${BLOCKFILE_DEST}.tmp.bz2 > $BLOCKFILE_DEST.bsa
	rm ${BLOCKFILE_DEST}.tmp.bz2
fi

