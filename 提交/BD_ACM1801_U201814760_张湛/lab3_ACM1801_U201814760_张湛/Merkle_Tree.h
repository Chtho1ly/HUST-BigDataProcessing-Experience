#include <io.h>//����ͷ�ļ�
#include <iostream>
#include <string>
#include <vector>

typedef struct node {
	string key;						//�ڵ�ļ����������ټ�������ѡ���ݿ�������ƵĴ���������
	string hashval;					//���ڵ�Ĺ�ϣֵ��Ҷ�ӽڵ�Ϊ���ݿ�Ĺ�ϣ���м�ڵ�͸��ڵ�Ϊ�����ӽڵ�ƴ�Ӻ�ֵ�Ĺ�ϣ��
	string oldHashval;			//ԭ����ĸýڵ��ϣֵ��ͨ��������
	int subTreeSize;				//������µ��ļ����������ڵ�ΪҶ�ӽڵ���Ϊ1
	struct node* parent;		//ָ���׽ڵ�
	struct node* left;				//ָ���������ĸ��ڵ�
	struct node* right;			//ָ���������ĸ��ڵ�
	string nth;						//��ţ����ڵ�Ϊ��1�����������ں�����ӡ�0������������ӡ�1��
	bool isleaf;						//���ڵ��Ƿ�ΪҶ�ӽڵ㣬��ʵ����δʹ��
	node(string keyInit, string hashvalInit)			//���캯������ʼ��key�͹�ϣֵ
	{
		key = keyInit;
		hashval = hashvalInit;
	};
} Node;

typedef struct merkleTree {
public:
	int size;					//Merkle���Ĺ�ģ���������ڵ㡢�м�ڵ��Ҷ�ӽڵ�
	Node* root;				//ָ��Merkle���ĸ��ڵ�
	string folderPath;		//��Merkle����Ӧ���ļ�Ŀ¼
	//���캯��
	merkleTree(string folderPathInit)
	{
		size = 0;
		root = NULL;
		folderPath = folderPathInit;
	};
	//���������ļ�Ŀ¼��Ӧ��merkleTree
	int generate();
	//��merkleTree�洢���ļ�MerkleTree.cfg��
	int save();
	//���ļ��ж�ȡ�洢��merkleTree
	int load();
	//��ӡ���нڵ�
	int output();
	//IN:string �ļ�����OUT:��ӦNodeָ�룬δ�ҵ�����NULL
	Node* search(string fileName);
	//�����ļ������ļ�Ŀ¼����������½ڵ㣬������Ӱ��ڵ�vector
	vector<Node> updataInsert();
	//�����ļ������ļ�Ŀ¼ɾ�����е�һЩ�ڵ㣬������Ӱ��ڵ�vector
	vector<Node*> updateDelet();
private:
	//��merkleTree�в���node
	vector<Node> insert(Node newNode);
	//��merkleTree��ɾ��node
	vector<Node*> delet(Node toDelNode);
	//��������Ҷ�ӽ���ָ��
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
	//��ȡ�ļ����µ������ļ�
	int readFolder(vector<Node>& nodeV);
	//��ȡһ���ַ���������* ֮������ݣ���Ϊstring����
	string getData(string line)
	{
		int dataBegin, dataEnd;
		dataBegin = line.find_first_of("*");
		dataEnd = line.find_last_of("*");
		string data = line.substr(dataBegin + 1, dataEnd - dataBegin - 1);
		return data;
	}
} MerkleTree;