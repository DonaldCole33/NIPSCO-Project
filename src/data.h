/*
 * data.h
 *
 *  Created on: Mar 10, 2016
 *      Author: doncole
 */

#ifndef DATA_H_
#define DATA_H_

#define maxCauseIndex 366
#define maxLOAIndex 161
#define maxNumofLOA 9
#define loaColumnNum 3
#define causeColumnNum 12
#define DAYS 366
#define HOURS 24 	//hours in a day
#define MASTER 0	//The master processor number
#define MONTHS 12	//number of months



struct Correlation{				//Structure for Computing the Pearson Correlation algorithm
	int *outtageDataX;				//add the data to each element in the array
	double *weatherDataY;			//add a one for each hour not found
	struct tm dateArray[3000];			//the dates that are valid we need to find in weather data
	int startMonth;						//The start month
	int durationofMonths;				//amount of months we are looking at
	int year;
	int LOA;							//integer of the LOA
	const char* weatherFactorName;		//name of the weather factor we are correlating
	const char* outageCauseName;		//Name of the Outage we are correlating

};

char *causeArray[] = {
"10 - AIRPLANE",
"101 - FIRE POLE",
"110 - FOREIGN OBJECT",
"140 - GALLOPING CONDUCTOR",
"170 - PERSONNEL",
"190 - BRANCH COMPANY",
"20 - ANIMAL",
"210 - TREE FELL - NATURALLY",
"211 - TREE FELL - STORM",
"220 - TREE GROW - NATURALLY",
"230 - UNKNOWN",
"250 - VANDALISM",
"262 - CAR HITS POLE",
"291 - WEATHER",
"30 - ANTENNA",
"310 - LIGHTNING",
"320 - SNOW",
"340 - TORNADO",
"360 - UG PRIMARY DIG IN",
"361 - UG PRIMARY FAULT",
"362 - UG PRIMARY TERMINATION",
"363 - UG SECONDARY DIG IN",
"364 - UG SECONDARY FAULTED",
"365 - UG SECONDARY TERMINATION",
"50 - CONTAMINATION",
"60 - CRANE IN CONDUCTOR",
"75 - CUSTOMER OWNED CONDUCTOR",
"76 - CUSTOMER OWNED SECONDARY SHORT",
"80 - CONDUCTORS",
"81 - STATIC DOWN",
"82 - LINE ARRESTER",
"83 - JUMPER OPEN",
"84 - LINE INSULATOR",
"85 - LINE SWITCH",
"86 - LINE TRANSFORMER",
"87 - POLE DAMAGE",
"88 - LINE FUSE",
"89 - LINE REGULATOR / CAPACITOR",
"90 - SUBSTATION TRANSFORMER",
"900 - LOAD MANAGEMENT",
"91 - SUBSTATION BREAKER",
"92 - SUBSTATION REGULATOR",
"93 - SUBSTATION ARRESTER",
"94 - SUBSTATION FUSE",
"95 - SUBSTATION SWITCH",
"96 - SUBSTATION INSULATOR",
"97 - SUBSTATION BUS",
"98 - SUBSTATION RELAY"};

FILE* findWeatherFile (int year, int LOA);					//Will find the Correct WeatherFile to open
FILE* findOutageFile (int year);

void ComputeCorrelation(struct Correlation* cData);


#endif /* DATA_H_ */
