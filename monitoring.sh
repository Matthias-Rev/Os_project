#!/bin/bash
var1=$1 var2=$2 var3=$3



if [ "$var1" = "run" ]; then
	if [ -x tinydb ]; then
		./tinydb
	else
		echo "la base de données n'existe pas"
	fi
fi

if [ "$var1" = "status" ]; then
	echo "sTaTuS"
fi

if [ "$var1" = "sync" ]; then
	echo "SYNC"
fi
if [ "$var1" = "shutdown" ]; then

	echo "SHUTDOWN"
fi
