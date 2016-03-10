/*
 * Main.c
 *
 *  Created on: Jan 25, 2016
 *      Author: doncole
 *
 *      Takes 2 args, input file name and output file name
 *      ETL input_file output_file
 */


#include <stdio.h>
#include <stdlib.h>
#include <csv.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#define maxCauseIndex 366
#define maxLOAIndex 161
#define maxNumofLOA 9
#define loaColumnNum 3
#define causeColumnNum 12
#define DAYS 366
#define HOURS 24 	//hours in a day
#define MASTER 0	//The master processor number
#define MONTHS 12	//number of months

const char* OUTTAGE_FILE = "../Data/2012 Outage Data FORMATED.csv";
const char* GOSHEN_WEATHER_FILE = "../Data/30407_Goshen__Hourly_2012.csv";
const char* GARY_WEATHER_FILE = "../Data/30437_Gary__Hourly_2012.csv";
const char* VALPO_WEATHER_FILE = "../Data/30407_Goshen__Hourly_2012.csv";
//const char* LOA = "LOA";
static int CURRENT_COLUMN_NUM = 0;
static int CURRENT_OUTTAGE_ROW_NUM = 0;
static int CURRENT_WEATHER_ROW_NUM = 0;
static int currentDataColumn = -1;
//static int OsampleSize = 0;
static int sampleSize = 0;
static int maxSample;
static int foundFlag = 0;

//struct WeatherFactor{
//	char* nameOfFactor;
//	//Could use a Date object here such as
//	//struct tm* date;	//date struct
//	char* month;
//	char* day;
//	char* hour;
//};

//struct OuttageFactor{
//	char* LOA;	//LOA number we are finding
//	char* duration;		//The lenght of the outtage
//	struct tm date;		//the Date of the outtage
//	char* month;
//	char* day;
//	char* hour;
//};

/*	The correlation data is as follows
 * 	Fill out the x and y data for each day
 * 	then multiply x and y for each day
 * 	square x and y for each day
 * 	add the totals up and use this algorithm
 *
 */
struct Correlation{				//Structure for Computing the Pearson Correlation algorithm
	int outtageDataX;					//add the data to each element in the array
	double weatherDataY[3000];			//add a one for each hour not found
	struct tm dateArray[3000];			//the dates that are valid we need to find in weather data
	int startMonth;						//The start month
	int durationofMonths;				//amount of months we are looking at
	int LOA;							//integer of the LOA
	const char* weatherFactorName;		//name of the weather factor we are correlating
	const char* outageCauseName;		//Name of the Outage we are correlating
};

//void readWeatherFile(FILE* weatherFile, struct Correlation* cData);

void ComputeCorrelation(struct Correlation* cData){
	//first sum the X and Y data
	double xsum = cData->outtageDataX;
	double ysum = 0;
	double xysum = 0;
	double xsqSum = pow(xsum,2);
	double ysqSum = 0;

	for (int i = 0; i < maxSample; i++){	//Compute sums of Weather Data
		ysum += cData->weatherDataY[i];
		ysqSum += pow(cData->weatherDataY[i], 2);
	}
	xysum = ysum;	//x's are only ones, summed after multiplying each x*y
	double numerator = sampleSize * xysum - (xsum * ysum);
	double denomenator = sqrt((sampleSize * xsqSum - pow(xsum,2)) *(sampleSize * ysqSum - pow(ysum,2)));
	double cf = (numerator/denomenator);
	if (cf >= .5){
		printf("YES, CF = %lf\n", cf);
	}
	else{
		printf("NO, CF = %lf\n", cf);
	}
	printf("Sample Size = %d\nYsum = %lf\n", maxSample, ysum);
}

//void convertDate(struct WeatherFactor* weatherFactor, char** argv);
void checkArgs(int argc, char* argv[]);
int CheckDate(struct tm* date){		//Return 0 for invalid date, 1 for valid date
	//invalid Dates
	/*6/29, 6/30
	 * 7/1,2,5,6,18,19,20,24,25,26
	 * 8/4,5,6,7
	 */

	int month = date->tm_mon;
	int day = date->tm_mday;

	//Months have to be minus 1 because of the date time struct is 0-11
	if (month == 5){
		if(day == 29 || day == 30){
			return 0;
		}
	}
	else if (month == 6){
		if(day == 1 || day == 2 || day == 5 || day == 6
				|| day == 18 || day == 19 || day == 20
				|| day == 24 || day == 25 || day == 26){
			return 0;
		}
	}
	else if (month== 7){
		if(day == 4 || day == 5 || day == 6 || day == 7){
			return 0;
		}
	}
	else{
		return 1;
	}
	return 1;
}

//prints each data set
void cb1 (void *src, size_t len, void *cData) {
	static int monthFlag, dayFlag, hourFlag = 0;		//flags for when we have found the correct dates
	//static validRowFlag = 0;
	//struct tm date;										//temp date struct
	char buf[100];			//String buffer

	len = csv_write(buf, 100, src, len);
	char* cellData = strtok(buf, "\"");

	//Convert the string to a comparable and readable format
//	char *cellData = (char*) malloc(sizeof(char*)*len);
//	memcpy(cellData, (char *)src, len);	//Copy the string into a placeholder
	struct Correlation *mycData = (struct Correlation*)cData;					//convert the cData pointer
	//struct WeatherFactor *myfactor = (struct WeatherFactor*)mycData->weatherFactor;		//convert to weatherFactor pointer

	//printf("%s\n", cellData);	//Print the data to Screen

	if (CURRENT_WEATHER_ROW_NUM > 0
			&& sampleSize > 0
			&& CURRENT_COLUMN_NUM > 0
			&& CURRENT_COLUMN_NUM < 4){	//to access the correct Date Data
		struct tm *datetofind = &mycData->dateArray[sampleSize-1];

		if (CURRENT_COLUMN_NUM == 1 &&
				datetofind->tm_mon == (atoi(cellData)-1) &&
				monthFlag != 1){		//The Month Column
			monthFlag = 1;
		}
		else if (CURRENT_COLUMN_NUM == 2 &&
				datetofind->tm_mday == atoi(cellData) &&
				monthFlag &&
				dayFlag != 1){		//The Month Column
			dayFlag = 1;
		}
		else if (CURRENT_COLUMN_NUM == 3 &&
				datetofind->tm_hour == atoi(cellData)
				&& monthFlag &&
				dayFlag
				&& hourFlag != 1){		//The Month Column
			hourFlag = 1;
		}
	}
//	else if (CURRENT_WEATHER_ROW_NUM == 0){
		//We need to first find the column that the weather factor is in
	if (currentDataColumn == -1 && strncmp(mycData->weatherFactorName, cellData, len-2 ) == 0){
		currentDataColumn = CURRENT_COLUMN_NUM;		//found the column number
//		}
	}

	if (monthFlag && dayFlag && hourFlag && CURRENT_COLUMN_NUM == currentDataColumn){	//found data
		if (cellData == NULL){
		//if (strncmp("", cellData, 1) == 0){		//Missing data
			//printf("The data is missing and not available.\n");
			//printf("SS = %d, Data = %s\n", sampleSize - 1, cellData);					//print the data found
			mycData->weatherDataY[--sampleSize] = 0;		//Data missing enter 0
			//--sampleSize;					//Decrease the sample size, data done
			foundFlag = 0;					//reset the found flag
			monthFlag = 0;					//reset flags
			dayFlag = 0;
			hourFlag = 0;
			//CURRENT_WEATHER_ROW_NUM = 0;	//reset the row number
		}
		else{	//Output the Found data
			//printf("SS = %d, Data = %s\n", sampleSize - 1, cellData);					//print the data found
			mycData->weatherDataY[--sampleSize] = atof(cellData);		//assign the data as float
			//--sampleSize;					//Decrease the sample size, data done
			foundFlag = 0;					//reset the found flag
			monthFlag = 0;					//reset flags
			dayFlag = 0;
			hourFlag = 0;
			//CURRENT_WEATHER_ROW_NUM = 0;	//reset the row number
		}
	}

	CURRENT_COLUMN_NUM++;
}
//prints the end of line char
void cb2 (int c, void *weatherFactor) {
	CURRENT_COLUMN_NUM = 0;
//	if (CURRENT_WEATHER_ROW_NUM == 8786){
//		CURRENT_WEATHER_ROW_NUM = 0;	//reset
//	}
//	else
		CURRENT_WEATHER_ROW_NUM++;		//increase
}

//prints each data set
void cb3 (void *src, size_t len, void *cData) {
	static int currentLOAColumn,		//Keep track of where certain data is in each column
				currentTimeColumn,
				currentDurationColumn,
				currentCauseColumn;
	struct tm date;						//for parsing date information
	static int validRowFlag = 0;		//for when you are on a correct row
	static int validDateFlag = 0;		//found a date of an outtage
	static int validCauseFlag = 0;		//found a cause in the current row
	int currentLOA;
	char buf[50];		//String Buffer
	//const char* myCellData;
	//int timeDuration;			//time duration of the outtage

	len = csv_write(buf, 50, src, len);			//Copy to Buffer for easy parsing & string Management
	char* myCellData = strtok(buf, "\"");		//Get rid of the Quotes & add an /0 char

	//Convert the string to a comparable and readable format
	//char *myCellData = (char*) malloc(sizeof(char)*len);
	//memcpy(myCellData, (char *)s, len);	//Copy the string into a placeholder
	struct Correlation *mycData = (struct Correlation*)cData;				//Convert the Pointer
	//struct OuttageFactor *myfactor = (struct OuttageFactor*)outtageData;		//convert to weatherFactor pointer
	//struct OuttageFactor *myfactor = mycData->outtageFactor;
	int myMonth = mycData->startMonth;										//Track the month
	int durationofMonths = mycData->durationofMonths;						//Track the amount of months

	//printf("%s\n", cellData);	//Print the data to Screen

	if (CURRENT_OUTTAGE_ROW_NUM > 0){						//to access the correct Data and not the header
		if (CURRENT_COLUMN_NUM == currentLOAColumn){		//Checking the LOA Column
			if(mycData->LOA == atoi(myCellData)){			//Check for the correct LOA
				currentLOA = atoi(myCellData);
				validRowFlag = 1;							//We are on the correct LOA Row
			}
		}
		//Now we are on the correct row and have found an outtage, now we need to get the date so
		//we can get the matching weather data
		else if(currentTimeColumn == CURRENT_COLUMN_NUM && validRowFlag){	//Get the Date
			strptime(myCellData, "%m/%d/%y %H:%M", &date);
			validDateFlag = 1;
		}
		else if(currentCauseColumn == CURRENT_COLUMN_NUM && validRowFlag){	//Get the Cause
			if (strcmp(mycData->outageCauseName, myCellData) == 0){			//Found the Cause
				validCauseFlag = 1;
			}
		}
	}
	else{		//We need to first find the column that the LOA is in
		if (strcmp("LOA", myCellData) == 0){
			currentLOAColumn = CURRENT_COLUMN_NUM;		//found the column number that will contain the LOA
		}
		else if (strcmp("REPORTED", myCellData) == 0){
			currentTimeColumn = CURRENT_COLUMN_NUM;
		}
		else if (strcmp("CAUSE", myCellData) == 0){
			currentCauseColumn = CURRENT_COLUMN_NUM;
		}
		else if (strcmp("DURATION_HRS", myCellData) == 0){
			currentDurationColumn = CURRENT_COLUMN_NUM;
		}
	}

	if (validDateFlag &&
			validCauseFlag &&
			date.tm_mon >= myMonth - 1 &&
			date.tm_mon < myMonth+durationofMonths ){
		//for (int i = myMonth; i < durationofMonths; i++){	//Need to see if we actually do work, and the months match my months
			//If the range of months we are looking for match the current found month
			//and it is not a significant Date
		if (CheckDate(&date)){							//Do some work and add the date to get the weather data
			//printf("%d Row=%d LOA=%d, DATE=%s\n", sampleSize, CURRENT_OUTTAGE_ROW_NUM, currentLOA, myCellData);
			memcpy(&mycData->dateArray[sampleSize++], &date, sizeof(date));	//This is the date we need to match in the weather data
			foundFlag = 1;						//set the found flag to exit the outtage data and look for the weather data
			validDateFlag = 0;					//reset flag
			validRowFlag = 0;					//Reset Flag
			mycData->outtageDataX++;	//increase the outage number for the sample size
		}
	}
	//printf("%d\n", CURRENT_COLUMN_NUM++, CURRENT_OUTTAGE_ROW_NUM);
	CURRENT_COLUMN_NUM++;
	//free(cellData);
}

//prints the end of line char
void cb4 (int c, void *weatherFactor) {
	CURRENT_COLUMN_NUM = 0;
	CURRENT_OUTTAGE_ROW_NUM++;			//Never reset
	//printf("\n");
}

char* getfield(char* line, int num){
    char* tok;
    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n")){
        if (!--num)
            return tok;
    }
    return NULL;
}

/* This program takes a minimum of 3 arguments
 * 1 - The LOA that we are matching the weather data to
 * 2 - The Outage Cause that we want to correlate
 * 2 - The Weather factor that is found in the data files given which must match a header
 *
 * The program will output the Data to the user whether or not there is a correlation
 */

int main(int argc, char **argv){
	int rc;
	char buf[1024];				//
	size_t i;					//
	struct csv_parser wf;		//the CSV Parser Struct for weather
	struct csv_parser of;		//struct for outtage data
	FILE *weatherFile;
	FILE *outtageFile;
	csv_init(&of, 0);			//Outage CSV
	csv_init(&wf, 0);			//Weather CSV
	struct Correlation* cData = (struct Correlation*)malloc(sizeof(struct Correlation*));
	cData->outtageDataX = 0;			//adds the amount of outtages

	/*	MPI INITS	*/
	int myrank,
		numProcessors,
		s,
		startMonth = 0;

	MPI_Status status;

	rc = MPI_Init(&argc, &argv);						//init MPI World
	if (rc != MPI_SUCCESS){
	     printf ("Error starting MPI program. Terminating.\n");
	     MPI_Abort(MPI_COMM_WORLD, rc);
	}

	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);	//get number of processors
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);		//get processor rank (id)

   printf ("Number of CPUs= %d My rank= %d\n", numProcessors,myrank);

				/*	PHASE I	*/

	s = MONTHS/numProcessors;	//months per processor

	if(myrank == MASTER){	//Master
		//printf("We have %d processors\n", numProcessors);

		for (int i = 1, startMonth = s; i < numProcessors; startMonth = startMonth+s, i++ ){	//Send out the data
			printf("Master -> Sending out to CPU %d\n", i);
			rc = MPI_Send(&startMonth,	//number of the starting month
					s, 			//amount of months to process
					MPI_INT, 	//data type
					i, 			//Who should be receiving the data
					i,			//Tag
					MPI_COMM_WORLD);	//Send the data to the processors
			if (rc != MPI_SUCCESS){
				printf("%d: Send failure on round %d\n", myrank, i);
			}
			printf("Data Sent Success!\n");
		}
	}
	else{	//Slaves receive from Master
		for (i = 1; i < numProcessors; i++){
			printf("Node %d Receiving\n", i);
			rc = MPI_Recv(&startMonth, 		//Your starting Month
					s, 						//How many months your getting
					MPI_INT, 				//type of data received
					MASTER, 				//From where
					i, 						//Which processor you are
					MPI_COMM_WORLD, 		//Tow which processors
					&status);				//Success or Failure
			printf("Processor %d starting on month %d for %d months\n", i, startMonth, s);	//Debug Message

			if (rc != MPI_SUCCESS){
				printf("Processor %d: Receive failure on round %d\n", (int)myrank, i);
			}
		}
	}

				/*	PHASE II	*/
	//FOR TESTING PURPOSES ONLY
	//MASTER
//	cData->durationofMonths = 2;
//	cData->startMonth = 11;

	printf("Begin Computing");

	checkArgs(argc, argv);						//Confirm the args are correct
	cData->LOA = atoi(argv[1]);					//assign the LOA
	cData->outageCauseName = argv[2];			//Assign the outage cause
	cData->weatherFactorName = argv[3];			//Assign the weather Factor
	cData->startMonth = startMonth;				//Assign the start month
	cData->durationofMonths = s;				//Assign the Number of Months to Compute

	//open the file to Read, Using "rb" for non-text files
	weatherFile = fopen(GOSHEN_WEATHER_FILE, "rb");
	if (weatherFile == NULL) {
		fprintf(stderr, "Failed to open file %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	outtageFile = fopen(OUTTAGE_FILE, "rb");
	if (outtageFile == NULL) {
		fprintf(stderr, "Failed to open file %s: %s\n", argv[2], strerror(errno));
		exit(EXIT_FAILURE);
	}

	//reading and parsing the weather Data File for Data
	while ((i=fread(buf, 1, 1024, outtageFile)) > 0) {	//For each line
		if (csv_parse(&of, buf, i, cb3, cb4, cData) != i) {			//continue until flag is set
			fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&wf)));
			fclose(weatherFile);
			fclose(outtageFile);
			remove(argv[2]);
			exit(EXIT_FAILURE);
		}
	}
	maxSample = sampleSize;		//Set the Max size
	CURRENT_COLUMN_NUM = 0;		//reset column number
	while(sampleSize != 0){
		while((i = fread(buf, 1, 1024, weatherFile)) > 0){	//find the weather data of the outtage found
			if (csv_parse(&wf, buf, i, cb1, cb2, cData) != i){
				fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&wf)));
				fclose(weatherFile);
				fclose(weatherFile);
				//remove(argv[2]);
				exit(EXIT_FAILURE);
			}
		}
		//CURRENT_WEATHER_ROW_NUM = 0;	//Reset the Row for Weather

		rewind(weatherFile);		//Reset the file pointer
	}
//	while( sampleSize != 0){
//		readWeatherFile(weatherFile, cData);		//Read the weather file each time to find the date
//	}

	ComputeCorrelation(cData);

	printf("SUCCESS for Processor %d!\n", myrank);

	csv_fini(&wf, cb1, cb2, cData);
	csv_free(&wf);
	csv_fini(&of, cb1, cb2, cData);
	csv_free(&of);

			/*	PHASE III	*/
	int finished = 1;

	if (myrank == MASTER){
		//Master Receives the data back from the Slaves
		printf("MASTER is receiving\n");
		for(int i = 1; i < numProcessors; i++){
			rc = MPI_Recv(&finished, 		//complete flag
					1, 						//amount of data
					MPI_INT, 				//type of data received
					i, 						//From where
					MASTER, 				//Which processor you are
					MPI_COMM_WORLD, 		//To which processors
					&status);				//Success or Failure
			if (rc != MPI_SUCCESS){
				printf("%d: Receive failure on round %d\n", myrank, i);
			}
			printf("MASTER Received from CPU %d\n", myrank);
		}
		printf("MASTER is done Receiving\n");
	}
	else{
		for(int i = 1; i < numProcessors; i++){
			printf("CPU %d is Sending to MASTER!\n", myrank);
			rc = MPI_Send(&finished,	//number of the starting month
				1,			 			//amount of data
				MPI_INT, 				//data type
				MASTER, 				//Who should be receiving the data
				i,						//Tag
				MPI_COMM_WORLD);		//Send the data to the processors
			if (rc != MPI_SUCCESS){
				printf("%d: Send failure on round %d\n", myrank, i);
			}
		}
		printf("CPU %d is done Sending\n", myrank);
	}

	//Close Files
	fclose(weatherFile);
	fclose(outtageFile);
	return EXIT_SUCCESS;
}

//void readWeatherFile(FILE* weatherFile, struct Correlation* cData ){
//	char buf[1024];
//	struct csv_parser wf;
//	csv_init(&wf, 0);
//	int i;
//
//	while((i = fread(buf, 1, 1024, weatherFile)) > 0){	//find the weather data of the outtage found
//		if (csv_parse(&wf, buf, i, cb1, cb2, cData) != i){
//			fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&wf)));
//			fclose(weatherFile);
//			fclose(weatherFile);
//			//remove(argv[2]);
//			exit(EXIT_FAILURE);
//		}
//	}
//	csv_fini(&wf, cb1, cb2, cData);
//	csv_free(&wf);
//}

//void convertDate(struct Correlation* cData, char** argv){
//	int dateInt;
//	struct tm date;
//	char* month;
//	char* day;
//	char* hour;
//
//	for(int i = 3; i < 6; i++){
//		dateInt = atoi(argv[i]);
////		dateInt = strtoimax(argv[i], NULL, 10);
////		if (dateInt > UINTMAX_MAX && errno == ERANGE){
////			fprintf(stderr, "Failed to convert to Integer %s: %s\n", argv[i], strerror(errno));
////			exit(EXIT_FAILURE);
////		}
//		if (dateInt > 0 && dateInt < 13 && i == 3){		//Valid Month Entered
//			date.tm_mon = dateInt-1;	//set date time struct Month
//			month = argv[i];
//		}
//		else if(dateInt > 0 && dateInt < 32 && i == 4){
//			date.tm_mday = dateInt;	//set date time struct Day
//			day = argv[i];
//		}
//		else if(dateInt >= 0 && dateInt < 24 && i == 5){
//			date.tm_hour = dateInt;	//Set hour
//			hour = argv[i];
//		}
//	}
//
//	if(!CheckDate(&date)){
//		fprintf(stderr, "This is a Major Event Day. Enter another Date. %s/%s.\n", month, day);
//		exit(EXIT_FAILURE);
//	}
//	cData->month = month;
//	cData->day = day;
//	cData->hour = hour;
//}

void checkArgs(int argc, char* argv[]){
	//Need at least 2 arguments
//	if (argc != 6) {
//		fprintf(stderr, "Error: Need the Hour.\n");
//		exit(EXIT_FAILURE);
//	}
//	//Need at least 3 arguments
//	if (argc < 5) {
//		fprintf(stderr, "Error: Need the Day.\n");
//		exit(EXIT_FAILURE);
//	}

	//Need at least 2 arguments
	if (argc < 4) {
		fprintf(stderr, "Error: Need the LOA, Outage Cause, and Weather Factor.\n");
		exit(EXIT_FAILURE);
	}

//	//Cannot name the files the same
//	if (!strcmp(argv[1], argv[3])) {
//		fprintf(stderr, "Input file and output file must not be the same!\n");
//		exit(EXIT_FAILURE);
//	}
}
