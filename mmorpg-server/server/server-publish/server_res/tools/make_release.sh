#!/bin/sh

source config.sh

DEST_DIR=dest
TMP_DIR=dest/tmp

mkdir -p ${DEST_DIR}
mkdir -p ${TMP_DIR}
rm -r -f ${TMP_DIR}/*

#打包AdminServer
ADMIN_DIR=${TMP_DIR}/AdminServer

mkdir -p ${ADMIN_DIR} 
mkdir -p ${ADMIN_DIR}/GameLog
mkdir -p ${ADMIN_DIR}/Log
cp  AdminServer/AdminServer.cfg ${ADMIN_DIR} 
cp  AdminServer/NetAddress.xml ${ADMIN_DIR}
cp  AdminServer/PlatformNetAddress.xml ${ADMIN_DIR}


#打包RecordServer
RECORD_DIR=${TMP_DIR}/RecordServer

mkdir -p ${RECORD_DIR}
mkdir -p ${RECORD_DIR}/Log
cp  RecordServer/RecordServer.cfg ${RECORD_DIR}


#打包GateServer
for((i=1;i<=${GATESERVER_NUM};i++))
do
    GATE_DIR=${TMP_DIR}/GateServer${i}

    mkdir -p ${GATE_DIR}
    mkdir -p ${GATE_DIR}/Log
    cp GateServer${i}/GateServer.cfg ${GATE_DIR}
done


#打包WorldServer
WORLD_DIR=${TMP_DIR}/WorldServer

mkdir -p ${WORLD_DIR}
mkdir -p ${WORLD_DIR}/Log
cp WorldServer/WorldServer.cfg ${WORLD_DIR}


#打包SceneServer
for((i=1;i<=${SCENESERVER_NUM};i++))
do 
    SCENE_DIR=${TMP_DIR}/SceneServer${i}

    mkdir -p ${SCENE_DIR}
    mkdir -p ${SCENE_DIR}/Log
    cp SceneServer${i}/SceneServer.cfg ${SCENE_DIR}
done

#打包CenterServer
CENTER_DIR=${TMP_DIR}/CenterServer

mkdir -p ${CENTER_DIR}
mkdir -p ${CENTER_DIR}/Log
cp CenterServer/CenterServer.cfg ${CENTER_DIR}

#打包SocialServer
SOCIAL_DIR=${TMP_DIR}/SocialServer

mkdir -p ${SOCIAL_DIR}
mkdir -p ${SOCIAL_DIR}/Log
cp SocialServer/SocialServer.cfg ${SOCIAL_DIR}

#打包ChallengeServer
CHALLENGE_DIR=${TMP_DIR}/ChallengeServer

mkdir -p ${CHALLENGE_DIR}
mkdir -p ${CHALLENGE_DIR}/Log
cp ChallengeServer/ChallengeServer.cfg ${CHALLENGE_DIR}


#打包WSceneServer
for((i=1;i<=${WSCENESERVER_NUM};i++))
do 
    WSCENE_DIR=${TMP_DIR}/WSceneServer${i}

    mkdir -p ${WSCENE_DIR}
    mkdir -p ${WSCENE_DIR}/Log
    cp WSceneServer${i}/SceneServer.cfg ${WSCENE_DIR}
    cp WSceneServer${i}/NetAddress.xml ${WSCENE_DIR}
done


#打包Bin
BIN_DIR=${TMP_DIR}/Bin

mkdir -p ${BIN_DIR}
#cp -r Bin/*Server     ${BIN_DIR}
#cp -r Bin/*Server.dbg ${BIN_DIR}

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

cp Database.xml ${TMP_DIR}
cp Game.cfg     ${TMP_DIR}

cp config.sh    ${TMP_DIR}
cp start.sh     ${TMP_DIR}
cp stop.sh      ${TMP_DIR}
cp db_conf.sh	${TMP_DIR}

#打包tools
TOOLS_DIR=${TMP_DIR}/tools
mkdir -p ${TOOLS_DIR}
cp -r tools/*   ${TOOLS_DIR}
rm -r -f ${TOOLS_DIR}/.svn ${TOOLS_DIR}/*/.svn


#压缩
rm  -f ${DEST_DIR}/release.zip
zip -r ${DEST_DIR}/release.zip ${TMP_DIR}/*

#删除临时文件
rm  -r -f ${TMP_DIR}



