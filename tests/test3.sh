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
$PROG "moodle3.unistra.fr" 1> $TMP_SS 2> $TMP_ER
if [ ! cmp $TMP_SS $TMP_SSS ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP_ER
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

clean_tmp()
return $TEST_CRASH

