#########################################################################
# File Name: build.sh
# Author: etond
# mail: monkey_tv@126.com
# Created Time: Mon Mar  7 16:52:17 2016
#########################################################################
#!/bin/bash

cd proj.ios
xcodebuild > /dev/null
cp Build/Release-iphoneos/libenet.a ../lib/ios/
cd ../
echo "build ios success!!! "

cd proj.android
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk
cp libs/armeabi/libenet.so ../lib/android/
cd ../
echo "build android success!!!"
