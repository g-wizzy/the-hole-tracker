#!/bin/bash

DIR=`dirname $0`

# Save PID for interruptions due to calibration
echo $$ > $DIR/pid

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
until $DIR/../$COMMAND; do
	echo "Tracker crashed. Relaunching in 1 second"
	sleep 1
done
