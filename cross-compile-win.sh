#!/bin/bash

if [ ! -d "$1" ]; then
	echo ERROR: The directory $1 does not exist
	exit 1
fi

export PATH=$1/usr/bin:$PATH
export PATH=$1/usr/i686-w64-mingw32.static/bin:$PATH
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
	CURL_LIB_PATH=$MXE_LIB_PATH \
	QMAKE_LRELEASE=$1/usr/i686-w64-mingw32.static/qt5/bin/lrelease swipp.pro

cd src/leveldb
TARGET_OS=NATIVE_WINDOWS make -j8 out-static/libleveldb.a out-static/libmemenv.a \
	CC=$1/usr/bin/i686-w64-mingw32.static-gcc \
	CXX=$1/usr/bin/i686-w64-mingw32.static-g++

cd ../secp256k1
./autogen.sh
./configure --host=i686-w64-mingw32.static
make -j8

cd ../..
make -j8 -f Makefile.Release
