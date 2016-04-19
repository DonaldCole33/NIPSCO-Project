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
#include <mpi.h>
#include <math.h>
#include "data.h"

static int CURRENT_COLUMN_NUM = 0;
static int CURRENT_OUTTAGE_ROW_NUM = 0;
static int CURRENT_WEATHER_ROW_NUM = 0;
static int sampleSize = 0;
static int maxSample;
static int foundHeaderFlag = 0;
const int numOfCauses = 50;

void copyDate(struct tm* newDate, struct tm* oldDate){
	newDate->tm_mon = oldDate->tm_mon;
	newDate->tm_mday = oldDate->tm_mday;
	newDate->tm_hour = oldDate->tm_hour;
}


//void convertDate(struct WeatherFactor* weatherFactor, char** argv);
void checkArgs(int argc, char* argv[]);

void ComputeCorrelation(struct Correlation* cData){
	//first sum the X and Y data
	double xsum = 0;
	double ysum = 0;
	double xysum = 0;
	double xsqSum = 0;
	double ysqSum = 0;
	int currentSampleSize = 0;		//sample size per month
	int startMonth = cData->startMonth;
	int endMonth = startMonth + cData->durationofMonths;
	double numerator = 0;
	double denomenator = 0;
	double cf = 0;
	char* month;

	printf("  MONTH   |  DECISION  |  C FACTOR  |  SAMPLES\n");

	for (int i = startMonth; i < endMonth; i++){	//Compute CF for each month
		for (int j = 0; j < 31; j++){
			if (cData->outtageDataX[i][j] > 0){
				currentSampleSize++;
				ysum += cData->weatherDataY[i][j];
				ysqSum += pow(cData->weatherDataY[i][j], 2);
				xsum += cData->outtageDataX[i][j];
				xsqSum += pow(cData->outtageDataX[i][j], 2);
				xysum += cData->weatherDataY[i][j]*cData->outtageDataX[i][j];
			}
		}

		numerator = currentSampleSize * xysum - (xsum * ysum);
		denomenator = sqrt((currentSampleSize * xsqSum - pow(xsum,2)) *(currentSampleSize * ysqSum - pow(ysum,2)));

		if (denomenator != 0){
			cf = (numerator/denomenator);
		}
		else{
			cf = 0;
		}

		switch (i){
			case 0:
				month = "January";
				break;
			case 1:
				month = "February";
				break;
			case 2:
				month = "March";
				break;
			case 3:
				month = "April";
				break;
			case 4:
				month = "May";
				break;
			case 5:
				month = "June";
				break;
			case 6:
				month = "July";
				break;
			case 7:
				month = "August";
				break;
			case 8:
				month = "September";
				break;
			case 9:
				month = "October";
				break;
			case 10:
				month = "November";
				break;
			case 11:
				month = "December";
				break;
			default:
				month = "Missing Month";
				break;
		}

		if (cf >= .5){
			printf("%-10s%9.5s%14.3lf%10d\n", month, "*YES*", cf, currentSampleSize);
		}
		else{
			printf("%-10s%9.4s%14.3lf%10d\n", month, "*NO*", cf, currentSampleSize);
		}

		//Reset all variables
		ysum = 0; ysqSum = 0; xsum = 0; xsqSum = 0; xysum = 0;
		numerator = 0; denomenator = 0; cf = 0; currentSampleSize = 0;
	}
}

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
	static int 	monthColumn,
				dayColumn,
				hourColumn;
	static int weatherDataColumn = -1;
	static int monthFlag, dayFlag, hourFlag = 0;		//flags for when we have found the correct dates
	char buf[100];			//String buffer
	char* cellData;
	const struct tm *datetofind;

	if (len != 0){
		len = csv_write(buf, 100, src, len);
		cellData = strtok(buf, "\"");										//Convert to String
	}
	else{
		cellData = "\0";
	}

	struct Correlation *mycData = (struct Correlation*)cData;				//convert the cData pointer

	if (sampleSize > 0){
		datetofind = mycData->dateArray[sampleSize - 1];
	}

//	printf("%s\n", cellData);	//Print the data to Screen

	if (CURRENT_WEATHER_ROW_NUM > 0){	//to access the correct Date Data

		if (CURRENT_COLUMN_NUM == monthColumn &&
				datetofind->tm_mon == (atoi(cellData)-1) &&
				monthFlag != 1){		//The Month Column
			monthFlag = 1;
		}
		else if (CURRENT_COLUMN_NUM == dayColumn &&
				datetofind->tm_mday == atoi(cellData) &&
				monthFlag &&
				dayFlag != 1){		//The Month Column
			dayFlag = 1;
		}
		else if (CURRENT_COLUMN_NUM == hourColumn &&
				datetofind->tm_hour == atoi(cellData)
				&& monthFlag &&
				dayFlag
				&& hourFlag != 1){		//The Month Column
			hourFlag = 1;
		}
	}
	else if (foundHeaderFlag == 0){		//We need to first find the column that the LOA is in
		if (weatherDataColumn && (strcmp(mycData->weatherFactorName, cellData) == 0)){
			weatherDataColumn = CURRENT_COLUMN_NUM;		//found the column number
			foundHeaderFlag = 1;
		}
		else if (strcmp("Month Local", cellData) == 0){
			monthColumn = CURRENT_COLUMN_NUM;		//found the column number that will contain the LOA
		}
		else if (strcmp("Day Local", cellData) == 0){
			dayColumn = CURRENT_COLUMN_NUM;
		}
		else if (strcmp("Hour Local", cellData) == 0){
			hourColumn = CURRENT_COLUMN_NUM;
		}
	}

	if (monthFlag && dayFlag && hourFlag && CURRENT_COLUMN_NUM == weatherDataColumn){	//found data
		if (strcmp(cellData, "") == 0){
			printf("The data is missing and not available.\n");
			mycData->outtageDataX[datetofind->tm_mon][datetofind->tm_mday - 1] -= 1;		//Remove the outage data
			--sampleSize;					//Decrease the sample size, data done
			monthFlag = 0;					//reset flags
			dayFlag = 0;
			hourFlag = 0;
		}
		else{	//Output the Found data
			printf("SS = %d, Data = %s\n", sampleSize - 1, cellData);					//print the data found
			mycData->weatherDataY[datetofind->tm_mon][datetofind->tm_mday - 1] += atof(cellData);			//add the amount of Data
			--sampleSize;					//Decrease the sample size, data done
			monthFlag = 0;					//reset flags
			dayFlag = 0;
			hourFlag = 0;
		}
	}

	CURRENT_COLUMN_NUM++;
}

//prints the end of line char
void cb2 (int c, void* cData) {
	//printf("wRow = %d\n", CURRENT_WEATHER_ROW_NUM);
	CURRENT_COLUMN_NUM = 0;			//reset
	CURRENT_WEATHER_ROW_NUM++;		//increase
}

//prints each data set
void cb3 (void *src, size_t len, void *cData) {
	static int currentLOAColumn,		//Keep track of where certain data is in each column
				currentTimeColumn,
				currentDurationColumn,
				currentCauseColumn;
	static struct tm date;						//for parsing date information
	static int validRowFlag = 0;		//for when you are on a correct row
	static int validDateFlag = 0;		//found a date of an outtage
	static int validCauseFlag = 0;		//found a cause in the current row
	static int currentLOA;
	char buf[50];						//String Buffer
	char* myCellData;

	if (len != 0){
		len = csv_write(buf, 50, src, len);						//Copy to Buffer for easy parsing & string Management
		myCellData = strtok(buf, "\"");					//Get rid of the Quotes & add an /0 char
	}
	else{
		myCellData = "\0";
	}

	struct Correlation *mycData = (struct Correlation*)cData;				//Convert the Pointer
	int myMonth = mycData->startMonth;										//Track the month
	int durationofMonths = mycData->durationofMonths;						//Track the amount of months

//	printf("Data :%s\n", myCellData);	//Print the data to Screen

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
			strptime(myCellData, "%Y/%m/%d %H:%M:%S", &date);
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
			validRowFlag &&
			date.tm_mon >= myMonth &&
			date.tm_mon < myMonth + durationofMonths ){
		printf("**************\n");
			//and it is not a significant Date
		if (CheckDate(&date)){							//Do some work and add the date to get the weather data
			//Need to hash the data found
			//char dateBuf[50];
			//strftime(dateBuf, 50, "%D %R", &date);
			printf("%d LOA=%d, DATE=\n", sampleSize, currentLOA);
			//struct tm* p = (struct tm*)malloc(sizeof(struct tm*));
			//memcpy (p, &date, sizeof(date));
			//memcpy(mycData->dateArray[sampleSize++], p, sizeof(struct tm));	//This is the date we need to match in the weather data
			//mycData->dateArray[sampleSize++] = p;
			copyDate(mycData->dateArray[sampleSize++], &date);

			mycData->outtageDataX[date.tm_mon][date.tm_mday-1]++;	//number of outtages per month
			validDateFlag = 0;					//reset flag
			validRowFlag = 0;					//Reset Flag
			validCauseFlag = 0;
		}
		else{		//found Invalid Date, reset flags
			validDateFlag = 0;					//reset flag
			validRowFlag = 0;					//Reset Flag
			validCauseFlag = 0;

		}
	}

	//printf("%d\n", CURRENT_COLUMN_NUM++);
	CURRENT_COLUMN_NUM++;
}

//prints the end of line char
void cb4 (int c, void* cData) {
	//printf("%d\n", CURRENT_OUTTAGE_ROW_NUM);
	CURRENT_COLUMN_NUM = 0;
	CURRENT_OUTTAGE_ROW_NUM++;			//Never resets
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
	char buf[1024];
	size_t i;
	struct csv_parser wf;		//the CSV Parser Struct for weather
	struct csv_parser of;		//struct for outtage data
	FILE *weatherFile;
	FILE *outtageFile;
	csv_init(&of, CSV_APPEND_NULL || CSV_EMPTY_IS_NULL);			//Outage CSV
	csv_init(&wf, CSV_APPEND_NULL || CSV_EMPTY_IS_NULL);			//Weather CSV
	struct Correlation* cData = (struct Correlation*) malloc(sizeof(struct Correlation*));
	int r = 12;
	int c = 31;

//	char options = CSV_APPEND_NULL || CSV_EMPTY_IS_NULL;
//	csv_set_opts(&of, options);
//	csv_set_opts(&wf, options);

	//*** THESE ARE THE NEW
	for (int i = 0; i < 1000; i++){
		cData->dateArray[i] = (struct tm*)malloc(sizeof(struct tm*));
	}

	for (int i = 0; i < 12; i++){
		for (int j = 0; j<31; j++){
			cData->weatherDataY[i][j] = 0;
			cData->outtageDataX[i][j] = 0;
		}
	}

	//***THESE ARE THE PREVIOUS
//	cData->weatherDataY = (double**) malloc(r * sizeof(double*));
//	cData->outtageDataX = (int**) malloc(r * sizeof(int*));
//
//	for (int i = 0; i < 1000; i++){
//		cData->dateArray[i] = (struct tm*)malloc(sizeof(struct tm*));
//	}
//
//	for (int i = 0; i < r; i++){
//		if((cData->weatherDataY[i] = malloc(c * sizeof(double))) == NULL){
//			printf("Error on Columns\n");
//		}
//	}
//
//	for(int i = 0; i < r; i++){
//		if((cData->outtageDataX[i] = (int*)malloc(c * sizeof(int))) == NULL){
//			printf("Error on outage columns\n");
//		}
//	}

	/*	MPI INITS	*/
//	int myrank,
//		numProcessors,
//		s,
//		startMonth = 0;
//
//	MPI_Status status;
//
//	printf("Initializing MPI.\n");
//
//	rc = MPI_Init(&argc, &argv);						//init MPI World
//	if (rc != MPI_SUCCESS){
//	     printf ("Error starting MPI program. Terminating.\n");
//	     MPI_Abort(MPI_COMM_WORLD, rc);
//	}
//
//	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);	//get number of processors
//	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);		//get processor rank (id)
//
//   printf ("Number of CPUs= %d My rank= %d\n", numProcessors,myrank);
//
//				/*	PHASE I	*/
//
//	s = MONTHS/numProcessors;	//months per processor
//
//	if(myrank == MASTER){	//Master
//		for (int i = 1, startMonth = s; i < numProcessors; startMonth = startMonth+s, i++ ){	//Send out the data
//			printf("Master -> Sending out to CPU %d\n", i);
//			rc = MPI_Send(&startMonth,	//number of the starting month
//					s, 			//amount of months to process
//					MPI_INT, 	//data type
//					i, 			//Who should be receiving the data
//					0,			//Tag
//					MPI_COMM_WORLD);	//Send the data to the processors
//			if (rc != MPI_SUCCESS){
//				printf("MASTER: Send failure to CPU %d\n", i);
//			}
//			printf("Data Sent to CPU %d!\n", i);
//		}
//	}
//	else{	//Slaves receive from Master
//		printf("CPU %d Receiving\n", myrank);
//		rc = MPI_Recv(&startMonth, 		//Your starting Month
//				s, 						//How many months your getting
//				MPI_INT, 				//type of data received
//				MASTER, 				//From where
//				0, 						//Tag
//				MPI_COMM_WORLD, 		//To which group of processors
//				&status);				//Success or Failure
//		printf("Processor %d starting on month %d for %d months\n", myrank, startMonth, s);	//Debug Message
//
//		if (rc != MPI_SUCCESS){
//			printf("Processor %d: Receive failure.\n", (int)myrank);
//		}
//	}

				/*	PHASE II	*/
	//FOR TESTING PURPOSES ONLY
	//MASTER
	cData->durationofMonths = 12;
	cData->startMonth = 0;

	printf("Begin Computing\n");

	checkArgs(argc, argv);						//Confirm the args are correct
	cData->year = atoi(argv[1]);				//assign the Year
	cData->LOA = atoi(argv[2]);					//assign the LOA
	cData->outageCauseName = argv[3];			//Assign the outage cause
	cData->weatherFactorName = argv[4];			//Assign the weather Factor
//	cData->startMonth = startMonth;				//Assign the start month
//	cData->durationofMonths = s;				//Assign the Number of Months to Compute

	weatherFile = findWeatherFile(cData->year, cData->LOA);	//open the file to Read, Using "rb" for non-text files
	outtageFile = findOutageFile(cData->year);

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

//	csv_fini(&of, cb3, cb4, cData);
//	csv_free(&of);

	printf("DOOOOOOOOOONE\n");
	maxSample = sampleSize;		//Set the Max size
	CURRENT_COLUMN_NUM = 0;		//reset column number

	while(sampleSize - 1 > 0){
		printf("sampleSize = %d\n", sampleSize);
		while((i = fread(buf, 1, 1024, weatherFile)) > 0){	//find the weather data of the outtage found
			if (csv_parse(&wf, buf, i, cb1, cb2, cData) != i){
				fprintf(stderr, "Error parsing file: %s\n", csv_strerror(csv_error(&wf)));
				fclose(weatherFile);
				fclose(weatherFile);
				//remove(argv[2]);
				exit(EXIT_FAILURE);
			}
		}

		csv_free(&wf);				//Free the pointer to reset the data
		rewind(weatherFile);		//Reset the file pointer
		CURRENT_WEATHER_ROW_NUM = 0;	//Reset the row number
	}

	ComputeCorrelation(cData);

//	printf("SUCCESS for Processor %d!\n", myrank);

	csv_fini(&wf, cb1, cb2, cData);
	csv_free(&wf);


			/*	PHASE III	*/
//	int finished = 1;
//
//	if (myrank != MASTER){		//Slaves Sending Back info
//		int done = 1;
//		printf("CPU %d is Sending to Master.\n", myrank);
//		rc = MPI_Send(&finished, 		//void* data
//				done, 					//int count
//				MPI_INT, 				//data type
//				MASTER, 				//int destination
//				0, 						//int Tag
//				MPI_COMM_WORLD); 		//MPI_COMM Communicator
//		if (rc != MPI_SUCCESS){
//			printf("CPU %d: Send failure to Master.\n", myrank);
//		}
//
//		printf("CPU %d is done Sending to Master.\n", myrank);
//	}
//	else{	//Master Receives data from slaves
//		for(int i = 1; i < numProcessors; i++){
//			printf("MASTER is receiving from CPU %d!\n", i);
//			rc = MPI_Recv(&finished, 				//void* data
//							1, 						//int count
//							MPI_INT, 				//data type
//							i,		 				//int destination
//							0, 						//int Tag
//							MPI_COMM_WORLD, 		//MPI_COMM Communicator
//							&status);				//MPI_STATUS* status
//			if (rc != MPI_SUCCESS){
//				printf("%d: Send failure on round %d.\n", myrank, i);
//			}
//			printf("MASTER Received from CPU %d.\n", i);
//		}
//		printf("MASTER is done Receiving.\n");
//	}
//
//	MPI_Finalize();


	//Close Files
	fclose(weatherFile);
	fclose(outtageFile);

	//Free Data Structures

	for(int i = 0; i < 1000; i++){
		free(cData->dateArray[i]);
	}
	free(cData);
	printf("**Program Completed**\n");

	return EXIT_SUCCESS;
}


void checkArgs(int argc, char* argv[]){

	//Need at least 4 arguments
	if (argc < 5) {
		fprintf(stderr, "Error: Need the Year, LOA, Outage Cause, and Weather Factor.\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < numOfCauses; i++){
		if (strcmp(argv[3], causeArray[i]) == 0){
			printf("Found a Valid Cause.\n");
			return;
		}
	}

	fprintf(stderr, "Failed to Enter a valid Cause\n");
	exit(EXIT_FAILURE);
}
