//Codes in terms of pre-processing of DWES

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include<vector>
#include<string>
#include <iostream>
#include <fstream>
#include <io.h>
#include <algorithm>
#include <stdio.h>
#include <map>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <time.h>  
#include <stdlib.h>  

using namespace std;
struct Driver
{
	int id;
	vector<int> Time;
	vector<double> lon;
	vector<double> lat;
	vector<vector<int>> TimeSeg;
	vector<vector<double>> lonSeg;
	vector<vector<double>> latSeg;
	int TrajectoryNumber = 0;

};
vector<Driver> DriversInforation;

int TimeDuration = 60; //high sampling rate
int EffectDuration = 600; //
int CountDriver = 0;
int CountTrajectory = 0;


void getFiles(string path, vector<string>& files)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	int a = 0;
	int b = 0;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{ 
			if ((fileinfo.attrib & _A_SUBDIR))
			{

				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

string ParseName(string a)
{
	string b = "";
	string c = "";
	for (int i = a.size(); i > 0; --i)
	{
		if (a[i] == '.')
		{
			break;
		}
		b.push_back(a[i]);
	}
	for (int i = b.size() - 1; i > 0; --i)
	{
		c.push_back(b[i]);
	}
	return c;
}

string FileParseName(string a)
{
	string b = "";
	string c = "";
	for (int i = 0; i <a.size(); ++i)
	{
		if (a[i] >= '0'&& a[i] <= '9')
		{
			c.push_back(a[i]);
		}
	}
	return c;
}

int StringToDatetime(char *timeMain, char *timeSub)
{
	int yearMain, monthMain, dayMain, hourMain, minuteMain, secondMain;
	int yearSub, monthSub, daySub, hourSub, minuteSub, secondSub;
	sscanf(timeMain, "%d-%d-%d %d:%d:%d", &yearMain, &monthMain, &dayMain, &hourMain, &minuteMain, &secondMain);
	sscanf(timeSub, "%d-%d-%d %d:%d:%d", &yearSub, &monthSub, &daySub, &hourSub, &minuteSub, &secondSub);
	int t1 = 24 * 3600 * dayMain + 3600 * hourMain + 60 * minuteMain + secondMain;
	int t2 = 24 * 3600 * daySub + 3600 * hourSub + 60 * minuteSub + secondSub;
	return t2 - t1; 
}


void loadMap(char *fileNewPath)
{
	cout.precision(15);
	vector<string> files;
	getFiles(fileNewPath, files);

	for (int i = 0; i < files.size(); ++i)
	{
		struct Driver driver;
		string FirstTime;
		int countLine = 0;
		driver.id = atoi(FileParseName(files[i]).c_str());
		ifstream fin(files[i]);
		string s;
		
		while (getline(fin, s))
		{
			countLine++;
			char delims[] = ",";
			char *result = NULL;
			char dst[100];
			strcpy(dst, s.c_str());
			result = strtok(dst, delims);
			int column = 0;
			while (result != NULL)
			{
				column++;
				double LatTemp = -1;
				double LonTemp = -1;
				if (column == 2)
				{
				
					if (countLine == 1)
					{
						FirstTime = result;
						driver.Time.push_back(0);
					}
					else
					{
						driver.Time.push_back(StringToDatetime((char*)FirstTime.c_str(), result));
					}
				}
				if (column == 3)
				{
					if (atof(result) >= 116.2703 && atof(result) <= 116.3721)
					{
						driver.lon.push_back(atof(result));
					}
					else
					{
						driver.lon.push_back(0);
					}
				}
				if (column == 4)
				{
					if (atof(result) >= 39.9137 && atof(result) <= 39.9731)
					{
						driver.lat.push_back(atof(result));
					}
					else
					{
						driver.lat.push_back(0);
					}

				}
				result = strtok(NULL, delims);
			}
			if (driver.lon[driver.lon.size() - 1] == 0 || driver.lat[driver.lat.size() - 1] == 0)
			{
				driver.Time.pop_back();
				driver.lat.pop_back();
				driver.lon.pop_back();
			}

		}
		if (driver.Time.size() > 50) //For different targets, set different thresholds
		{
			DriversInforation.push_back(driver);
		}
	
	}
}


vector<vector<int>> SegTime(vector<int> times)
{
	vector<int>BackIndex;
	vector<int>FrontIndex;
	for (int i = 0; i < times.size()-1; ++i)
	{
		if (times[i + 1] - times[i] > TimeDuration)
		{
			BackIndex.push_back(i + 1);
			FrontIndex.push_back(i);
		}
	}

	vector<int>::iterator itor;
	for (itor = FrontIndex.begin(); itor != FrontIndex.end(); itor++)
	{
		vector<int>::iterator result = find(BackIndex.begin(), BackIndex.end(), *itor);
		if (result != BackIndex.end()) //Found 
		{
			itor = FrontIndex.erase(itor);
			itor--;
			result = BackIndex.erase(result);
		}
	}
	
	vector<vector<int>> SegTimes;

	vector<int> STime;
	if (FrontIndex.size() > 0 && BackIndex.size() > 0)
	{

		if (FrontIndex[0] != 0)
		{
			for (int i = 0; i <= FrontIndex[0]; ++i)
			{
				STime.push_back(i);
			}
			SegTimes.push_back(STime);
		}



		for (int i = 0; i < BackIndex.size() - 1; ++i)
		{
			vector<int> SegTime;

			for (int count = BackIndex[i]; count <= FrontIndex[i + 1]; ++count)
			{
				SegTime.push_back(count);
			
			}
			SegTimes.push_back(SegTime);
		}
	}
 	else
	{
		for (int i = 0; i < times.size(); ++i)
		{
			STime.push_back(i);
		}
		SegTimes.push_back(STime);
	}
	
	return SegTimes;
}


void TrajectorySeg()
{
	double TimeDuration;
	for (int i = 0; i < DriversInforation.size(); ++i)
	{

		vector<vector<int>> SegIndex = SegTime(DriversInforation[i].Time);
		for (int k = 0; k < SegIndex.size(); ++k)
		{
			vector<int>Time;
			vector<double> LonTime;
			vector<double> LatTime;
			for (int kk = 0; kk < SegIndex[k].size(); ++kk)
			{
				Time.push_back(DriversInforation[i].Time[SegIndex[k][kk]]);
				LatTime.push_back(DriversInforation[i].lat[SegIndex[k][kk]]);
				LonTime.push_back(DriversInforation[i].lon[SegIndex[k][kk]]);
			}
			if (Time[Time.size() - 1] - Time[0] > EffectDuration)  //20Min
			{
				DriversInforation[i].TimeSeg.push_back(Time);
				DriversInforation[i].latSeg.push_back(LatTime);
				DriversInforation[i].lonSeg.push_back(LonTime);
			}
		}
		DriversInforation[i].TrajectoryNumber = DriversInforation[i].TimeSeg.size();
	}
}


void WriteDriver(char *path)
{
	ofstream fout;
	fout.precision(15);
	fout.open(path);

	for(int i = 0; i < DriversInforation.size(); ++i)
	{
		for (int j = 0; j < DriversInforation[i].TimeSeg.size(); ++j)
		{
			for (int k = 0; k < DriversInforation[i].TimeSeg[j].size(); ++k)
			{
				fout << DriversInforation[i].id << " " << DriversInforation[i].TimeSeg[j][k] << " " << DriversInforation[i].lonSeg[j][k] << " " << DriversInforation[i].latSeg[j][k]<<endl;
			}
		}
	}

	fout.close();
}

void WriteDriverN(char *path)
{
	ofstream fout;
	fout.precision(15);
	fout.open(path);
	fout << "Driver Number  " << CountDriver << endl;
	fout << "Trajectory Number " << CountTrajectory << endl;
	fout.close();
}

void WriteDriverNDetails(char *path)
{
	ofstream fout;
	fout.precision(15);
	fout.open(path);

	for (int i = 0; i < DriversInforation.size(); ++i)
	{
		for (int j = 0; j < DriversInforation[i].TimeSeg.size(); ++j)
		{	
			fout << DriversInforation[i].TimeSeg[j].size() << endl;
		}
	}
	fout.close();
}


void CalculateDriver()
{
	for (int i = 0; i < DriversInforation.size(); ++i)
	{
		if (DriversInforation[i].TimeSeg.size() > 0)
		{
			CountDriver++;
			CountTrajectory += DriversInforation[i].TimeSeg.size();
		}
	}
}


int main()
{

	//string address = "E:\\RealFutureWork\\T_driver\\drivers";
	//string Waddress = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\Trajectory.txt";
	//string WaddressN = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\TrajectoryAnalysis.txt";
	//string WaddressNDetails = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\TrajectoryDetails.txt";
	//string ShowPoints = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\TrajectoryPoints.txt";
	//string MapPath = "E:\\Map-Matching_Beijing\\Beijing";
	//string GPSaddress = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\Trajectory.txt";
	//string GPSaddressDetail = "E:\\RealFutureWork\\T_driver\\driverTrajectoryResult\\TrajectoryDetails.txt";

	char *filePath = const_cast<char*>(address.c_str());
	char *WfilePath = const_cast<char*>(Waddress.c_str());
	char *WfilePathN = const_cast<char*>(WaddressN.c_str());
	char *WfilePathDetail = const_cast<char*>(WaddressNDetails.c_str());
	char *showPoints = const_cast<char*>(ShowPoints.c_str());
	char *BeijingPath = const_cast<char*>(MapPath.c_str());
	char *GPSaddressNew = const_cast<char*>(GPSaddress.c_str());
	char *GPSaddressDetailNew = const_cast<char*>(GPSaddressDetail.c_str());

	loadMap(filePath);
	TrajectorySeg();
	CalculateDriver();
	WriteDriver(WfilePath);
	WriteDriverN(WfilePathN);
	WriteDriverNDetails(WfilePathDetail);
	return 0;
}