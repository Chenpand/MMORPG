#!/bin/sh

source ../db_conf.sh
source ../config.sh

../Bin/pointuserecover --host=${H} --port=${PT} --user=${U} --passwd=${P} --db=${DBD} --zoneid=${ZONE_ID}


