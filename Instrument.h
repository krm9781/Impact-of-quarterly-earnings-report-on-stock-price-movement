#pragma once
#include<stdio.h>
#include<algorithm>
#include<iostream>
#include<curl/curl.h>
#include<string>
#include<vector>
#include<cstring>
#include<sstream> 
#include<locale>
#include<fstream>
#include<iterator>
#include<cmath>
#include<ctime>
#include<map>
#include "Matrix.h"
//#include "DataRetriever.h"

using namespace std;

namespace fre
{
    
    class Instrument
    {
        protected:
            string ticker;
            string StartDate;
            string EndDate;
            //vector<double> Price;
            //vector<double> Returns;
            
        public:
            vector<double> Price;
            vector<double> Returns;
            vector<string> dates; // For Index Class, Master list of dates for which prices of IVW are retrieved (biglistdatesIVW), For Stock Class range of 2n+1 dates for each ticker
            Vector cumulativeReturns;
            string warning;
            string GetStartDate() { return StartDate; }
            string GetEndDate() { return EndDate; }
            string GetTicker() { return ticker; }
            string GetWarning() { return warning; }
            

            void SetPrices(const vector<double>& prices)
            {
                Price.clear();
                Price.resize(prices.size());
                for (int i=0; i < Price.size(); i++)
                {
                    Price[i] = prices[i];
                }
            }
            void PrintPrice()
            {
                for (int i=0; i<Price.size(); i++)
                {
                    cout << Price[i] << endl;
                }
            }
    };
    
    class Index : public Instrument
    {
        private:
            string ticker = "IWV";
            vector<string> IndexConstituents;   // 2585 Tickers
        
        public:
            // vector<string> dates; // Master list of dates for which prices of IVW are retrieved (biglistdatesIVW)
            
            void SetDates(vector<string> dates1)
            {
                dates.resize(dates1.size());
                for (size_t i=0; i<dates.size(); i++)
                {
                    dates[i] = dates1[i];
                }
            }
            
            void SetStartDate() { StartDate = dates[0]; }
            void SetEndDate() { EndDate = dates[dates.size()-1]; }
            
            void RetrieveTickers()
            { 
                const char* SourceFile = "Russell_3000_component_stocks.csv";
                
                ifstream fin;
                fin.open(SourceFile, ios::in);
                if (!fin.is_open())
                    cout << "Error! File not opened!" << endl;
                string line, name, symbol;
                while(!fin.eof())
                {
                    getline(fin, line);
                    stringstream sin(line);
                    getline(sin, symbol, ',');
                    getline(sin, name);
                    IndexConstituents.push_back(symbol);
                }
            }
            
            string GetTicker() { return ticker; }
            vector<string> GetIndexConstituents() { return IndexConstituents; }
    };
    
    class Stock : public Instrument
    {
        private:
            string AnnouncementDate;
            string PeriodEnding;
            string EstimatedEPS;
            string ReportedEPS;
            string SurpriseAbs;
            string SurprisePct;
            string Group = "Master";
            
        public:
            vector<double> AbnormalReturns;
            vector<double> CumulativeAbnormalReturns;
            // Constructor with parameters
            Stock(string ticker_, string AnnouncementDate_,string PeriodEnding_ ,string EstimatedEPS_, string ReportedEPS_, string SurpriseAbs_, string SurprisePct_)
            {
                ticker = ticker_;
                AnnouncementDate = AnnouncementDate_;
                PeriodEnding = PeriodEnding_;
                EstimatedEPS = EstimatedEPS_;
                ReportedEPS = ReportedEPS_;
                SurpriseAbs = SurpriseAbs_;
                SurprisePct = SurprisePct_;
            }
            
            // Function Overloading
            void SetDates(vector<string>& referencedates, int N_)
            {
                auto itr = find(referencedates.begin(), referencedates.end(), AnnouncementDate);
                int index = itr - referencedates.begin();
                int StartDateIndex , EndDateIndex;
                //int index = std::distance(referencedates.begin(), itr);
                if ((index - N_) < 0)
                { 
                    StartDate = referencedates[0]; 
                    StartDateIndex = 0;
                }
                else 
                { 
                    StartDate = referencedates[index - N_]; 
                    StartDateIndex = index - N_;
                }
                if ((index + N_) >= referencedates.size()) 
                {
                    EndDate = referencedates.back();
                    EndDateIndex = referencedates.size()-1;
                }
                else 
                { 
                    EndDate = referencedates[index + N_]; 
                    EndDateIndex = index + N_;
                }
                for(int i = StartDateIndex ; i<=EndDateIndex ;i++)
                {
                    dates.push_back(referencedates[i]);
                }
                
            }
            
            
            string GetAnnouncementDate() { return AnnouncementDate; }
            string GetPeriodEnding() {return PeriodEnding; }
            string GetEstimatedEPS() { return EstimatedEPS; }
            string GetReportedEPS() { return ReportedEPS; }
            string GetSurpriseAbs() { return SurpriseAbs; }
            string GetSurprisePct() { return SurprisePct; }
            void SetGroup(string Group_) { Group = Group_; }
            string GetGroup(){return Group;}
            
    };
    
}