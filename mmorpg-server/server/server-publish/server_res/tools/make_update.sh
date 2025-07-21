#!/bin/sh

source config.sh

DEST_DIR=dest
TMP_DIR=dest/tmp

mkdir -p ${DEST_DIR}
mkdir -p ${TMP_DIR}
rm -r -f ${TMP_DIR}/*


#打包Bin
BIN_DIR=${TMP_DIR}/Bin

mkdir -p ${BIN_DIR}
cp -r Bin/*Server     ${BIN_DIR}
cp -r Bin/*Server.dbg ${BIN_DIR}

#打包其他
CONFIG_DIR=${TMP_DIR}/Config
mkdir -p ${CONFIG_DIR}
cp -r Config/*  ${CONFIG_DIR}
rm -r -f ${CONFIG_DIR}/.svn ${CONFIG_DIR}/*/.svn ${CONFIG_DIR}/*/*/.svn ${CONFIG_DIR}/*/*/*/.svn

#打包sql
SQL_DIR=${TMP_DIR}/Sql
mkdir -p ${SQL_DIR}
cp -r Sql/*     ${SQL_DIR}
rm -r -f ${SQL_DIR}/.svn ${SQL_DIR}/*/.svn ${SQL_DIR}/*/*/.svn ${SQL_DIR}/*/*/*/.svn

cp start.sh     ${TMP_DIR}
cp stop.sh      ${TMP_DIR}

#打包tools
TOOLS_DIR=${TMP_DIR}/tools
mkdir -p ${TOOLS_DIR}
cp -r tools/*   ${TOOLS_DIR}
rm -r -f ${TOOLS_DIR}/.svn ${TOOLS_DIR}/*/.svn

#压缩
rm  -f ${DEST_DIR}/update.zip
zip -r ${DEST_DIR}/update.zip ${TMP_DIR}/*

#删除临时文件
rm  -r -f ${TMP_DIR}


