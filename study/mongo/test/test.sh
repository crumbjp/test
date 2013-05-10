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
    DIV=`expr $NUM / 2500`
    # gen data file
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.dropDatabase()'
    echo ./gen_data.pl -s $SIZE -n $NUM
    ./gen_data.pl -s $SIZE -n $NUM

    echo == IMPORT == ${TARGET}
    date
    time for i in {0..9} ; do ${MONGO_HOME}/bin/mongoimport -d ${TARGET} -c testcol --file data${i}.js > /dev/null  ; done
    tail -n 5 mongo.pidstat
    rm -f data{0..9}.js

    echo == INDEX == ${TARGET}
    date
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.ensureIndex({value1:1})'
    tail -n 5 mongo.pidstat
    sleep 1
    tail -n 5 ${MONGO_HOME}/logs/mongod.log
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.ensureIndex({value2:1})'
    sleep 1
    tail -n 5 ${MONGO_HOME}/logs/mongod.log
    tail -n 5 mongo.pidstat
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.stats()'

    echo == UPDATE1 == ${TARGET}
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value0:3}},{multi:true})'
    tail -n 5 mongo.pidstat
    date
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value0:4}},{multi:true})'
    tail -n 5 mongo.pidstat
    echo == UPDATE2 == ${TARGET}
    date
    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value0:NumberLong(4)}},{multi:true})'
    tail -n 5 mongo.pidstat
#
#    echo == UPDATE3 == ${TARGET}
#    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value5:"cccccccccccccccccccccccccccccc"}},{multi:true})'
#    tail -n 5 mongo.pidstat
#    date
#    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value5:"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"}},{multi:true})'
#    tail -n 5 mongo.pidstat
#
#    echo == UPDATE4 == ${TARGET}
#    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value7:"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"}},{multi:true})'
#    tail -n 5 mongo.pidstat
#    date
#    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.update({},{$set:{value7:"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"}},{multi:true})'
#    tail -n 5 mongo.pidstat
#    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.testcol.stats()'
#    sleep 1
#    tail -n 15 ${MONGO_HOME}/logs/mongod.log

    echo == RANDOM FETCH1 == ${TARGET}
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value1:0 })
' | wc
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value1:20 })
' >/dev/null
    date
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value1:30 })
' >/dev/null
    tail -n 5 mongo.pidstat

    echo == RANDOM FETCH2 == ${TARGET}
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value2:0 })
' | wc
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value2:2 })
' > /dev/null
    date
    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
DBQuery.shellBatchSize=100000000
db.testcol.find({ value2:3 })
' >/dev/null
    tail -n 5 mongo.pidstat

    sleep 1
    tail -n 300 ${MONGO_HOME}/logs/mongod.log | grep getmore

#    echo == SEQ FETCH1 == ${TARGET}
#    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
#DBQuery.shellBatchSize=100000000
#db.testcol.find({ _id: { $lt : '$DIV'} })
#' | wc
#    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
#DBQuery.shellBatchSize=100000000
#db.testcol.find({ _id: { $lt : '$DIV'} })
#' > /dev/null
#    date
#    time ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'
#DBQuery.shellBatchSize=100000000
#db.testcol.find({ _id: { $lt : '$DIV'} })
#' | wc
#    tail -n 5 mongo.pidstat
#
#    sleep 1
#    tail -n 300 ${MONGO_HOME}/logs/mongod.log | grep getmore


    ${MONGO_HOME}/bin/mongo ${TARGET} --quiet <<<'db.dropDatabase()'
}
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