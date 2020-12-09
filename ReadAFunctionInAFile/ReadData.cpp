#include "ReadData.h"

ReadData::ReadData()
{
	numberCategory = 3;
	categoryFile = new CATEGORYFILE[numberCategory];
	Initilize();
	
	for (int i = 0; i < m_listFile.size(); i++)
	{
		WriteDataToFile(m_listFile[i].first, m_listFile[i].second);
	}
}

ReadData::~ReadData()
{
}

void ReadData::Initilize()
{
	char tempPath[1024];
	GetPrivateProfileString("Settings", "InputFolder", "", tempPath, sizeof(tempPath), ".\\ReadAFunctionInAFile.ini");
	inDir = tempPath;
	GetPrivateProfileString("Settings", "OutputFolder", "", tempPath, sizeof(tempPath), ".\\ReadAFunctionInAFile.ini");
	outDir = tempPath;

	SetIDFile();
	ReadListFileInFolder(inDir);
}

void ReadData::ReadListFileInFolder(string inputFolder)
{
	string pattern(inputFolder);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			string fileName = data.cFileName;
			int pos = fileName.find(".");
			string name;
			string type;
			name = fileName.substr(0, pos);
			type = fileName.substr(pos+1);
			m_listFile.push_back(make_pair(name, type));
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

void ReadData::SetIDFile()
{	
	string type;
	vector<string> vt_tempOK;
	vector<string> vt_tempNOK;

	//For Cpp file
	type = "cpp";
	vt_tempOK.push_back("::");
	vt_tempOK.push_back("(");
	vt_tempOK.push_back(")");

	vt_tempNOK.push_back(";");
	vt_tempNOK.push_back(".");
	vt_tempNOK.push_back("//");
	vt_tempNOK.push_back("if");
	vt_tempNOK.push_back("while");
	vt_tempNOK.push_back("for");

	categoryFile[0].fileType = type;
	categoryFile[0].vt_charOK = vt_tempOK;
	categoryFile[0].vt_charNOK = vt_tempNOK;

	vt_tempNOK.clear();
	vt_tempOK.clear();

	//For CSharp file
	type = "cs";
	vt_tempOK.push_back("public");
	vt_tempOK.push_back("(");
	vt_tempOK.push_back(")");

	vt_tempNOK.push_back(";");
	vt_tempNOK.push_back(".");
	vt_tempNOK.push_back("//");
	vt_tempNOK.push_back("if");
	vt_tempNOK.push_back("while");
	vt_tempNOK.push_back("for");

	categoryFile[1].fileType = type;
	categoryFile[1].vt_charOK = vt_tempOK;
	categoryFile[1].vt_charNOK = vt_tempNOK;

	vt_tempNOK.clear();
	vt_tempOK.clear();

	type = "cs";
	vt_tempOK.push_back("private");
	vt_tempOK.push_back("(");
	vt_tempOK.push_back(")");

	vt_tempNOK.push_back(";");
	vt_tempNOK.push_back(".");
	vt_tempNOK.push_back("//");
	vt_tempNOK.push_back("if");
	vt_tempNOK.push_back("while");
	vt_tempNOK.push_back("for");

	categoryFile[2].fileType = type;
	categoryFile[2].vt_charOK = vt_tempOK;
	categoryFile[2].vt_charNOK = vt_tempNOK;

	vt_tempNOK.clear();
	vt_tempOK.clear();
}

vector<string> ReadData::ReadFile(string fileName)
{
	fstream inFile;
	string line;
	vector<string> vt_line;

	inFile.open(inDir + "\\" + fileName, ios::in);
	if (!inFile) return vt_line;
	while (getline(inFile, line))
	{
		if (!line.empty())
		{
			vt_line.push_back(line);
		}
	}
	inFile.close();	
	return vt_line;
}

bool ReadData::CheckConditionToWrite(string line, CATEGORYFILE categoryFile)
{
	bool isOK = true;
	vector<string> vt_tempOK;
	vector<string> vt_tempNOK;

	vt_tempOK = categoryFile.vt_charOK;
	vt_tempNOK = categoryFile.vt_charNOK;
	
	for (int i = 0; i < vt_tempOK.size(); i++)
	{
		isOK &= (line.find(vt_tempOK[i]) != string::npos);
	}
	for (int i = 0; i < vt_tempNOK.size(); i++)
	{
		isOK &= (line.find(vt_tempNOK[i]) == string::npos);
	}
	
	return isOK;
}

void ReadData::GetReturnLine(string name, string type)
{
	vector<string> vt_return;
	string fileName = name + "." + type;
	vector<string> vt_line = ReadFile(fileName);
	for (int i = 0; i < vt_line.size(); i++)
	{
		if (vt_line[i].find("return"))
		{
			vt_return.push_back(vt_line[i]);
		}
	}
}

void ReadData::WriteDataToFile(string name, string type)
{
	vector<string> vt_func;
	string fileName = name + "." + type;
	vector<string> vt_line = ReadFile(fileName);

	ReadFile(fileName);
	for (int i = 0; i < numberCategory; i++)
	{
		if (type == categoryFile[i].fileType)
		{
			for (int j = 0; j < vt_line.size(); j++)
			{
				if (CheckConditionToWrite(vt_line[j], categoryFile[i]))
				{
					vt_func.push_back(vt_line[j]);
				}
			}
		}
		if (!dirExists(outDir))
		{
			cout << "Output folder: " << outDir << " is not exist, create new." << endl;
			if (CreateDirectory(outDir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
			{
			}
		}
		GenerateFile(name, vt_func);
	}
}

void ReadData::GenerateFile(string fileName, vector<string> vt_func)
{
	fstream outFile;
	outFile.open(outDir + fileName + ".csv", ios::out);
	outFile << "Type,FunctionName,Parameters,Return" << endl;
	for (int i = 0; i < vt_func.size(); i++)
	{
		vt_func[i].erase(0, vt_func[i].find_first_not_of(" \t\r\n"));

		int begin = vt_func[i].find("(");
		int end = vt_func[i].find(")");

		string temp = vt_func[i].substr(0, begin);
		string param = vt_func[i].substr(begin + 1, end - begin - 1);
		if (param.find(",") != string::npos)
		{
			int pos = param.find(",");
			while (pos != -1)
			{
				param.replace(pos, 1, "\n");
				pos = param.find(",");
			}
		}
		
		int pos = temp.find_last_of(" ");
		string type = temp.substr(0, pos);
		string funcName = temp.substr(pos + 1);
		
		outFile << type << "," << funcName << "," << '"' << param << '"' << endl;
	}
	outFile.close();
}

bool ReadData::dirExists(const string& dirName)
{
	DWORD isExist = GetFileAttributesA(dirName.c_str());
	if (isExist == INVALID_FILE_ATTRIBUTES)
		return false;

	if (isExist & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}