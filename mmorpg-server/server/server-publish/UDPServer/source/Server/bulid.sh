#!/bin/bash
. env.sh

cd UdpConnectServer
make clean;
make -f Makefile_release;
make install;
cd ..

cd LogServer;
make clean;
make -f Makefile_release;
make install;
cd ..;