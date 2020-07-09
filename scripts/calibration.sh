#!/bin/bash

# Find the process of the tracker using the camera
PID=`pgrep the-hole`

if [ ! -z $PID ]
then
	# Kill tracker (15 = SIGTERM)
	sudo kill -15 $PID
fi

# Run calibrator
/home/encor/thehole/the-hole-calibrator

