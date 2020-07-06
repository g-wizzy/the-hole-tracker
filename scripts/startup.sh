#!/bin/bash

export LD_LIBRARY_PATH="/usr/local/lib/nuitrack" ;

MODEL=$(`dirname $0`/../realsense-model)
echo "Model : $MODEL"

if [ "$MODEL" = "D435" ]; then
	COMMAND=`dirname $0`/../the-hole-blob-tracker
elif [ "$MODEL" = "D415" ]; then
	COMMAND=`dirname $0`/../the-hole-skeleton-tracker
else
	echo "No RealSense camera detected. Exiting ..."
	exit 1
fi

while :
do
	echo $COMMAND
done
