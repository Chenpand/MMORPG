#!/bin/bash

BIN_PATH=../
mkdir -p tmp
STARTPID_FILE=tmp/START.PID

function StopServer()
{
    SERVER_DIR=${BIN_PATH}$1
    SERVER_NAME=$2
    SERVER_BIN=$1/${SERVER_NAME}

    CURRENT_PATH=`pwd`
    cd ${SERVER_DIR}

    PID_FILE=${SERVER_NAME}.pid

    isrun=0
    if [ -f ${PID_FILE} ] ; then
        sh_pid=`cat ${PID_FILE}`
        isrun=`ps -p${sh_pid}|wc -l`
    fi

    if [ ${isrun} -lt 2 ]; then
	echo "${SERVER_BIN} is already stopped..."
    else
	kill $sh_pid
	sleep 1

	isrun=0
        if [ -f ${PID_FILE} ] ; then
             sh_pid=`cat ${PID_FILE}`
             isrun=`ps -p ${sh_pid}|wc -l`
        fi

        if [ ${isrun} -lt 2 ]; then
            echo "${SERVER_BIN} stopped..."
        else
            echo "${SERVER_BIN} stop failed..."
        fi
    fi

    cd ${CURRENT_PATH}

}

source ./server_list.sh
server_dir_len=${#SERVER_DIR_LIST[@]}
for((i=0;i<server_dir_len;i++))
do
        StopServer ${SERVER_DIR_LIST[$i]} ${SERVER_NAME_LIST[$i]}
done
