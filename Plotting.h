
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <ctime>
#include "Matrix.h"
#include "DataRetriever.h"
#include "Instrument.h"
#include "StockMap.h"
#include <numeric>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

namespace fre{


void plotResults( vector <double> yData1, vector <double> yData2, vector <double> yData3, int dataSize) {
    FILE *gnuplotPipe,*tempDataFile;
    const char *tempDataFileName1 = "Beat";
    const char *tempDataFileName2 = "Meet";
    const char *tempDataFileName3 = "Miss";
    double x1,y1, x2, y2 , x3,y3;
    int i;
    const char *title = "Avg CAAR for Groups";
    const char *yLabel = "Avg CAAR";
    gnuplotPipe = popen("gnuplot -persist", "w");
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set xlabel 'Announcement Date'\nset ylabel '%s'\n", yLabel);
    if (gnuplotPipe) {
        fprintf(gnuplotPipe,"plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n",tempDataFileName1, tempDataFileName2 , tempDataFileName3);
        fflush(gnuplotPipe);
        tempDataFile = fopen(tempDataFileName1,"w");
        for (i=-dataSize; i < dataSize; i++) {
            x1 = i;
            y1= yData1[i+dataSize];
            fprintf(tempDataFile,"%lf %lf\n",x1,y1);
        }
        fclose(tempDataFile);
        tempDataFile = fopen(tempDataFileName2,"w");
        for (i=-dataSize; i < dataSize; i++) {
            x2 = i;
            y2 = yData2[i+dataSize];
            fprintf(tempDataFile,"%lf %lf\n",x2,y2);
        }
        fclose(tempDataFile);
        tempDataFile = fopen(tempDataFileName3,"w");
        for (i=-dataSize; i < dataSize; i++) {
            x3 = i;
            y3 = yData3[i+dataSize];
            fprintf(tempDataFile,"%lf %lf\n",x3,y3);
        }       
        fclose(tempDataFile);
        printf("press enter to continue...");
        getchar();
    } else {        
        printf("gnuplot not found...");    
    }
} 
}