#!/bin/bash
#
# Copyright (c) 2017-2018 The Swipp developers
#
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Recipe for creating a Swipp QT AppImage package

# Create AppDir FHS-like stucture
mkdir -p swipp.AppDir/usr swipp.AppDir/usr/bin

# Used by AppImageKit-checkrt (see below)
mkdir -p swipp.AppDir/usr/optional swipp.AppDir/usr/optional/libstdc++

# Copy files into empty AppDir
cp swipp-qt swipp.AppDir/usr/bin

# Get and run linuxdeployqt
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

# Prepare AppDir
./linuxdeployqt-continuous-x86_64.AppImage swipp.AppDir/usr/bin/swipp-qt -bundle-non-qt-libs

# Workaround to increase compatibility with older systems; see https://github.com/darealshinji/AppImageKit-checkrt for details
rm swipp.AppDir/AppRun
cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6 swipp.AppDir/usr/optional/libstdc++/
wget -c https://github.com/darealshinji/AppImageKit-checkrt/releases/download/continuous/exec-x86_64.so -O swipp.AppDir/usr/optional/exec.so
wget -c https://github.com/darealshinji/AppImageKit-checkrt/releases/download/continuous/AppRun-patched-x86_64 -O swipp.AppDir/AppRun
chmod a+x swipp.AppDir/AppRun

# Copy in desktop descriptor and icon
printf "[Desktop Entry]\nType=Application\nName=swipp-qt\nGenericName=swipp-qt\nComment=Store and transfer Swipp coins\nIcon=swipp\nExec=../usr/bin/swipp-qt\nTerminal=false\nCategories=Network;Finance;" > swipp.AppDir/swipp-qt.desktop
cp src/qt/res/icons/swipp.png swipp.AppDir/

# Manually invoke appimagetool so that the modified AppRun stays intact
./linuxdeployqt-continuous-x86_64.AppImage --appimage-extract
export PATH=$(readlink -f ./squashfs-root/usr/bin):$PATH
./squashfs-root/usr/bin/appimagetool swipp.AppDir swipp-qt-x86_64.AppImage

# Purge temporary files
rm -rf swipp.AppDir linuxdeployqt-continuous-x86_64.AppImage squashfs-root
