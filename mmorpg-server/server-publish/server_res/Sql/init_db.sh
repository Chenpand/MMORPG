#!/bin/sh
source ../db_conf.sh

function init_db()
{
	echo "init database begin..."
	
	#${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/init_db.sql
	
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_announcement.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_auction.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_counter.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_gameparam.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_guid.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_item.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_lottery_res.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_mail.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_mailitem.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_offline_event.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_pet.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_player_info.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_punishment.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_relation.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_sortlist.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_tribe.sql
    ${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_activitygiftbag.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_babel.sql 
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_bidauction.sql	
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_halloffame.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_marriage.sql
	${MYSQL} -h$H -u$U -p$P -P$PT $DBD < create/t_fish.sql	
	echo "init database end. "
}

init_db
