#pragma once
#ifndef READDATA_H
#define READDATA_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <windows.h>
#include <map>
#include <io.h>
#include <stdio.h>

using namespace std;

typedef struct __CategoryFile
{ 
	string fileType;
	vector<string> vt_charOK;
	vector<string> vt_charNOK;
}CATEGORYFILE;

class ReadData
{
public:
	ReadData();
	~ReadData();

	int numberCategory;
	string m_sInputPath;
	string inDir;
	string outDir;
	vector<pair<string, string>> m_listFile;
	CATEGORYFILE* categoryFile;
	
	void Initilize();
	void SetIDFile();
	bool dirExists(const string& dirName_in);
	bool OpenFile(string fileName);
	void ReadListFileInFolder(string folderName);
	void WriteDataToFile(string fileName, string type);
	vector<string> ReadFile(string fileName);
	bool CheckConditionToWrite(string line, CATEGORYFILE category);
	void GenerateFile(string fileName, vector<string> funcLine);
	void GetReturnLine(string name, string type);
};

#endif
