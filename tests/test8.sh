#!/bin/sh

if [ $# -ne 3 ]
then
    echo "usage: $0 <testdir> <program-to-test> <server-to-test>" >&2
    exit 1
fi

#Test multiples serveur (dont des serveurs racines) qui se partagent les adresses de maniere exclusive IPV4/IPV6 et elles s'entrelacent

TESTDIR="$1"
PROG="$2"
SERV="$3"
LOG="log"
DATA="$TESTDIR/datatest8"
TMPC_S="tmp_sortie"
TMPS_S="tmpServ_sortie"
TMPC_ER="tmpErr"
TMPS_ER="tmpServErr"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

declare -a PID=()
$SERV 3500 "$DATA/bddserv11" 1> $TMPS_S 2> $TMPS_ER &
PID+=($!)
$SERV 3501 "$DATA/bddserv21" 1>> $TMPS_S 2>> $TMPS_ER &
PID+=($!)
$SERV 3502 "$DATA/bddserv31" 1>> $TMPS_S 2>> $TMPS_ER &
PID+=($!)
$SERV 3504 "$DATA/bddserv22" 1>> $TMPS_S 2>> $TMPS_ER &
PID+=($!)
$SERV 3505 "$DATA/bddserv32" 1>> $TMPS_S 2>> $TMPS_ER &
PID+=($!)
$SERV 3507 "$DATA/bddserv12" 1> $TMPS_S 2> $TMPS_ER &
PID+=($!)
sleep 0.1	#on laisse un peu de temps au serveur pour setup
$PROG "$DATA/bddclient" "$DATA/bddsites" 1> $TMPC_S 2> $TMPC_ER
sleep 0.4	#on laisse un peu de temps au serveur pour resoudre

for i in ${PID[@]}
do
	kill $i
done

nbResolutions=$( grep -c "Adresse resolue" $TMPC_S )
if [ $nbResolutions -eq 4 ]
then
	clean_tmp
	exit 0
fi

cat $TMPC_S >> $LOG
echo " " >> $LOG
cat $TMPC_ER >> $LOG
echo " " >> $LOG
cat $TMPS_S >> $LOG
echo " " >> $LOG
cat $TMPS_ER >> $LOG
clean_tmp
exit $((4-$nbResolutions))

