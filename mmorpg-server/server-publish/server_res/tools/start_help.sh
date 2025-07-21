#!/bin/sh

BIN_PATH=../Bin
mkdir -p tmp
STARTPID_FILE=tmp/START.PID

ZONE_ID=$1
GATESERVER_NUM=$2
SCENESERVER_NUM=$3
PORT=$4

#start one service 
function StartServer()
{
    SERVER_CMD=$1
    SERVER_DIR=$2
    SERVER_ID=
    if [ $# -gt 2 ]; then
        SERVER_ID=_$3
    fi
    
    SERVER_BINFILE=${BIN_PATH}/${SERVER_CMD}
    SERVER_BIN=${BIN_PATH}/${SERVER_CMD}${PORT}_${ZONE_ID}${SERVER_ID}
 
    cd ${SERVER_DIR}

    PID_FILE=${SERVER_CMD}.pid

    isrun=0
    if [ -f ${PID_FILE} ] ; then
        sh_pid=`cat ${PID_FILE}`
        isrun=`ps -p${sh_pid}|wc -l`
    fi
    if [ ${isrun} -lt 2 ]; then
	chmod +x ${SERVER_BINFILE}
        rm -f ${SERVER_BIN}
        ln -s ${SERVER_BINFILE} ${SERVER_BIN}
        ./${SERVER_BIN} --UNIQUE& 
        sleep 5 

        isrun=0
        if [ -f ${PID_FILE} ] ; then
             sh_pid=`cat ${PID_FILE}`
             isrun=`ps -p${sh_pid}|wc -l`
        fi

        if [ ${isrun} -lt 2 ]; then
            echo "${SERVER_BIN} start failed..."
        else
            echo "${SERVER_BIN} started..."
        fi
    else
        echo "${SERVER_BIN} is already running ..."
    fi

    cd ..
   
}

#start all services
function StartAll()
{
    StartServer "AdminServer" "AdminServer"
    StartServer "RecordServer" "RecordServer"
    
    StartServer "WorldServer" "WorldServer"

    for((i=1;i<=${SCENESERVER_NUM};i++))
    do
        StartServer "SceneServer" "SceneServer${i}" "${i}"
    done

    for((i=1;i<=${GATESERVER_NUM};i++))
    do
        StartServer "GateServer" "GateServer${i}" "${i}"
    done
}

#check one service alive, if alive return 1, else return 0 
function CheckServerAlive()
{
    SERVER_CMD=$1
    SERVER_DIR=$2
 
    PID_FILE=${SERVER_DIR}/${SERVER_CMD}.pid
    
    isrun=0
    if [ -f ${PID_FILE} ] ; then
        sh_pid=`cat ${PID_FILE}`
        isrun=`ps -p${sh_pid}|wc -l`
    fi
    
    if [ ${isrun} -lt 2 ]; then
	return 0
    fi
    return 1
}

#check all service alive, if all alive return 1, else return 0 
function CheckAll()
{
    if CheckServerAlive "AdminServer" "AdminServer"; then
	return 0 
    fi
    
    if CheckServerAlive "RecordServer" "RecordServer"; then
        return 0 
    fi

    if CheckServerAlive "WorldServer" "WorldServer"; then
	return 0 
    fi

    for((i=1;i<=${SCENESERVER_NUM};i++))
    do
        if CheckServerAlive "SceneServer" "SceneServer${i}"; then
            return 0 
	fi  
    done

    for((i=1;i<=${GATESERVER_NUM};i++))
    do
        if CheckServerAlive "GateServer" "GateServer${i}"; then
            return 0 
        fi
    done

    return 1 
}



function KeepAlive()
{ 
    ISRUN=0
    if [ -f ${STARTPID_FILE} ] ; then
        SH_PID=`cat ${STARTPID_FILE}`
        ISRUN=`ps -p${SH_PID}|wc -l`
    fi
    if [ ${ISRUN} -gt 1 ]; then
        echo "start.sh is already running..."
        return;
    fi
  
    echo $$ > ${STARTPID_FILE} 
    
    SLEEP_TIMES=0 
    while true; do
	sleep 30
       
	if CheckAll; then
            SLEEP_TIMES=`expr ${SLEEP_TIMES} + 1`
            if [ ${SLEEP_TIMES} -gt 2 ]; then
                 StartAll
            fi    
        else
	    SLEEP_TIMES=0
	fi 
    done
}

StartAll
KeepAlive

