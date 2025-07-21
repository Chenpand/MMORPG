#!/bin/sh

source ../db_conf.sh
source ../config.sh

COUNTRY1=3
COUNTRY2=3
COUNTRY3=2

COUNTRY_TABLES="
t_tribe
t_player_info
t_relation
t_sortlist
"

for table in ${COUNTRY_TABLES}
do
	${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} -e"UPDATE $table SET country="`expr $COUNTRY1 + 10`" WHERE country=1";
	${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} -e"UPDATE $table SET country="`expr $COUNTRY2 + 10`" WHERE country=2";
	${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} -e"UPDATE $table SET country="`expr $COUNTRY3 + 10`" WHERE country=3";

	${MYSQL} -h${H} -u${U} -p${P} -P${PT} ${DBD} -e"UPDATE $table SET country = country - 10 WHERE country > 10";
done

echo "change country succeed!"

