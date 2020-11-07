#include <io.h>//??
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void getAllFileNames(string folderPath, vector<string>& fileNameV)
//IN:?·string?vectorOUT:?vector??·
{
	_finddata_t file;
	long flag;
	string filename = string(folderPath) + "\\*.*";
	if ((flag = _findfirst(filename.c_str(), &file)) == -1)//?¼??
	{
		cout << "There is no such type file" << endl;
	}
	else
	{
		//??_findfirst???
		string name = string(folderPath) + "\\" + file.name;//file.name??õ?
		fileNameV.push_back(name);
		//???
		while (_findnext(flag, &file) == 0)
		{
			string name = string(folderPath) + "\\" + file.name;//file.name??õ?
			fileNameV.push_back(name);
		}
		fileNameV.erase(fileNameV.begin(), fileNameV.begin() + 2);
	}
	_findclose(flag);
}