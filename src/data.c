/*
 * data.c
 *
 *  Created on: Mar 10, 2016
 *      Author: doncole
 */

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define GoshenLOA 110
#define GaryLOA  150
#define ValpoLOA  50

const char* GOSHEN_WEATHER_FILE = "./Data/30407_Goshen__Hourly_2012.csv";
const char* GARY_WEATHER_FILE = "./Data/30437_Gary__Hourly_2012.csv";
const char* VALPO_WEATHER_FILE = "./Data/30407_Goshen__Hourly_2012.csv";

FILE * findWeatherFile (int LOA){
	FILE *weatherFile;

	if (LOA == GoshenLOA){
		weatherFile = fopen(GOSHEN_WEATHER_FILE, "rb");
		if (weatherFile == NULL) {
				fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GOSHEN_WEATHER_FILE,LOA,strerror(errno));
				exit(EXIT_FAILURE);
			}
		return weatherFile;
	}
	else if (LOA == GaryLOA){
		weatherFile = fopen(GARY_WEATHER_FILE, "rb");
		if (weatherFile == NULL) {
				fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GARY_WEATHER_FILE,LOA,strerror(errno));
				exit(EXIT_FAILURE);
			}
		return weatherFile;
	}
	else if (LOA == ValpoLOA){
		weatherFile = fopen(VALPO_WEATHER_FILE, "rb");
		if (weatherFile == NULL) {
				fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", VALPO_WEATHER_FILE,LOA,strerror(errno));
				exit(EXIT_FAILURE);
			}
		return weatherFile;
	}
	else{
		fprintf(stderr, "Failed to find the file for your LOA: %d\n", LOA);
		exit(EXIT_FAILURE);
	}

}
