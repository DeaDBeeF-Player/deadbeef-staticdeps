#!/bin/sh
# tests/settings.sh.  Generated from settings.sh.in by configure.
#
# $Id: settings.sh.in,v 1.11 2006/10/15 10:07:50 airborne Exp $

# location of the example program used in the tests
CDDB_QUERY='/home/waker/prj/deadbeef-staticdeps/libcddb-1.3.2/examples/cddb_query -q'

# location of local test cache
CDDB_CACHE='/home/waker/prj/deadbeef-staticdeps/libcddb-1.3.2/tests/testcache'

# location of test data
CDDB_DATA='/home/waker/prj/deadbeef-staticdeps/libcddb-1.3.2/tests/testdata'

# library built with iconv support?
WITH_ICONV='1'

# name of temporary file for result data
TMP_FILE='./result.tmp'

# test result return codes
SUCCESS=0
FAILURE=1
SKIPPED=77

# libcddb/cddb_query error codes
DISC_NOT_FOUND=12

# counters for number of succeeded/failed/skipped/total tests
CNT_SUCCESS=0
CNT_FAIL=0
CNT_SKIP=0

start_test()
{
    MSG=$*
    printf '%s...' "${MSG}"
    RESULT=
}

fail()
{
    REASON=$*
    if test -z "$REASON"; then
        printf 'failed.\n'
    else
        printf 'failed (%s).\n' "${REASON}"
    fi
    CNT_FAIL=`expr $CNT_FAIL + 1`
    RESULT=${FAILURE}
}

success()
{
    printf 'ok.\n'
    CNT_SUCCESS=`expr $CNT_SUCCESS + 1`
    RESULT=${SUCCESS}
}

skip()
{
    REASON=$*
    if test -z "$REASON"; then
        printf 'skipped.\n'
    else
        printf 'skipped (%s).\n' "${REASON}"
    fi
    CNT_SKIP=`expr $CNT_SKIP + 1`
    RESULT=${SKIPPED}
}

finalize()
{
    rm $TMP_FILE > /dev/null 2>&1
    CNT_TOTAL=`expr $CNT_SUCCESS + $CNT_FAIL + $CNT_SKIP`
    echo "Results for $0"
    if [ $CNT_SUCCESS -gt 0 ]; then
        echo "  Succeeded: ${CNT_SUCCESS}/${CNT_TOTAL}"
    fi
    if [ $CNT_SKIP -gt 0 ]; then
        echo "  Skipped:   ${CNT_SKIP}/${CNT_TOTAL}"
    fi
    if [ $CNT_FAIL -gt 0 ]; then
        echo "  Failed:    ${CNT_FAIL}/${CNT_TOTAL}"
    fi
    if [ $CNT_FAIL -gt 0 ]; then
        exit $FAILURE
    elif [ $CNT_SUCCESS -gt 0 -o $CNT_SKIP -eq 0 ]; then
        exit $SUCCESS
    else
        exit $SKIPPED
    fi
}

cddb_query()
{
    rm $TMP_FILE > /dev/null 2>&1
    $CDDB_QUERY "$@" > $TMP_FILE
}

check_discid()
{
    DISCID_STR='CD disc ID is '
    RV=$1
    EXP_ID=$2
    if [ ${RV} -ne 0 ]; then
        fail 'cddb_query failed'
        return
    fi
    RET_ID=`cat $TMP_FILE | sed "/^${DISCID_STR}[0-9a-f]*\$/!d;s/${DISCID_STR}\([0-9a-f]*\)/\1/"`
    if [ ${RET_ID}x != ${EXP_ID}x ]; then
        fail 'result mismatch'
        return
    fi
    success
}

diff_res_exp()
{
    EXPECTED=$1
    diff $TMP_FILE $EXPECTED > /dev/null
    if [ $? -ne 0 ]; then
        fail 'result mismatch'
        return
    fi
    success
}

check_read()
{
    RV=$1
    DISC_ID=$2
    if [ ${RV} -ne 0 ]; then
        fail 'cddb_query failed'
        return
    fi
    EXP_READ="${CDDB_DATA}/${DISC_ID}.txt"
    diff_res_exp $EXP_READ
}

check_query()
{
    RV=$1
    if [ ${RV} -ne 0 ]; then
        fail 'cddb_query failed'
        return
    fi
    MATCH_COUNT='^Number of matches: \([0-9]*\)$'
    CNT=`grep "${MATCH_COUNT}" ${TMP_FILE} | sed "s/${MATCH_COUNT}/\1/"`
    if test -z "$CNT"; then
        fail 'number of matches not found'
        return
    fi
    if test $CNT -eq 0; then
        fail 'number of matches not found'
        return
    fi
    # check whether all $CNT entries are present
    # XXX: check for category, disc ID and title ?!?
    I=1
    while test $I -le $CNT; do
        grep "Match $I" $TMP_FILE > /dev/null 2>&1
        if test $? -ne 0; then
            fail "entry $I missing, $CNT expected"
            return
        fi
        I=`expr $I + 1`
    done
    # make sure there are not more matches than $CNT
    grep "Match $I" $TMP_FILE > /dev/null 2>&1
    if test $? -eq 0; then
        fail "entry $I present, $CNT expected"
        return
    fi
    success
}

check_not_found()
{
    RV=$1
    DISC_ID=$2
    if [ $RV -ne $DISC_NOT_FOUND ]; then
        fail
        return
    fi
    success
}
