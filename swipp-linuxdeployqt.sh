#!/bin/bash
#
# Copyright (c) 2017-2018 The Swipp developers
#
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
#
# Recipe for creating a Swipp QT AppImage package

# Create AppDir FHS-like stucture
mkdir -p swipp.AppDir/usr
mkdir -p swipp.AppDir/usr/bin

# Copy files into empty AppDir
cp swipp-qt swipp.AppDir/usr/bin

# Get and run linuxdeployqt
wget -c https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

# Prepare AppDir
./linuxdeployqt-continuous-x86_64.AppImage ./swipp.AppDir/usr/bin/swipp-qt

# Copy in desktop descriptor and icon
printf "[Desktop Entry]\nType=Application\nName=swipp-qt\nGenericName=swipp-qt\nComment=Store and transfer Swipp coins\nIcon=swipp\nExec=/usr/bin/swipp-qt\nTerminal=false\nCategories=Network;Finance;" > swipp.AppDir/swipp-qt.desktop
cp src/qt/res/icons/swipp.png swipp.AppDir/

# Create executable AppImage
./linuxdeployqt-continuous-x86_64.AppImage ./swipp.AppDir/usr/bin/swipp-qt -appimage

# Purge temporary files
rm -rf swipp.AppDir linuxdeployqt-continuous-x86_64.AppImage

