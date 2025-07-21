#########################################################################
# File Name: build.sh
# Author: etond
# mail: monkey_tv@126.com
# Created Time: Mon Apr 25 16:58:52 2016
#########################################################################
#!/bin/bash

cd proj.ios
xcodebuild > /dev/null
cp Build/Release-iphoneos/libsnappy.a ../lib/ios/
cd ../
echo "build ios success!!! "

cd proj.mac
xcodebuild > /dev/null
cp -r build/Release/snappy.bundle ../lib/mac/
cd ../
echo "build mac success!!! "

cd proj.android
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk
cp libs/armeabi/libsnappy.so ../lib/android/
cd ../
echo "build android success!!!"
