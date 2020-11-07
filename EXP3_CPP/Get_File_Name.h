#include <io.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void getFileNames(string folderPath, vector<string>& fileNameV)
//IN:�ļ�·��string���ļ���vector��OUT:�޸�vectorΪ�������ļ�·��
{
	_finddata_t file;
	long flag;
	string filename = string(folderPath) + "\\*.*";
	if ((flag = _findfirst(filename.c_str(), &file)) == -1)//Ŀ¼���Ҳ����ļ�
	{
		cout << "There is no such type file" << endl;
	}
	else
	{
		//ͨ��ǰ���_findfirst�ҵ���һ���ļ�
		string name = string(folderPath) + "\\" + file.name;//file.name��ŵ��Ǳ����õ����ļ���
		fileNameV.clear();//��մ洢�ļ�·����vector
		fileNameV.push_back(name);
		//����Ѱ���Ժ���ļ�
		while (_findnext(flag, &file) == 0)
		{
			string name = string(folderPath) + "\\" + file.name;//file.name��ŵ��Ǳ����õ����ļ���
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