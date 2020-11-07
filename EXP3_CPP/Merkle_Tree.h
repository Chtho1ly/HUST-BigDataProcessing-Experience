#include <io.h>//所需头文件
#include <iostream>
#include <string>
#include <vector>

typedef struct node {
	string key;						//节点的键，用来加速检索，可选数据块或其名称的代表性特征
	string hashval;					//本节点的哈希值，叶子节点为数据块的哈希，中间节点和根节点为左右子节点拼接后值的哈希。
	string oldHashval;			//原保存的该节点哈希值，通常无意义
	int subTreeSize;				//本届点下的文件数量，若节点为叶子节点则为1
	struct node* parent;		//指向父亲节点
	struct node* left;				//指向左子树的根节点
	struct node* right;			//指向右子树的根节点
	string nth;						//标号，根节点为“1”，左子树在后面添加‘0’，右子树添加‘1’
	bool isleaf;						//本节点是否为叶子节点，本实验中未使用
	node(string keyInit, string hashvalInit)			//构造函数，初始化key和哈希值
	{
		key = keyInit;
		hashval = hashvalInit;
	};
} Node;

typedef struct merkleTree {
public:
	int size;					//Merkle树的规模，包含根节点、中间节点和叶子节点
	Node* root;				//指向Merkle树的根节点
	string folderPath;		//本Merkle树对应的文件目录
	//构造函数
	merkleTree(string folderPathInit)
	{
		size = 0;
		root = NULL;
		folderPath = folderPathInit;
	};
	//生成最新文件目录对应的merkleTree
	int generate();
	//将merkleTree存储到文件MerkleTree.cfg中
	int save();
	//从文件中读取存储的merkleTree
	int load();
	//打印所有节点
	int output();
	//IN:string 文件名，OUT:对应Node指针，未找到返回NULL
	Node* search(string fileName);
	//按照文件夹下文件目录向树中添加新节点，返回受影响节点vector
	vector<Node> updataInsert();
	//按照文件夹下文件目录删除树中的一些节点，返回受影响节点vector
	vector<Node*> updateDelet();
private:
	//向merkleTree中插入node
	vector<Node> insert(Node newNode);
	//从merkleTree中删除node
	vector<Node*> delet(Node toDelNode);
	//返回所有叶子结点的指针
	vector<Node> getLeaf()
	{
		vector<Node*> waiting;
		vector<Node> leafV;
		waiting.push_back(root);
		while (!waiting.empty())
		{
			Node* nodeP = waiting.front();
			if (nodeP->left != NULL)
			{
				waiting.push_back(nodeP->left);
				waiting.push_back(nodeP->right);
			}
			else
				leafV.push_back(*nodeP);
			waiting.erase(waiting.begin());
		}
		sort(leafV.begin(), leafV.end(), [](Node a, Node b) {return a.key < b.key; });
		return leafV;
	}
	//读取文件夹下的所有文件
	int readFolder(vector<Node>& nodeV);
	//读取一行字符串中两个* 之间的数据，作为string返回
	string getData(string line)
	{
		int dataBegin, dataEnd;
		dataBegin = line.find_first_of("*");
		dataEnd = line.find_last_of("*");
		string data = line.substr(dataBegin + 1, dataEnd - dataBegin - 1);
		return data;
	}
} MerkleTree;