#!/bin/sh

if [ $# -ne 3 ]
then
    echo "usage: $0 <testdir> <program-to-test> <server-to-test>" >&2
    exit 1
fi

TESTDIR="$1"
PROG="$2"
SERV="$3"
TMP="tmp"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

ERROR_STR="Client | Syntaxe invalide : pas d'argument"
$PROG 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

ERROR_STR="Client | Syntaxe invalide : trop d'arguments"
$PROG "moodle3.unistra.fr" b  2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

#ERROR_STR="Serveur | Syntaxe invalide : trop d'arguments"
#$PROG "moodle3.unistra.fr" 2> $TMP
#if [ ! $? ]
#then
#	echo $ERROR_STR
#	TEST_CRASH=$((TEST_CRASH + 1))
#else
#	test_rerror $TMP
#	if [ $? ]
#	then
#		echo $ERROR_STR
#		TEST_CRASH=$((TEST_CRASH + 1))
#	fi
#fi


ERROR_STR="Client | Motif d'entree trop court"
$PROG "m" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

ERROR_STR="Client | Motif d'entree trop long"
$PROG "moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3moodle3.unistra.fr" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

ERROR_STR="Client | Syntaxe invalide 1"
$PROG "moodle3.unistra.fr.com" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

ERROR_STR="Client | Syntaxe invalide 2"
$PROG "moodle3.unistra" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $? ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
	fi
fi

clean_tmp()
return $TEST_CRASH

