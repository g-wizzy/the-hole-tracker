#!/bin/bash

DIR=`dirname $0`

MODEL=$($DIR/../realsense-model)
echo "Model : $MODEL"

if [ "$MODEL" = "D435" ]; then
	COMMAND=the-hole-blob-tracker
elif [ "$MODEL" = "D415" ]; then
	COMMAND=the-hole-skeleton-tracker
else
	echo "No RealSense camera detected. Exiting ..."
	exit 1
fi

export LD_LIBRARY_PATH="/usr/local/lib/nuitrack"
while :
do
	$DIR/../$COMMAND
	if [ $? -eq 0 ]
	then
		echo "Tracker closed. Assuming from the calibrator. Relaunching in 20 seconds"
		sleep 20
	else
		echo "Tracker crashed. Attempting to relaunch in 1 second"
		sleep 1
	fi
done
