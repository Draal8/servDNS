#!/bin/sh

if [ $# -ne 3 ]
then
    echo "usage: $0 <testdir> <program-to-test> <server-to-test>" >&2
    exit 1
fi

TESTDIR="$1"
PROG="$2"
SERV="$3"
LOG="log"
DATA="$TESTDIR/datatest4"
TMPC_S="tmp_sortie"
TMPS_S="tmpServ_sortie"
TMPC_ER="tmpErr"
TMPS_ER="tmpServErr"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

ERROR_STR="test retour inexistant"
$SERV 3500 "$DATA/bddserv1" 1 1> $TMPS_S 2> $TMPS_ER &
sleep 0.1	#on laisse un peu de temps au serveur pour setup
PID=$!
$PROG "$DATA/bddclient" "$DATA/bddsites" 1> $TMPC_S 2> $TMPC_ER
sleep 0.1	#on laisse un peu de temps au serveur pour resoudre

kill $PID

nbResolutions=$( grep -c "Adresse resolue" $TMPC_S )
if [ $nbResolutions -eq 1 ]
then
	clean_tmp
	exit 0
fi

cat $TMP >> $LOG
clean_tmp
exit $((1-$nbResolutions))

