#pragma once
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
	class calculations 
	{
		private:
			int sampleSize = 80, iterations = 40, days;
			Matrix AAR;
			Matrix CAAR;
			vector<string> tickers; // Creating a vector of all the tickers in the given map
			vector<vector<string>> simTickers; // Creating a vector of the 80 stocks used in 40 simulations
			Vector averageAAR;
			Vector averageCAAR;
			Vector stdDevAAR;
			Vector stdDevCAAR;
			
		public:
			calculations(){}
			Matrix getCAAR() { return CAAR; }
			Matrix getAAR() { return AAR; }
			Vector getAverageAAR() { return averageAAR; }
			Vector getAverageCAAR() { return averageCAAR;}
			Vector getStdDevAAR() { return stdDevAAR; }
			Vector getStdDevCAAR() { return stdDevCAAR;}
			void setDays(int N) { days = 2*N; }
			
			void calcs(Index& ind, map<string, Stock*>& myMap)
			{	
				//cout << "Start Calculations"<< endl;
				clearAll();
				map<string, Stock*>::iterator itr;
				for (auto itr = myMap.begin(); itr != myMap.end(); itr++)
				{
					setReturns(itr->second);
					setAbnormalReturns(itr->second, ind);
				}
				
				validateStocks(myMap);

				cout << "Finished calculating validateStocks"<< endl;
				calcSimTickers(myMap);
				cout<< "Finished making simulation data" << endl;
				setAverageAbnormalReturns(myMap);
				cout<< "Finished calculating AAR" << endl;
				setCumulativeAbnormalReturns(myMap);
				cout<< "Finished calculating CAAR" << endl;
				setAverages(); // Populate averageAAR and averageCAAR
				cout<< "Finished calculating Averages" << endl;
				setStdDevs(); // Populate stdDevAAR and stdDevCAAR
				cout<< "Finished calculating Standard Deviation" << endl;
				
			}
			
			void validateStocks(map<string, Stock*>& myMap)
			{
				cout << "For N = " << days/2 << endl;
				int flag = 0;
				int originalSize = myMap.size();
				map<string, Stock*>::iterator itr;
				
				//cout << "\nBefore validation, size of " << myMap.begin()->second->GetGroup() << " = " << myMap.size() << endl;
				cout << "Validating for " << myMap.begin()->second->GetGroup() << endl;
				
				for (auto itr = myMap.begin(); itr != myMap.end(); itr++)
				{
					if (itr->second->Price.size() < days+1) 
					{
						//cout << itr->first << "\t";
						myMap.erase(itr);
						itr->second->warning = "True";
						flag++;
					}else{
						tickers.push_back(itr->first); // Populating vector of tickers
						itr->second->warning = "False";
					}
				}
				if (flag != 0) cout << originalSize - myMap.size() << " stocks deleted as enough price data is not available for given N." << endl;
			}
			
			void setReturns(Stock* st){
				Vector temp = st->Price;
				for (int i = 1; i < temp.size(); i++) st->Returns.push_back((temp[i]/temp[i-1])-1);
				setCumulativeReturns(st);
			}
			
			void setCumulativeReturns(Stock* st){
				Vector temp = st->Price;
				for (int i = 1; i < temp.size(); i++) st->cumulativeReturns.push_back((temp[i]/temp[0])-1);
			}
			
			void setAbnormalReturns(Stock* st, const Index& ind){
				Vector temp_returns = st->Returns;
				auto startItr = find(ind.dates.begin(), ind.dates.end(), st->GetStartDate());
				auto endItr = find(ind.dates.begin(), ind.dates.end(), st->GetEndDate());
				int startIndex = distance(ind.dates.begin(), startItr);
				int endIndex = distance(ind.dates.begin(), endItr);
				for(int i=startIndex-1; i<endIndex-1; i++) st->AbnormalReturns.push_back(st->Returns[i-startIndex] - ind.Returns[i]);
			}
			
			void calcSimTickers(map<string, Stock*>& MyMap){ // returns 40 iteration rows, 80 stock samples
				for(int i=0; i<iterations; i++){
					auto rng = std::default_random_engine {}; // randomizer
					std::shuffle(std::begin(tickers), std::end(tickers), rng); // shuffles the tickers list 
					simTickers.push_back(slicing(tickers, 0, sampleSize-1)); // take the first 80 stocks from the shuffled tickers list
				}
			}
			
			vector<string> slicing(vector<string>& arr, int X, int Y){
			    auto start = arr.begin() + X;
			    auto end = arr.begin() + Y + 1;
			    vector<string> result(Y - X + 1);
			    copy(start, end, result.begin());
			    return result;
			}
			
			void setAverageAbnormalReturns(map<string, Stock*>& MyMap){	
				AAR.resize(iterations, vector<double>(days,0.0)); //40 iteration rows, 120 days columns
				for(int i=0;i<iterations;i++)
					for(double j=0;j<sampleSize;j++)
						AAR[i] = ((j* AAR[i]) + MyMap[simTickers[i][j]]->AbnormalReturns ) / (j+1); //create AAR 40*120
			}
			
			void setCumulativeAbnormalReturns(map<string, Stock*>& MyMap){
				CAAR.resize(iterations, vector<double>(days, 0.0)); //40 iteration rows, 120 days columns
				for(int i=0;i<iterations;i++){
					double sum = 0.0;
					for(int j=0;j<days;j++){
						sum+=AAR[i][j];
						CAAR[i][j] = sum;
					}
				}
			}
			
			void setAverages(){
				averageAAR.resize(days, 0.0);
				averageCAAR.resize(days, 0.0);
				for (int i = 0; i<iterations; i++){
					averageAAR = (i*averageAAR + AAR[i])/(i+1);
					averageCAAR = (i*averageCAAR + CAAR[i])/(i+1);
				}
			}
	
			void setStdDevs(){
				stdDevAAR.resize(days, 0.0);
				stdDevCAAR.resize(days, 0.0);
				for (int i = 0; i<iterations; i++){
					stdDevAAR = (i*stdDevAAR + ((AAR[i]-averageAAR)^(double)2.0))/(i+1);
					stdDevCAAR = (i*stdDevCAAR + ((CAAR[i]-averageCAAR)^(double)2.0))/(i+1);
				}
				stdDevAAR = stdDevAAR^0.5;
				stdDevCAAR = stdDevCAAR^0.5;
			}
			
			void clearAll(){
				AAR.clear();
				CAAR.clear();
				tickers.clear();
				simTickers.clear();
				averageAAR.clear();
				averageCAAR.clear();
				stdDevAAR.clear();
				stdDevCAAR.clear();
			}
			
	};
	
	void displayReturns(calculations& obj){
		cout << "Average AAR" << endl; 
		Display(obj.getAverageAAR());
		cout << "Average CAAR" << endl; 
		Display(obj.getAverageCAAR());
		cout << "Std Dev AAR" << endl; 
		Display(obj.getStdDevAAR());
		cout << "Std Dev CAAR" << endl; 
		Display(obj.getStdDevCAAR());
	}
	vector<Matrix> groupMatrix(calculations& beatCalc, calculations& meetCalc, calculations& missCalc){
		Matrix m1,m2,m3;
		vector<Matrix> m;
		m1.push_back(beatCalc.getAverageAAR());
		m2.push_back(meetCalc.getAverageAAR());
		m3.push_back(missCalc.getAverageAAR());
		m1.push_back(beatCalc.getAverageCAAR());
		m2.push_back(meetCalc.getAverageCAAR());
		m3.push_back(missCalc.getAverageCAAR());
		m1.push_back(beatCalc.getStdDevAAR());
		m2.push_back(meetCalc.getStdDevAAR());
		m3.push_back(missCalc.getStdDevAAR());
		m1.push_back(beatCalc.getStdDevCAAR());
		m2.push_back(meetCalc.getStdDevCAAR());
		m3.push_back(missCalc.getStdDevCAAR());
		m.push_back(m1);
		m.push_back(m2);
		m.push_back(m3);
		return m;
	}
}