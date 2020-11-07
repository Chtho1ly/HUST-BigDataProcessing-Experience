#include <io.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void getFileNames(string folderPath, vector<string>& fileNameV)
//IN:文件路径string，文件名vector，OUT:修改vector为所有子文件路径
{
	_finddata_t file;
	long flag;
	string filename = string(folderPath) + "\\*.*";
	if ((flag = _findfirst(filename.c_str(), &file)) == -1)//目录内找不到文件
	{
		cout << "There is no such type file" << endl;
	}
	else
	{
		//通过前面的_findfirst找到第一个文件
		string name = string(folderPath) + "\\" + file.name;//file.name存放的是遍历得到的文件名
		fileNameV.clear();//清空存储文件路径的vector
		fileNameV.push_back(name);
		//依次寻找以后的文件
		while (_findnext(flag, &file) == 0)
		{
			string name = string(folderPath) + "\\" + file.name;//file.name存放的是遍历得到的文件名
			fileNameV.push_back(name);
		}
		fileNameV.erase(fileNameV.begin(), fileNameV.begin() + 2);
		for (auto i = fileNameV.begin(); i < fileNameV.end(); i++)
		{
			if (i->find("MerkleTree.cfg") != i->npos)
			{
				fileNameV.erase(i);
				break;
			}
		}
	}
	_findclose(flag);
}

string toHex(unsigned int n)
{
	string s;
	unsigned int c;
	unsigned int remain = n;
	for (int i = 0; i < 8; i++)
	{
		c = remain % 16;
		if (0 <= c && c < 10)
			s.insert(s.begin(), c + '0');
		else
			s.insert(s.begin(), c + 'A' - 10);
		remain /= 16;
	}
	return s;
}