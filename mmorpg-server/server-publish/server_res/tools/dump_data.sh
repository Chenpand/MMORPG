#!/bin/sh

source ../db_conf.sh
source ../config.sh

CREATE_PLAYER_STATEMENT="CREATE TABLE IF NOT EXISTS t_player_tmp(
			guid     BIGINT UNSIGNED NOT NULL,
			accid    INT UNSIGNED NOT NULL,
			name	 VARCHAR(32) NOT NULL,
			level	 SMALLINT UNSIGNED NOT NULL DEFAULT 1,
			occu     TINYINT  UNSIGNED NOT NULL DEFAULT 0,
			sex      TINYINT  UNSIGNED NOT NULL DEFAULT 0,
			hastribe TINYINT UNSIGNED NOT NULL DEFAULT 0,
			PRIMARY KEY(guid)
		)"	

DELETE_PLAYER_STATEMENT="DELETE FROM t_player_tmp"

SELECT_PLAYER_STATEMENT1="INSERT INTO t_player_tmp(guid,accid,name,level,occu,sex,hastribe) SELECT guid,accid,name,level,occu,sex,1 FROM t_player_info
			 WHERE guid IN (SELECT guid FROM t_tribe_member WHERE tribe <> 0)"

SELECT_PLAYER_STATEMENT2="INSERT INTO t_player_tmp(guid,accid,name,level,occu,sex,hastribe) SELECT guid,accid,name,level,occu,sex,0 FROM t_player_info
			 WHERE guid NOT IN (SELECT guid FROM t_player_tmp)"	

${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${CREATE_PLAYER_STATEMENT}"
${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${DELETE_PLAYER_STATEMENT}"
${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${SELECT_PLAYER_STATEMENT1}"
${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${SELECT_PLAYER_STATEMENT2}"

../Bin/tabledump --host=${H} --port=${PT} --user=${U} --passwd=${P} --db=${DBD} --table=t_country --outfile=player.txt


CREATE_TRIBE_STATEMENT="CREATE TABLE IF NOT EXISTS t_tribe_tmp(
			guid	BIGINT UNSIGNED NOT NULL,
			name	VARCHAR(32) NOT NULL,
			level	TINYINT UNSIGNED NOT NULL DEFAULT 1,
			setuptime  INT UNSIGNED NOT NULL,
			totem		TINYINT UNSIGNED NOT NULL DEFAULT 1,
			folkhouse	TINYINT UNSIGNED NOT NULL DEFAULT 1,
			college		TINYINT UNSIGNED NOT NULL DEFAULT 1,
			workshop	TINYINT UNSIGNED NOT NULL DEFAULT 1,
			membernum	INT UNSIGNED NOT NULL DEFAULT 0,
			PRIMARY KEY(guid)
		)"


DELETE_TRIBE_STATEMENT="DELETE FROM t_tribe_tmp"

SELECT_TRIBE_STATEMENT="INSERT INTO t_tribe_tmp(guid,name,level,setuptime,totem,folkhouse,college,workshop,membernum) 
				SELECT t1.guid,t1.name,t1.level,t1.setuptime,t1.totem,t1.folkhouse,t1.college,t1.workshop,count(t2.guid)
				FROM t_tribe AS t1, t_tribe_member AS t2 WHERE t1.guid=t2.tribe GROUP by t2.tribe"


${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${CREATE_TRIBE_STATEMENT}"
${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${DELETE_TRIBE_STATEMENT}"
${MYSQL} -h${H} -u${U} -p${P} -P${PT}  ${DBD} -e "${SELECT_TRIBE_STATEMENT}"


../Bin/tabledump --host=${H} --port=${PT} --user=${U} --passwd=${P} --db=${DBD} --table=t_tribe_tmp --outfile=tribe.txt

zip data_${ZONE_ID}.zip player.txt tribe.txt
rm tribe.txt


