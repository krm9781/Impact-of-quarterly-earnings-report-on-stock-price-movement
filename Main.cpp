#include<iostream>
#include<curl/curl.h>
#include<string>
#include<vector>
#include<iterator>
#include<cmath>
#include<chrono>
#include <limits>
#include "DataRetriever.h"
#include "Instrument.h"
#include "StockMap.h"
#include "Calculations.h"
#include "Matrix.h"
#include "Plotting.h"

using namespace std;
using namespace fre;
using namespace std::chrono;
    
int main()
{
    Index Russell;
    cout << "Index object created successfully!" << endl;
    
    DataRetriever dr;
    cout << "DataRetriever Object created successfully!" << endl;
    
    StockMapCreator smc;
    cout << "StockMap Object created successfully!" << endl;
    
    string sd = "2022-01-01";
    string ed = "2022-12-15";
    string t = "IWV";
    map<string, Stock*> MasterMap;
    map<string, Stock*> BeatMap;
    map<string, Stock*> MeetMap;
    map<string, Stock*> MissMap;
    vector<Matrix> GroupMatrix;
    
    calculations BeatCalc;
    calculations MeetCalc;
    calculations MissCalc;
    cout << "Calculation Objects created successfully!" << endl;

    int N=0,tempN=0;
    int input = -1,tinput;
    string s;
    string ticker;
    string group;
    
    while (input != 5)
    {
        cout << "Menu" << endl;
        cout << "Enter 1 to Retrieve price data for all stocks " << endl;
        cout << "Enter 2 to Pull Individual Stock Data" << endl;
        cout << "Enter 3 to Show Group Data" << endl;
        cout << "Enter 4 to Plot Returns" << endl;
        cout << "Enter 5 to Exit: " << endl;
        cin >> input;
        while(cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            cout << "Invalid Input, please enter a number between 1-5" << endl;
            cin >> input;
        }
        cout<<endl;
		// only do things of n !=0
		switch(input)
		{
		    case 1:
                cout << "Enter the number of days between 60 to 90 for which the stock data needs to be populated" << endl;
                cin >> tempN;
                while(cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                    cout << "Invalid Input, please enter a number between 60 and 90" << endl;
                    cin >> tempN;
                }
                if(tempN < 60 || tempN >90){
                    cout << "Number of days not in the range of 60 to 90. Please enter a correct value for N" <<endl;
                    cout<<endl;
                    break;
                }else if(tempN==N){
                    cout<<"Data already populated, proceed to step 2"<<endl;
                    cout<<endl;
                    break;
                }else{
                    const std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
                    
                    MasterMap.clear();
                    BeatMap.clear();
                    MeetMap.clear();
                    MissMap.clear();
                    N = tempN;
                    dr.SingleTickerData(Russell,t,sd,ed);
                    smc.CreateStockMap(Russell, MasterMap);
                    dr.MultipleTickerData(MasterMap,Russell,N);
                    smc.splitMasterMap(MasterMap, BeatMap, MeetMap, MissMap);
                    
                    BeatCalc.setDays(N);
                    BeatCalc.calcs(Russell, BeatMap);
                    cout << "Performed Bootstrap for BeatMap" << endl;
                    
                    MeetCalc.setDays(N);
                    MeetCalc.calcs(Russell, MeetMap);
                    cout << "Performed Bootstrap for MeetMap" << endl;
                    
                    MissCalc.setDays(N);
                    MissCalc.calcs(Russell, MissMap);
                    cout << "Performed Bootstrap for MissMap" << endl;
                    cout<<endl;
                    
                    GroupMatrix = groupMatrix(BeatCalc, MeetCalc, MissCalc);
                    cout<<" 3D Matrix of avgAAR, avgCAAR, stdDevAAR, stdDevCAAR for the 3 groups created"<<endl;
                    //Display(GroupMatrix[0]);
                    //Display(GroupMatrix[1]);
                    //Display(GroupMatrix[2]);
                    
                    
                    const std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
                    std::cout
                      << "The process took "
                      << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "µs ≈ "
                      << (end - start) / 1ms << "ms ≈ " // almost equivalent form of the above, but
                      << (end - start) / 1s << "s ≈ " // using milliseconds and seconds accordingly
                      << (end - start) / 1s / 60 << "m " << ((end - start) / 1s) % 60 << "s.\n" << endl; // using minutes and seconds.
                    
                    cout<<endl;
                    break;
                }
            
            case 2:
                if(N!=0){
                    cout<<"Enter the ticker name:"<<endl;
                    cin >> ticker;
                    transform(ticker.begin(), ticker.end(), ticker.begin(), ::toupper);
                    if(MasterMap.find(ticker) != MasterMap.end())
                    {
                        cout<<"Ticker found"<<endl;
                        cout<<"Ticker: "<< ticker << endl;
                        cout<<"Earning Announcement Date: "<< MasterMap[ticker]->GetAnnouncementDate() << endl;
                        cout<<"Period Ending: "<< MasterMap[ticker]->GetPeriodEnding()<<endl;
                        cout<<"Estimated EPS: "<< MasterMap[ticker]->GetEstimatedEPS()<<endl;
                        cout<<"Reported EPS: "<< MasterMap[ticker]->GetReportedEPS()<<endl;
                        cout<<"Surprise Ratio Pct: "<< MasterMap[ticker]->GetSurprisePct() << endl;
                        cout<<"Surprise Ratio Abs: "<< MasterMap[ticker]->GetSurpriseAbs() << endl;
                        cout<<"Group: "<< MasterMap[ticker]->GetGroup() << endl;
                        cout<<"Not enough data warning: "<< MasterMap[ticker]->GetWarning() << endl;
                        cout<<"Daily Price Dates: "<<endl;Display(MasterMap[ticker]->dates);
                        cout<<"Daily Prices: "<<MasterMap[ticker]->Price<<endl;
                        cout<<"Daily Returns: "<<endl<<MasterMap[ticker]->Returns<<endl;
                        cout<<"Daily Cumulative Returns: "<<MasterMap[ticker]->cumulativeReturns<<endl;
                        cout<<"Daily Abnormal Returns: "<<endl<<MasterMap[ticker]->AbnormalReturns<<endl;
                    }else{
                        cout<<"Ticker not found, enter a valid ticker"<<endl;
                    }
                }else{
                    cout<<"Please run option 1 to access this feature"<<endl;
                }
                cout<<endl;
                break;
            
            case 3:
                if(N!=0){
                    cout<<"Enter Group Name (Beat, Miss or Meet): "<<endl;
                    cin>>group;
                    transform(group.begin(), group.end(), group.begin(), ::tolower);
                    if(group=="beat") displayReturns(BeatCalc);
                    else if(group=="miss") displayReturns(MissCalc);   
                    else if(group=="meet") displayReturns(MeetCalc);
                    else cout<<"No such group, enter a correct group name"<<endl;
                }else{
                    cout<<"Please run option 1 to access this feature"<<endl;
                }
                cout<<endl;
                break;
                
            case 4:
                {
                    if(N!=0)
                    {
                        vector<double> AvgCaarBeat = BeatCalc.getAverageCAAR();
                        vector<double> AvgCaarMeet = MeetCalc.getAverageCAAR();
                        vector<double> AvgCaarMiss = MissCalc.getAverageCAAR();
                        plotResults(AvgCaarBeat , AvgCaarMeet,AvgCaarMiss,N);
                    }
                    else
                    {
                        cout<<"Please run option 1 to access this feature"<<endl;
                    }
                } 
                cout<<endl;
                break;
                
            case 5:
                break;
                
            default:
                cout<< "Invalid Input, please enter a number between 1-5"<<endl;
                cout<<endl;
                break;
		}
    }
    
    map<string,Stock*>::iterator itr;
    for (itr = MasterMap.begin(); itr!=MasterMap.end(); itr++){
        delete itr->second;
        itr->second = NULL;
    }
    return 0;
}