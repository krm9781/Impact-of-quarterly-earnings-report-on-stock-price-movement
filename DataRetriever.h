#pragma once
#include<iostream>
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
#include "Matrix.h"
#include "Instrument.h"
#include "progressbar.hpp"

using namespace std;

namespace fre
{
    struct MemoryStruct {
		char* memory;
		size_t size;
		//size_t total_size;
	};

	void* myrealloc(void* ptr, size_t size)
	{
		if (ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	}
	
	int write_data(void* ptr, int size, int nmemb, FILE* stream)
	{
		size_t written;
		written = fwrite(ptr, size, nmemb, stream);
		return written;
	}

	static size_t write_data2(void* ptr, size_t size, size_t nmemb, void* data)
	{
		size_t realsize = size * nmemb;
		struct MemoryStruct* mem = (struct MemoryStruct*)data;
		mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory) {
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
		}
		return realsize;
	}
    
    class DataRetriever
    {
        public:
            
            void SingleTickerData(Index& ind, string ticker, string startDate, string endDate)
            {
                vector<string> sDates;
            	vector<double> sPrices;
                CURL* handle;
                CURLcode result;
                
                curl_global_init(CURL_GLOBAL_ALL);

        		handle = curl_easy_init();
        
        		// if everything's all right with the easy handle...
        		if (handle)
        		{
        			string url_common = "https://eodhistoricaldata.com/api/eod/";
        			string api_token = "6369b1aaa3d005.79760191";  
        
        			struct MemoryStruct data;
        			data.memory = NULL;
        			data.size = 0;
        
        			//string SYMBOL = "IWV";
        			string SYMBOL = ticker;
        
        			if (SYMBOL.back() == '\r') {
        				SYMBOL.pop_back();
        			}
        
        			string url_request = url_common + SYMBOL + ".US?" + "from=" + startDate + "&to=" + endDate + "&api_token=" + api_token + "&period=d";
        			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
        
        			//adding a user agent
        			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        
        			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
        
        			result = curl_easy_perform(handle);
        
        			// check for errors
        			if (result != CURLE_OK) {
        				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        				//return -1; Need to STOP EXECUTION HERE;
        				return;
        			}
        
        			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
        
        			stringstream sData;
        			sData.str(data.memory);
        			string sValue, sDate;
        			double dValue = 0;
        			string line;
        			while (getline(sData, line))
        			{
        				size_t found = line.find('-');
        				if (found != std::string::npos)
        				{
        					sDate = line.substr(0, line.find_first_of(','));
        					line.erase(line.find_last_of(','));
        					sValue = line.substr(line.find_last_of(',') + 1);
        					dValue = strtod(sValue.c_str(), NULL);
        					//cout << "Price on " << sDate << " = " << dValue << endl;
        					sPrices.push_back(dValue);
        					sDates.push_back(sDate);
        					//ind.Price.push_back(dValue);
        					//ind.dates.push_back(sDate);
        				}
        			}
        			ind.SetDates(sDates);
        			ind.SetStartDate(); 
        			ind.SetEndDate(); 
        			ind.SetPrices(sPrices);
        			free(data.memory);
        			data.size = 0;
        		}
        		else
        		{
        			fprintf(stderr, "Curl init failed!\n");
        			return;
        		}
        		// cleanup since you've used curl_easy_init
        		curl_easy_cleanup(handle);
        		// release resources acquired by curl_global_init()
        		curl_global_cleanup();
        		
        		//Calculate Index Returns & Cumulative Returns;
        		for (int i = 1; i <= ind.Price.size(); i++) 
				{
					if (i==ind.Price.size()) {;}
					else ind.Returns.push_back((ind.Price[i] / ind.Price[i-1]) - 1);
					if (i==1) {;}
					else ind.cumulativeReturns.push_back(ind.Returns[i-1] * ind.Returns[i]);
				}
            }
            
            void MultipleTickerData(map<string,Stock*>& MyMap, Index& ind, int N_)
            {
            	// declaration of an object CURL 
				CURL* handle;
			
				CURLcode result;
			
				// set up the program environment that libcurl needs 
				curl_global_init(CURL_GLOBAL_ALL);
			
				// curl_easy_init() returns a CURL easy handle 
				handle = curl_easy_init();
			
				// if everything's all right with the easy handle... 
				if (handle)
				{
					string url_common = "https://eodhistoricaldata.com/api/eod/";
					string api_token = "6369b1aaa3d005.79760191";
					
					progressbar bar(MyMap.size()); // Code for Progress Bar
						
					map<string,Stock*>::iterator itr;
					for (itr=MyMap.begin(); itr != MyMap.end(); itr++)
					{
						bar.update(); // Code for Progress Bar
					
						//vector<double> MTDPrices;
						itr->second->SetDates(ind.dates,N_);
						struct MemoryStruct data;
						data.memory = NULL;
						data.size = 0;
			
						string symbol = itr->first;
						string start_date = itr->second->GetStartDate();
						string end_date = itr->second->GetEndDate();
						string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
						curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
			
						//adding a user agent
						curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
						curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
						curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			
						curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
						curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
			
						// perform, then store the expected code in result
						result = curl_easy_perform(handle);
			
						if (result != CURLE_OK)
						{
							// if errors have occured, tell us what is wrong with result
							fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
							//return 1;
						}
						
						stringstream sData;
						sData.str(data.memory);
						string sValue, sDate;
						double dValue = 0;
						string line;
						//cout << symbol << endl;
						while (getline(sData, line)) {
							size_t found = line.find('-');
							if (found != std::string::npos)
							{
								//cout << line << endl;
								sDate = line.substr(0, line.find_first_of(','));
								line.erase(line.find_last_of(','));
								sValue = line.substr(line.find_last_of(',') + 1);
								dValue = strtod(sValue.c_str(), NULL);
								itr->second->Price.push_back(dValue);
								//MTDPrices.push_back(dValue);
								//cout << sDate << " " << std::fixed << ::setprecision(2) << dValue << endl;
							}
						}
						
						//itr->second->SetPrices(MTDPrices);
						
						free(data.memory);
						data.size = 0;
					}
			
				}
				else
				{
					fprintf(stderr, "Curl init failed!\n");
					//return -1;
				}
			
				// cleanup since you've used curl_easy_init  
				curl_easy_cleanup(handle);
			
				// release resources acquired by curl_global_init() 
				curl_global_cleanup();
				cout << endl; // Code for Progress Bar better display
            }
            
    };
}