#pragma once
#include<iostream>
#include<algorithm>
#include<stdio.h>
#include<curl/curl.h>
#include<string>
#include<vector>
#include<cstring>
#include<sstream> 
#include<locale>
#include<fstream>
#include<iterator>
#include<cmath>
#include<map>
#include "Matrix.h"
#include "Instrument.h"
#include "DataRetriever.h"

using namespace std;

namespace fre{
    
    bool cmp(pair<string, double>& a, pair<string, double>& b){ // Function to sort the map in descding order of SurpriseRatio
        return a.second > b.second;}
    
    string GetNumericalDate(string date_)
    {
        string day = date_.substr(0,2);
        string month = date_.substr(3,3);
        string year = date_.substr(7,9);
        string newmonth;
        string CalenderMonths[12] = {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
        transform(month.begin(), month.end(), month.begin(), ::toupper);
        for (int i=0; i<12; i++)
        {
            int sc = month.compare(CalenderMonths[i]);
            if (sc == 0)
            {
                if ((i+1)<10) { newmonth = "0" + to_string(i+1); }
                else { newmonth = to_string(i+1); }
            }
        }
        
        string result = "20" + year + "-" + newmonth + "-" + day;
        return result;
    }
    
    class StockMapCreator
    {
        public:
            string filename_ = "Russell3000EarningsAnnouncements.csv";
            vector<string> TickersVec; // Vector to store sorted tickers
            vector<pair<string, double> > SurpriseRatio;
            void CreateStockMap(Index& ind, map<string, Stock*>& MyMap)
            {
                //cout << "CreateStockMap initiated!" << endl;
                TickersVec.clear();
                SurpriseRatio.clear();
                
                ifstream ifile;
                ifile.open(filename_, ios::in);
                if (!ifile.is_open())
                    cout << "Error!! File is not open!" << endl;
                    
                //cout << filename_ << "opened" << endl;
                string line, word;
                vector<string> lineitem;
                
                ifile.ignore(10000, '\n');
                
                while(!ifile.eof())
                {
                    lineitem.clear();
                    getline(ifile, line);
                    
                    if (!line.empty())
                    {
                        stringstream s(line);
                        
                        while(getline(s,word,','))
                        {
                            lineitem.push_back(word);
                        }
                        //cout << "Working on ticker: " << lineitem[0] << endl;
                        string announcement_date = GetNumericalDate(lineitem[1]);
                        //cout << "Modified announcement date: " << announcement_date << endl;
                        Stock* st = new Stock(lineitem[0], announcement_date, lineitem[2],lineitem[3], lineitem[4], lineitem[5], lineitem[6]);
                        MyMap.insert(pair<string, Stock*>(lineitem[0], st));
                        //cout << "Item inserted in MasterMap successfully!" << endl;
                        SurpriseRatio.push_back(make_pair(lineitem[0], strtod(lineitem[6].c_str(), NULL)));
                    }
                }
                ifile.close();
                // Sorting & Grouping
                sort(SurpriseRatio.begin(), SurpriseRatio.end(), cmp);
                for (auto const& element : SurpriseRatio) TickersVec.push_back(element.first);
            }
            
            void splitMasterMap(map<string, Stock*>& MyMap, map<string, Stock*>& BeatMap, map<string, Stock*>& MeetMap, map<string, Stock*>& MissMap)
            {
                int Total = SurpriseRatio.size();
                int CapSize = floor(Total / 3);
                int excess = Total - (CapSize*3);
                int itr = 0;
                
                split(MyMap, itr, CapSize, BeatMap, "Beat", excess);
                split(MyMap, itr, CapSize, MeetMap, "Meet", excess);
                split(MyMap, itr, CapSize, MissMap, "Miss", excess);
                cout << "MasterMap size = " << MyMap.size() << ", BeatMap size = " << BeatMap.size() << ", MeetMap size = " << MeetMap.size()<< ", MissMap size = " << MissMap.size() << endl;
                //cout << "Dropped last " << MyMap.size() - (BeatMap.size() + MeetMap.size() + MissMap.size()) << " stocks while splitting into groups." << endl;
            }
            
            void split(map<string, Stock*>& MyMap, int& itr, int& CapSize, map<string, Stock*>& map, const string& Group, int& excess)
            {
                if (Group == "Meet") CapSize += excess; // If total stocks is not divisible by 3, excess stocks will be in placed in Meet group.
                
                int m_size = 1;
                while(m_size<= CapSize && itr!=TickersVec.size())
                {
                    string CurrTicker = TickersVec[itr];
                    map.insert(pair<string, Stock*>(CurrTicker, MyMap[CurrTicker]));
                    map[CurrTicker]->SetGroup(Group);
                    m_size++, itr++;
                }
            }
    };
}