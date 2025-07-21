#!/bin/sh

BIN_PATH=../
mkdir -p tmp
STARTPID_FILE=tmp/START.PID


#start one service 
function StartServer()
{
    SERVER_DIR=${BIN_PATH}$1
    SERVER_NAME=$2
    SERVER_BIN=$1/${SERVER_NAME}
    SERVER_START_FLAG=
    if [ $# -gt 2 ]; then
        SERVER_START_FLAG=_$3
    fi
    
    CURRENT_PATH=`pwd` 
    cd ${SERVER_DIR}

    PID_FILE=${SERVER_NAME}.pid

    isrun=0
    if [ -f ${PID_FILE} ] ; then
        sh_pid=`cat ${PID_FILE}`
        isrun=`ps -p${sh_pid}|wc -l`
    fi

    if [ ${isrun} -lt 2 ]; then
	chmod +x ${SERVER_NAME}
        ./${SERVER_NAME} --UNIQUE ${SERVER_START_FLAG} &
        sleep 5 

        isrun=0
        if [ -f ${PID_FILE} ] ; then
             sh_pid=`cat ${PID_FILE}`
             isrun=`ps -p ${sh_pid}|wc -l`
        fi

        if [ ${isrun} -lt 2 ]; then
            echo "${SERVER_BIN} start failed..."
        else
            echo "${SERVER_BIN} started..."
        fi
    else
        echo "${SERVER_BIN} is already running ..."
    fi

    cd ${CURRENT_PATH}
   
}

source ./server_list.sh
server_dir_len=${#SERVER_DIR_LIST[@]}
for((i=0;i<server_dir_len;i++))
do
	StartServer ${SERVER_DIR_LIST[$i]} ${SERVER_NAME_LIST[$i]}
done
