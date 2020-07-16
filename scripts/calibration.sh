#!/bin/bash

function kill_tracker {
	PID=`pgrep the-hole`

	if [ ! -z $PID ]
	then
		# Kill tracker (15 = SIGTERM)
		sudo kill -15 $PID
	fi
}

# Run calibrator
kill_tracker
/home/encor/thehole/the-hole-calibrator

# Sometimes, the tracker starts again, but doesn't work (although the process itself doesn't)
kill_tracker
