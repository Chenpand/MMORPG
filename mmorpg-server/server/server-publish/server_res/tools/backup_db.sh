#!/bin/sh

source ../db_conf.sh
source ../config.sh


echo "../Bin/dbcopy --host=${H} --port=${PT} --user=${U} --passwd=${P} --srcdb=${DBD} --destdb=${DBD}_backup"
../Bin/dbcopy --host=${H} --port=${PT} --user=${U} --passwd=${P} --srcdb=${DBD} --destdb=${DBD}_backup
if [ ! $? -eq 0 ];then
	exit 1
fi

echo "backup database succeed!"


