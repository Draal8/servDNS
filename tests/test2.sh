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
TMP_ER="tmpErr"
TMP_SER="tmpServErr"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

ERROR_STR="test retour inexistant"
$SERV 1> $TMP_SSS 2> $TMP_SER &
sleep 0.1
$PROG "moodle3.unistra.fr" 1> $TMP_SS 2> $TMP_ER
sleep 0.1
kill -INT $(pidof $SERV)

echo hello >> $TMP_SS
WC1=$(wc -l $TMP_SS)
WC1=$(echo $WC1|cut -d' ' -f1)
echo hello >> $TMP_SSS
WC2=$(wc -l $TMP_SSS)
WC2=$(echo $WC2|cut -d' ' -f1)
if [ $WC1 -eq 0 ] || [ $WC2 -eq 0 ]
then
	echo 1
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP_ER) ]
	then
		echo 2
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

clean_tmp
return $TEST_CRASH

