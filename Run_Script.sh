#!/bin/bash

COMMAND="NIPSCO-Project"
PROGRAM_LOC="/Users/doncole/git/NIPSCO-Project"
WEATHER_ARRAY=("Visibility (km)" "Wind Gust (m/s)" "Wind Speed (m/s)")
CAUSE_ARRAY=("210 - TREE FELL - NATURALLY" "211 - TREE FELL - STORM" "220 - TREE GROW - NATURALLY" "291 - WEATHER" "262 - CAR HITS POLE")
LOA_ARRAY=(110 150 50)
YEAR_ARRAY=(2012 2013 2014)

for i in "${YEAR_ARRAY[@]}"     #For Each Year 
do
    for j in "${LOA_ARRAY[@]}"    #For Each LOA
    do
        for k in "${CAUSE_ARRAY[@]}"   #for each Cause
        do
            for l in "${WEATHER_ARRAY[@]}" #For Each Weather Factor
            do
                echo "year=$i LOA=$j Cause=$k Factor=$l"
                $PROGRAM_LOC/$COMMAND "$i" "$j" "$k" "$l"
                sleep 10
            done
        done
    done
done
