#!/bin/bash

function kill_tracker {
	PID=`pgrep the-hole`

	if [ ! -z $PID ]
	then
		# Kill tracker (15 = SIGTERM)
		sudo kill -15 $PID
	fi
}

function run_tracker {
    DIR=`dirname $0`

    MODEL=$($DIR/../realsense-model)
    echo "Model : $MODEL"

    if [ "$MODEL" = "D435" ]; then
        $DIR/../the-hole-blob-tracker
    elif [ "$MODEL" = "D415" ]; then
        # Since Nuitrack was installed by encor, this script should be run through
        # sudo -u encor -- ./test_tracker.sh
        export LD_LIBRARY_PATH="/usr/local/lib/nuitrack"
        $DIR/../the-hole-skeleton-tracker
    else
        echo "No RealSense camera detected. Exiting ..."
        exit 1
    fi
}

kill_tracker
run_tracker

# Sometimes, the tracker starts again, but doesn't work (although the process itself doesn't)
kill_tracker
