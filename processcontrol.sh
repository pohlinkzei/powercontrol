#!/bin/bash

while true;
do

if pgrep powercontrol >/dev/null
	then
		sleep 1
	else
		sudo ~/powercontrol/powercontrol
		sleep 5
fi

done
