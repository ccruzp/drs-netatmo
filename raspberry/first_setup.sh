#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

UNZIPPED_DEAMON="$SCRIPT_DIR/"


echo install needed packages
sudo apt-get install build-essential cmake python3-dev python2.7-dev
sudo dpkg -i NFC-Reader-Library-4.010-2.deb
sudo apt-get install cmake
pip install nxppy
pip3 install nxppy

echo "make sure spi authorized in configured !"


cd $UNZIPPED_DEAMON

mkdir _build 
cd _build
cmake ..


EXAMPLE_NAME="BasicDiscoveryLoop"
cd ../_build/Examples/NfcrdlibEx<$EXAMPLE_NAME
make 
