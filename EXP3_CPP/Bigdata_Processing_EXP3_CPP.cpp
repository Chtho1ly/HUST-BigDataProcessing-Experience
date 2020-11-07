#include <io.h>//所需头文件
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include "Get_File_Name.h"
#include "Merkle_Tree.h"
#include "SHA-256.h"
#define FOLDER_PATH1 "folder1"
#define FOLDER_PATH2 "folder2"

typedef unsigned int UInt32;
typedef unsigned __int64 UInt64;
typedef unsigned char UChar;
int MaxSize;
int Ylen;
SHA256 sha256 = SHA256();
Message_Digest M_D;
vector<UInt32> W;//整型
UInt32 M[16];   //存分组信息

using namespace std;

//添加填充位+添加长度
string PAD(vector<UChar> Y) {
	//x+1+d+l=|x|
	UInt32 i, j;
	UInt32 T1 = 0, T2 = 0, T3 = 0, T4 = 0;
	vector<UChar> temp(MaxSize);
	UInt64 x = Ylen;//数据长度
	UInt32 d = (55 - x) % 64;   //填充长度
	UInt32 n = (x + 8) / 64 + 1; //分组数
	UInt32 m = x % 64;       //最后组数据长度
	UInt32 l = 8;
	/*
	cout << "数据长度x:" << int(x) << " ";
	cout << "填充长度d:" << d << " ";
	cout << "分组数量n:" << n << " ";
	cout << "最后长度m:" << m << endl;
	*/
	//不填充
	for (i = 0; i < x; i++) {
		temp[i] = Y[i];
	}
	//填充1次1000 0000
	temp[x] = 0x80;
	//填充d次0000 0000
	for (i = x + 1; i < x + d + 1; i++) {
		temp[i] = 0x00;
	}
	//填充长度的63-0位
	for (i = 1; i <= l; i++) {
		temp[(n * 64) - i] = (UChar)(8 * x >> (i - 1) * 8);
	}
	//无符号字符转换为无符号整型
	for (i = 0; i < MaxSize / 4; i++) {
		for (j = 0; j < 4; j++) {
			if (j == 0)
				T1 = temp[4 * i + j];
			if (j == 1)
				T2 = temp[4 * i + j];
			if (j == 2)
				T3 = temp[4 * i + j];
			if (j == 3)
				T4 = temp[4 * i + j];
		}
		W[i] = (T1 << 24) + (T2 << 16) + (T3 << 8) + T4;
	}
	//显示16进制数据
	/*
	cout << "16进制数据：";
	for (i = 0; i < n * 16; i++) {
		cout << hex << " " << W[i];
	}
	cout << endl;
	*/
	//分组处理
	for (i = 0; i < n; i++) {
		//cout << "分组处理:" << i + 1 << endl;
		for (j = 0; j < 16; j++) {
			M[j] = W[(i * 16) + j];
		}
		M_D = sha256.DEAL(M);
	}
	string HASH;
	for (i = 0; i < 8; i++) {
		HASH += toHex(M_D.H[i]);
	}
	return HASH;
}

//IN:文件路径；OUT:string形式的HASH值
string getHASH(string filePath)
{
	vector<UChar> Y;
	ifstream inFile;
	inFile.open(filePath, ios_base::binary);
	if (!inFile.is_open())
	{
		cout << "Error";
		exit(1);
	}
	char c[1];
	while (inFile.read(c, 1))
	{
		if (inFile.eof())
			break;
		Y.push_back(c[0]);
	}
	Ylen = Y.size();
	MaxSize = (Y.size() / 64 + 2) * 64;
	while (Y.size() < MaxSize)
	{
		Y.push_back('/0');
	}
	while (W.size() < MaxSize / 4)
	{
		W.push_back('/0');
	}
	string HASHstr = PAD(Y);
	W.clear();
	sha256 = SHA256();
	//system("pause");
	return HASHstr;
}

//IN:左侧HASH值，右侧HASH值；OUT:string形式的HASH值
string getHASH(string leftHash, string rightHash)
{
	vector<UChar> Y;
	//左右HASH首尾相连后求HASH

	for (char c : leftHash)
	{
		Y.push_back(c);
	}
	for (char c : rightHash)
	{
		Y.push_back(c);
	}

	//左右HASH做异或后求HASH
	/*
	for (int i = 0; i < leftHash.length(); i++)
	{
		UChar a = leftHash[i];
		UChar b = leftHash[i];
		UChar c;
		a = ('0' <= a && a <= '9') ? a - '0' : a - 'A';
		b = ('0' <= b && b <= '9') ? b - '0' : b - 'A' + 10;
		c = a ^ b;
		c %= 16;
		c = (0 <= c && c <= 9) ? c + '0' : b + 'A' - 10;
		Y.push_back(c);
	}
	*/
	Ylen = Y.size();
	MaxSize = (Y.size() / 64 + 1) * 64;
	while (Y.size() < MaxSize)
	{
		Y.push_back('/0');
	}
	while (W.size() < MaxSize / 4)
	{
		W.push_back('/0');
	}
	string HASHstr = PAD(Y);
	W.clear();
	sha256 = SHA256();
	//system("pause");
	return HASHstr;
}

//IN:新节点，向merkleTree中插入node
vector<Node> MerkleTree::insert(Node newNode)
{
	vector<Node> influencedNodeV;
	auto newNodeP = new Node(newNode.key, newNode.hashval);
	//初始化为叶子，子树数目为1
	newNodeP->isleaf = true;
	newNodeP->left = NULL;
	newNodeP->right = NULL;
	newNodeP->parent = NULL;
	newNodeP->subTreeSize = 1;
	//若树此前为空
	if (size == 0)
	{
		size = 1;
		newNodeP->nth = "1";
		root = newNodeP;
		influencedNodeV.push_back(*newNodeP);
	}
	else
	{
		Node* newParentP;
		for (auto i = root;;)
		{
			//如果i没有子树，则将新节点插入到i下
			if (i->left == NULL)
			{
				newParentP = i;
				break;
			}
			//如果i有子树，则选择其中规模较小的作为新的i
			else
			{
				if (i->left->subTreeSize <= i->right->subTreeSize)
					i = i->left;
				else
					i = i->right;
			}
		}
		//源parent作为新节点的兄弟节点
		auto newBroP = new node(*newParentP);
		//规定父子关系
		newParentP->left = newBroP; // Father
		newParentP->right = newNodeP;
		newBroP->parent = newParentP;
		newNodeP->parent = newParentP;
		newBroP->nth = newParentP->nth + '0'; // Slibing
		newNodeP->nth = newParentP->nth + '1';
		influencedNodeV.push_back(*newNodeP);
		influencedNodeV.push_back(*newBroP);
		//更新所有受影响结点的hash、key、subTreeSize
		for (auto i = newNodeP; i->parent != NULL; i = i->parent)
		{
			auto tempParentP = i->parent;
			tempParentP->isleaf = false;
			tempParentP->hashval = getHASH(tempParentP->left->hashval, tempParentP->right->hashval);
			tempParentP->key = tempParentP->left->key + string("|") + tempParentP->right->key;//父节点的key为两子节点key相连
			tempParentP->subTreeSize += 1;
			influencedNodeV.push_back(*tempParentP);
		}
		size += 2;
	}
	return influencedNodeV;
}

//从merkleTree中删除node,以key为匹配条件
vector<Node*> MerkleTree::delet(Node toDelNode)
{
	Node* toDelP;
	Node* broP;
	Node* parentP;
	string toDelKey;
	vector<Node*> influencedNodeV;
	toDelP = search(toDelNode.key);
	toDelKey = toDelP->key;
	parentP = toDelP->parent;
	if (parentP->left->hashval == toDelP->hashval)
	{
		toDelKey.insert(toDelKey.end(), '|');
		broP = parentP->right;
	}
	else
	{
		toDelKey.insert(toDelKey.begin(), '|');
		broP = parentP->left;
	}
	parentP->hashval = broP->hashval;
	parentP->key = broP->key;
	parentP->subTreeSize = 1;
	parentP->left = NULL;
	parentP->right = NULL;
	size -= 2;
	free(toDelP);
	free(broP);
	influencedNodeV.push_back(parentP);
	for (auto i = parentP->parent; i != NULL; i = i->parent)
	{
		i->hashval = getHASH(i->left->hashval, i->right->hashval);
		i->key.erase(i->key.find(toDelKey, 0), toDelKey.size());
		influencedNodeV.push_back(i);
	}
	return influencedNodeV;
}

//IN:储存文件名的vector<Node>，读取文件夹下的所有文件
int MerkleTree::readFolder(vector<Node>& nodeV)
{
	nodeV.clear();
	vector<string> filePathV;
	vector<string> hashV;
	getFileNames(folderPath, filePathV);
	for (auto i : filePathV)
	{
		nodeV.push_back(node(i, getHASH(i)));
		nodeV.back().key.erase(nodeV.back().key.begin(), nodeV.back().key.begin() + folderPath.size() + 1);
	}
	return 0;
}

//由节点vector生成merkleTree
int MerkleTree::generate()
{
	root = NULL;
	size = 0;
	vector<Node> nodeV;
	readFolder(nodeV);
	//sort(nodeV.begin(), nodeV.end(), [](Node a, Node b) {return a.hashval < b.hashval; });
	for (auto i : nodeV)
		insert(i);
	return 0;
}

//将merkleTree存储到文件中
int MerkleTree::save()
{
	vector<Node*> waiting;
	vector<Node*> topo;
	waiting.push_back(root);
	while (!waiting.empty())
	{
		Node* nodeP = waiting.front();
		if (nodeP->left != NULL)
		{
			waiting.push_back(nodeP->left);
			waiting.push_back(nodeP->right);
		}
		topo.push_back(nodeP);
		waiting.erase(waiting.begin());
	}
	ofstream config;
	config.open(folderPath + "\\MerkleTree.cfg");
	if (!config.is_open())
	{
		cout << "save: Fail opening MerkleTree.cfg" << endl;
		return 1;
	}
	else
	{
		config << "treeSize " << "*" << size << "*" << endl;
		for (auto i : topo)
		{
			config << "nth " << "*" << i->nth << "*" << endl;
			config << "key " << "*" << i->key << "*" << endl;
			config << "hashval " << "*" << i->hashval << "*" << endl;
			config << "subTreeSize " << "*" << i->subTreeSize << "*" << endl;
		}
	}
	config.close();
	if (config.is_open())
	{
		cout << "save: Fail closing MerkleTree.cfg" << endl;
		return 1;
	}
	else
		return 0;//成功
}

//从文件中读取存储的merkleTree
int MerkleTree::load()
{
	ifstream config;
	config.open(folderPath + "\\MerkleTree.cfg");
	if (!config.is_open())
	{
		cout << "load: Fail opening MerkleTree.cfg" << endl;
		return 1;
	}
	else
	{
		string buffer;
		getline(config, buffer);
		size = stoi(getData(buffer));
		for (int i = 0; i < size; i++)
		{
			Node* newNodeP = new Node("", "");
			getline(config, buffer);
			newNodeP->nth = getData(buffer);
			getline(config, buffer);
			newNodeP->key = getData(buffer);
			getline(config, buffer);
			newNodeP->hashval = getData(buffer);
			getline(config, buffer);
			newNodeP->subTreeSize = stoi(getData(buffer));

			newNodeP->parent = NULL;
			newNodeP->left = NULL;
			newNodeP->right = NULL;
			if (i == 0)
			{
				root = newNodeP;
			}
			else
			{
				Node* newParentP = root;
				for (auto j = newNodeP->nth.begin() + 1; j < newNodeP->nth.end(); j++)
				{
					if (*j == '0')
					{
						if (j != newNodeP->nth.end() - 1)
							newParentP = newParentP->left;
						else
						{
							newNodeP->parent = newParentP;
							newParentP->left = newNodeP;
						}
					}
					else if (*j == '1')
					{
						if (j != newNodeP->nth.end() - 1)
							newParentP = newParentP->right;
						else
						{
							newNodeP->parent = newParentP;
							newParentP->right = newNodeP;
						}
					}
				}
			}
		}
	}

	return 0;
}

//输出所有节点
int MerkleTree::output()
{
	vector<Node*> waiting;
	vector<Node*> topo;
	waiting.push_back(root);
	while (!waiting.empty())
	{
		Node* nodeP = waiting.front();
		if (nodeP->left != NULL)
		{
			waiting.push_back(nodeP->left);
			waiting.push_back(nodeP->right);
		}
		topo.push_back(nodeP);
		waiting.erase(waiting.begin());
	}
	cout << "treeSize " << "*" << size << "*" << endl << endl;
	for (auto i : topo)
	{
		cout << "nth " << "*" << i->nth << "*" << endl;
		cout << "key " << "*" << i->key << "*" << endl;
		cout << "hashval " << "*" << i->hashval << "*" << endl;
		cout << "subTreeSize " << "*" << i->subTreeSize << "*" << endl << endl;
	}
	return 0;//成功
}

//IN:string 文件名，OUT:文件名对应Node指针，未找到返回NULL
Node* MerkleTree::search(string fileName)
{
	Node* newRootP = root;
	if (newRootP->key.find(fileName) == newRootP->key.npos)
		return NULL;
	else
	{
		while (true)
		{
			if (newRootP->left == NULL)
				return newRootP;
			else if (newRootP->left->key.find(fileName) != newRootP->left->key.npos)
			{
				newRootP = newRootP->left;
			}
			else
			{
				newRootP = newRootP->right;
			}
		}
	}
}

//IN:另一个MerkleTree的指针，OUT:变化节点的Vector<Node>
vector<Node> compare(Node* oldParentP, Node* newParentP)
{
	vector<Node> gap;
	if (oldParentP->hashval != newParentP->hashval && oldParentP->key == newParentP->key)
	{
		if (oldParentP->left == NULL && newParentP->left == NULL)
		{
			Node gapNode(*newParentP);
			gapNode.oldHashval = oldParentP->hashval;
			gap.push_back(gapNode);
		}
		else
		{
			gap = compare(oldParentP->left, newParentP->left);
			auto tempV = compare(oldParentP->right, newParentP->right);
			gap.insert(gap.end(), tempV.begin(), tempV.end());
		}
	}
	return gap;
}
vector<Node> compare(MerkleTree oldTree, MerkleTree newTree)
{
	if (oldTree.root->key == newTree.root->key)
		return compare(oldTree.root, newTree.root);
	else
	{
		vector<Node> temp;
		cout << "Fail. Contain different files." << endl;
		return temp;
	}
}

//按照文件夹下文件目录向树中添加新节点，返回受影响节点vector
vector<Node>MerkleTree::updataInsert()
{
	vector<Node> NodeV;
	vector<Node> addNodeV;
	readFolder(NodeV);
	for (auto i : NodeV)
	{
		if (search(i.key) == NULL)
		{
			auto toInsertV = insert(i);
			addNodeV.insert(addNodeV.end(), toInsertV.begin(), toInsertV.end());
		}
	}
	return addNodeV;
}

//按照文件夹下文件目录删除树中的一些节点，返回受影响节点vector
vector<Node*> MerkleTree::updateDelet()
{
	vector<Node> NodeV;
	vector<Node*> deletNodeV;
	vector<Node> leafV;
	readFolder(NodeV);
	leafV = getLeaf();
	for (auto i : leafV)
	{
		auto cmpKey = i.key;
		auto toDelP = find_if(NodeV.begin(), NodeV.end(), [cmpKey](Node a) {return cmpKey == a.key; });
		auto endNodeP = NodeV.end();
		if (toDelP == endNodeP)
		{
			auto toInsertV = delet(i);
			deletNodeV.insert(deletNodeV.end(), toInsertV.begin(), toInsertV.end());
		}
	}
	return deletNodeV;
}


int main()
{
	string folderPath1(FOLDER_PATH1);
	string folderPath2(FOLDER_PATH2);

	MerkleTree oldTree(folderPath1);
	MerkleTree newTree(folderPath2);

	//oldTree.generate();
	//newTree.generate();
	//oldTree.save();
	//newTree.save();
	//newTree.output();

	while (true)
	{
		cout << "1 load" << endl;
		cout << "2 generate" << endl;
		cout << "3 save" << endl;
		cout << "4 update_newFile" << endl;
		cout << "5 update_deletFile" << endl;
		cout << "9 compare folder1 and folder2" << endl;
		char command = getchar();
		getchar();
		if (command == 'q')
			break;
		else if (command == '1')
		{
			cout << "which folder" << endl;
			command = getchar();
			getchar();
			if (command == '1')
			{
				oldTree.load();
				oldTree.output();
			}
			else if (command == '2')
			{
				newTree.load();
				newTree.output();
			}
		}
		else if (command == '2')
		{
			cout << "which folder" << endl;
			command = getchar();
			getchar();
			if (command == '1')
			{
				oldTree.generate();
				oldTree.output();
			}
			else if (command == '2')
			{
				newTree.generate();
				newTree.output();
			}
		}
		else if (command == '3')
		{
			cout << "which folder" << endl;
			command = getchar();
			getchar();
			if (command == '1')
			{
				//保存树
				oldTree.save();
			}
			else if (command == '2')
			{
				//保存树
				newTree.save();
			}
		}
		else if (command == '4')
		{
			cout << "which folder" << endl;
			command = getchar();
			getchar();
			if (command == '1')
			{
				//检测添加文件
				oldTree.load();
				auto gap = oldTree.updataInsert();
				for (auto i : gap)
					cout << "Key: " << i.key << endl << "Hash: " << i.hashval << endl << endl;
			}
			else if (command == '2')
			{
				//检测添加文件
				newTree.load();
				auto gap = newTree.updataInsert();
				for (auto i : gap)
					cout << "Key: " << i.key << endl << "Hash: " << i.hashval << endl << endl;
			}
		}
		else if (command == '5')
		{
			cout << "which folder" << endl;
			command = getchar();
			getchar();
			if (command == '1')
			{
				//检测删除文件
				oldTree.load();
				auto gap = oldTree.updateDelet();
				for (auto i : gap)
					cout << "Key: " << i->key << endl << i->nth << endl << "Hash: " << i->hashval << endl << endl;
			}
			else if (command == '2')
			{
				//检测删除文件
				newTree.load();
				auto gap = newTree.updateDelet();
				for (auto i : gap)
					cout << "Key: " << i->key << endl << i->nth << endl << "Hash: " << i->hashval << endl << endl;
			}
		}
		else if (command == '9')
		{
			//检测修改文件，两文件夹横向对比
			oldTree.generate();
			newTree.generate();
			auto gap = compare(oldTree, newTree);
			for (auto i : gap)
			{
				cout << "Key: " << i.key << endl << "NewHash: " << i.hashval << endl << "OldHash: " << i.oldHashval << endl << endl;
				cout << "Recover?" << endl;
				command = getchar();
				getchar();
				if (command != 'y')
					continue;
				else
				{
					Node* toRecoverP = newTree.search(i.key);
					Node* referenceP = oldTree.search(i.key);
					ofstream toRecoverS;
					ifstream referenceS;
					toRecoverS.open(newTree.folderPath + "\\" + toRecoverP->key, ios_base::binary);
					referenceS.open(oldTree.folderPath + "\\" + referenceP->key, ios_base::binary);
					if (!(toRecoverS.is_open()&&referenceS.is_open()))
					{
						cout << "Error";
						exit(1);
					}
					char c[1];
					while (referenceS.read(c, 1))
					{
						if (referenceS.eof())
							break;
						toRecoverS << c[0];
					}
					toRecoverS.close();
					referenceS.close();
					toRecoverP->hashval = referenceP->hashval;
				}
			}
		}
		cout << "Complished. Enter to get back." << endl;
		while (getchar() != '\n');
		system("cls");
	}


	return 0;
}
