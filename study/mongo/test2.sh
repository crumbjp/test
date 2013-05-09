#!/usr/bin/env bash
export LC_ALL=C
export LANG=C
export LANGUAGE=C

if [ "${MONGO_HOME}" = "" ] ; then
    MONGO_HOME=/usr/local/mongo
fi
#ulimit -s 10240
function mongo_test {
    TARGET=$1
    SIZE=$2
    NUM=$3
    echo == ${TARGET} == 
    date
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.dropDatabase()'
    nice ./test_client 127.0.0.1:27017 ${TARGET}.TEST `cat ${MONGO_HOME}/logs/mongod.pid` 10 ${SIZE} ${NUM}
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.stats()'
    tail -n 2000 ${MONGO_HOME}/logs/mongod.log | grep -e 'build index ' -e 'system.indexes ' -e '] update ' -e '] query ' -e '] getmore '

    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.dropDatabase()'
}
mongo_test 'small'   400                         100000
exit
pidstat -p `cat ${MONGO_HOME}/logs/mongod.pid` 60 > mongo.pidstat &
PID_STAT=$!
mongo_test 'small'   400                         100000000
mongo_test 'mild'    1024                         40000000
mongo_test 'normal'  `expr    4 '*' 1024`         10000000
mongo_test 'extent'  `expr   10 '*' 1024`          4000000
mongo_test 'large'   `expr   40 '*' 1024`          1000000
mongo_test 'more'    `expr  100 '*' 1024`           400000
mongo_test 'pretty'  `expr  400 '*' 1024`           100000
mongo_test 'extra'   `expr 1000 '*' 1024`            40000
mongo_test 'massive' `expr 4000 '*' 1024`            10000
kill -p ${PID_STAT}