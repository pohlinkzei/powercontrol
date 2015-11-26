#!/bin/bash

while true;
do

if pgrep "powercontrol" >/dev/null
then
	sleep 1
else
	echo "Restarting PowerControl"
	sudo ./powercontrol
	sleep 5
fi
sleep 1
done
