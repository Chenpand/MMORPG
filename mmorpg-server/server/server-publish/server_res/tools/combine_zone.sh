#!/bin/sh

source ../db_conf.sh
source ../config.sh

MYSQLDUMP=/opt/app/mysql/bin/mysqldump


#来源区的相关配置
SRC_H=192.168.88.19
SRC_U=root
SRC_P=admin
SRC_PT=3306
SRC_DBD=tutengrole
SRC_ZONEID=9


#直接合并数据的表列表
COMBINE_TABLES="
t_activitygiftbag
t_auction
t_counter
t_item
t_mail
t_mailitem
t_offline_event
t_pet
t_pasture
t_player_info
t_player_info_name
t_punishment
t_relation
t_transfigure
t_tribe
t_tribe_item
t_tribe_log
t_tribe_member
t_tribe_name
t_tribewar_bid
t_sortlist
"

#检查主键是否冲突
function CheckDBKey()
{
	for table in ${COMBINE_TABLES}
	do
		echo "../Bin/checkdbkey --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --table=${table}"	
		../Bin/checkdbkey --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --table=${table}
		if [ ! $? -eq 0 ]; then
			exit 1
		fi	
	done
}

#检查表结构是否一致
function CheckDBStruct()
{
	for table in ${COMBINE_TABLES}
	do
		echo "../Bin/checkdbstruct --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --table=${table}"	
		../Bin/checkdbstruct --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --table=${table}
		if [ ! $? -eq 0 ]; then
			exit 1
		fi	
	done
}


function Combine()
{
	#检查并修复名字冲突
	echo "../Bin/checkdbname --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --srczone=${SRC_ZONEID} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --destzone=${ZONE_ID} --cfg=../Config/PlayerDB.xml"
	../Bin/checkdbname --srchost=${SRC_H} --srcport=${SRC_PT} --srcuser=${SRC_U} --srcpasswd=${SRC_P} --srcdb=${SRC_DBD} --srczone=${SRC_ZONEID} --desthost=${H} --destport=${PT} --destuser=${U} --destpasswd=${P} --destdb=${DBD} --destzone=${ZONE_ID} --cfg=../Config/PlayerDB.xml
	if [ ! $? -eq 0 ]; then
		exit 1
	fi	


	#合并数据
	for table in ${COMBINE_TABLES}
	do
		echo "${MYSQLDUMP} -h${SRC_H} -u${SRC_U} -p${SRC_P} -P${SRC_PT} --default-character-set=latin1 --complete-insert --no-create-info --compact --skip-comments --triggers=false ${SRC_DBD} ${table} | ${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD}"
		${MYSQLDUMP} -h${SRC_H} -u${SRC_U} -p${SRC_P} -P${SRC_PT} --default-character-set=latin1 --complete-insert --no-create-info --compact --skip-comments --triggers=false ${SRC_DBD} ${table} | ${MYSQL} --default-character-set=latin1 -h${H} -u${U} -p${P} -P${PT} ${DBD}
	done


	#某些全局表的处理
	echo "${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} < ../Sql/combinezone_init.sql"
	${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} < ../Sql/combinezone_init.sql
}




if [ "$1" = "checkdbkey" ]; then
	CheckDBKey
	echo "check db key succeed!"
elif [ "$1" = "checkdbstruct" ]; then
	CheckDBStruct
	echo "check db struct succeed!"
elif [ "$1" = "combine" ]; then
	CheckDBKey
	CheckDBStruct
	Combine	
	echo "combine zones succeed!"
else
	echo "unknow command $1"
fi

