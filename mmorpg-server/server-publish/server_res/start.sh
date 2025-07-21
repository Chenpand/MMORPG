#!/bin/sh
source config.sh

STIME=`date +%F-%H-%M-%S`
DB_DIR=Sql/
if [ -f ${DB_DIR}/update_db.key ]; then
	cd ${DB_DIR}
	source update_db.sh
	rm -f update_db.key
	mv update_db.sh update_db.sh.${STIME}
	cd ..	
fi

START_HELP=tools/start_help.sh

chmod +x ${START_HELP}
./${START_HELP} ${ZONE_ID} ${GATESERVER_NUM} ${SCENESERVER_NUM} ${PORT}&

