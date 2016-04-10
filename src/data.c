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

const char* GOSHEN_WEATHER_FILE_2012 = "./Data/Goshen_Weather_2012.csv";
const char* GOSHEN_WEATHER_FILE_2013 = "./Data/Goshen_Weather_2013.csv";
const char* GOSHEN_WEATHER_FILE_2014 = "./Data/Goshen_Weather_2014.csv";

const char* GARY_WEATHER_FILE_2012 = "./Data/Gary_Weather_2012.csv";
const char* GARY_WEATHER_FILE_2013 = "./Data/Gary_Weather_2013.csv";
const char* GARY_WEATHER_FILE_2014 = "./Data/Gary_Weather_2014.csv";

const char* VALPO_WEATHER_FILE_2012 = "./Data/Valpo_Weather_2012.csv";
const char* VALPO_WEATHER_FILE_2013 = "./Data/Valpo_Weather_2013.csv";
const char* VALPO_WEATHER_FILE_2014 = "./Data/Valpo_Weather_2013.csv";

const char* OUTTAGE_FILE_2012 = "./Data/2012 Outage Data FORMATED.csv";
const char* OUTTAGE_FILE_2013 = "./Data/2013 Outage Data.csv";
const char* OUTTAGE_FILE_2014 = "./Data/2014 Outage Data.csv";

FILE * findWeatherFile (int year, int LOA){
	FILE *weatherFile;

	if (LOA == GoshenLOA){
		if (year == 2012){
			weatherFile = fopen(GOSHEN_WEATHER_FILE_2012, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GOSHEN_WEATHER_FILE_2012,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if (year == 2013){
			weatherFile = fopen(GOSHEN_WEATHER_FILE_2013, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GOSHEN_WEATHER_FILE_2013,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if (year == 2014){
			weatherFile = fopen(GOSHEN_WEATHER_FILE_2014, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GOSHEN_WEATHER_FILE_2014,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
	}
	else if (LOA == GaryLOA){
		if (year == 2012){
			weatherFile = fopen(GARY_WEATHER_FILE_2012, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GARY_WEATHER_FILE_2012,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if (year == 2013){
			weatherFile = fopen(GARY_WEATHER_FILE_2013, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GARY_WEATHER_FILE_2013,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if (year == 2014){
			weatherFile = fopen(GARY_WEATHER_FILE_2014, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", GARY_WEATHER_FILE_2014,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
	}
	else if (LOA == ValpoLOA){
		if( year == 2012){
			weatherFile = fopen(VALPO_WEATHER_FILE_2012, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", VALPO_WEATHER_FILE_2012,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if( year == 2013){
			weatherFile = fopen(VALPO_WEATHER_FILE_2013, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", VALPO_WEATHER_FILE_2013,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
		else if( year == 2014){
			weatherFile = fopen(VALPO_WEATHER_FILE_2014, "rb");
			if (weatherFile == NULL) {
					fprintf(stderr, "Failed to open file %s for LOA %d: %s\n", VALPO_WEATHER_FILE_2014,LOA,strerror(errno));
					exit(EXIT_FAILURE);
				}
			return weatherFile;
		}
	}
	else{
		fprintf(stderr, "Failed to find the file for your LOA: %d and Year: %d.\n", LOA, year);
		exit(EXIT_FAILURE);
	}
}

FILE* findOutageFile(int year){
	FILE* outageFile;

	if (year == 2012){
		outageFile = fopen(OUTTAGE_FILE_2012, "rb");
		if (outageFile == NULL) {
			fprintf(stderr, "Failed to open file %s for year %d.\n", OUTTAGE_FILE_2012, year);
			exit(EXIT_FAILURE);
		}
	}
	else if (year == 2013){
		outageFile = fopen(OUTTAGE_FILE_2013, "rb");
		if (outageFile == NULL) {
			fprintf(stderr, "Failed to open file %s for year %d.\n", OUTTAGE_FILE_2013, year);
			exit(EXIT_FAILURE);
		}
	}
	else if (year == 2013){
		outageFile = fopen(OUTTAGE_FILE_2014, "rb");
		if (outageFile == NULL) {
			fprintf(stderr, "Failed to open file %s for year %d.\n", OUTTAGE_FILE_2014, year);
			exit(EXIT_FAILURE);
		}
	}
	else{
		fprintf(stderr, "Failed to open an Outage File for year %d.\n", year);
		exit(EXIT_FAILURE);
	}
	return outageFile;
}
