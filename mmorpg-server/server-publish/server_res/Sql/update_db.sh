#!/bin/sh
source ../db_conf.sh

function update_db()
{
	${MYSQL}	-h${H}	-u${U}	-p${P}	-P${PT}	${DBD} < updates/update_20160406.sql
}
update_db
