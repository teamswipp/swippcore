#!/bin/bash

if [ ! -d "$1" ]; then
	echo ERROR: The directory $1 does not exist
	exit 1
fi

MXE_INCLUDE_PATH=$1/usr/i686-w64-mingw32.static/include
MXE_LIB_PATH=$1/usr/i686-w64-mingw32.static/lib

if [ ! -d "$MXE_LIB_PATH" ]; then
	echo ERROR: The directory $MXE_LIB_PATH does not exist
	exit 1
fi

i686-w64-mingw32.static-qmake-qt5 \
	BOOST_LIB_SUFFIX=-mt \
	BOOST_THREAD_LIB_SUFFIX=_win32-mt \
	BOOST_INCLUDE_PATH=$MXE_INCLUDE_PATH/boost \
	BOOST_LIB_PATH=$MXE_LIB_PATH \
	OPENSSL_INCLUDE_PATH=$MXE_INCLUDE_PATH/openssl \
	OPENSSL_LIB_PATH=$MXE_LIB_PATH \
	BDB_INCLUDE_PATH=$MXE_INCLUDE_PATH \
	BDB_LIB_PATH=$MXE_LIB_PATH \
	MINIUPNPC_INCLUDE_PATH=$MXE_INCLUDE_PATH \
	MINIUPNPC_LIB_PATH=$MXE_LIB_PATH \
	QMAKE_LRELEASE=$1/usr/i686-w64-mingw32.static/qt5/bin/lrelease swipp.pro

cd src/leveldb
TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a \
	CC=$1/usr/bin/i686-w64-mingw32.static-gcc \
	CXX=$1/usr/bin/i686-w64-mingw32.static-g++
make -j8 -f leveldb/Makefile

cd ../secp256k1
./auotgen.sh
./configure --host=i686-w64-mingw32.static
make -j8

cd ../..
make -j8 -f Makefile.Release
