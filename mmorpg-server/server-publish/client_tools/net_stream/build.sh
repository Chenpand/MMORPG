#########################################################################
# File Name: build.sh
# Author: etond
# mail: monkey_tv@126.com
# Created Time: Mon Mar  7 16:52:17 2016
#########################################################################
#!/bin/bash

cd proj.ios
xcodebuild
cp Build/Release-iphoneos/*.a ../lib/ios/
cd ../
echo "build success!!! "

