#!/bin/sh

if [ $# -ne 3 ]
then
    echo "usage: $0 <testdir> <program-to-test> <server-to-test>" >&2
    exit 1
fi

TESTDIR="$1"
PROG="$2"
SERV="$3"
TMP_SS="tmp_sortie"
TMP_SSS="tmpServ_sortie"
TMP_ER="tmp"
TMP_SER="tmpServ"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

ERROR_STR="test retour correspondant"
$SERV 1> $TMP_SSS 2> $TMP_SER &
sleep 0.1
$PROG "test.eu" 1> /dev/null 2> $TMP_ER
$PROG "test.com" 1> /dev/null 2> $TMP_ER
$PROG "test.fr" 1> /dev/null 2> $TMP_ER
$PROG "test.net" 1> /dev/null 2> $TMP_ER
$PROG "test.org" 1> /dev/null 2> $TMP_ER
sleep 0.1
kill -INT $!
if [ ! cmp tests/dataCheck/checkt3 $TMP_SSS ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	
	if [ $(test_rerror $TMP_ER) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

clean_tmp()
return $TEST_CRASH

