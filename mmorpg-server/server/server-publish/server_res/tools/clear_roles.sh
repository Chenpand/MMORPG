#!/bin/sh

source ../db_conf.sh
source ../config.sh

#清理小号的最高等级
ROLE_LEVEL=30
#清理小号的最少离线天数
ROLE_OFFLINEDAY=30


echo "../Bin/rolecleaner --cfg=../Config/PlayerDB.xml --host=${H} --port=${PT} --user=${U} --passwd=${P} --database=${DBD} --level=${ROLE_LEVEL} --offlineday=${ROLE_OFFLINEDAY}"
../Bin/rolecleaner --cfg=../Config/PlayerDB.xml --host=${H} --port=${PT} --user=${U} --passwd=${P} --database=${DBD} --level=${ROLE_LEVEL} --offlineday=${ROLE_OFFLINEDAY}
if [ ! $? -eq 0 ];then
	exit 1
fi

echo "clear roles succeed!"

