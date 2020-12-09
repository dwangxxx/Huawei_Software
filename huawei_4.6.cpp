#include <iostream>
#include <fstream>
#include <typeinfo>
#include <string>
#include <vector>
#include <malloc.h>
#include <memory.h>
#include<map>
#include<algorithm>
/* 4.6�հ汾���Ѿ�ʵ���˶��������ȥ�أ����Եõ���ȷ������������ˣ���һ����Ҫ����txt����*/
using namespace std;
const int MAX_Vertex_Num=5018;
int vexs[MAX_Vertex_Num]={0};
int arcs[MAX_Vertex_Num][MAX_Vertex_Num]; //�ڽӾ���
int vexnum=5018;//������
int arcnum=5031;//����
int cnt=0;

map<int, vector<vector<int>>> out;

void CreateGraph();//����ͼ
void CheckCircle();
//
int get_position(vector<int> &arr,int x,int len);
bool myFunction(vector<int> &arr, int v);
void quickSort(vector<int> &a,int,int);
//

int get_position(int arr[],int x,int len){
	for(int i=0;i<len;i++){
		if (arr[i]==x){
			return i;
			}
		}
}

vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;
	int separate_characterLen = separate_character.size();//�ָ��ַ����ĳ���,�����Ϳ���֧���硰,,�����ַ����ķָ���
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//��ȡ���һ���ָ����������
	if (!lastString.empty())
		strs.push_back(lastString);//������һ���ָ����������ݾ����
	return strs;
}
void quickSort(int s[], int l, int r)
{
	if (l< r)
	{      
		int i = l, j = r, x = s[l];
		while (i < j)
		{
			while(i < j && s[j]>= x) // ���������ҵ�һ��С��x����
				j--; 
			if(i < j)
				s[i++] = s[j];
			while(i < j && s[i]< x) // ���������ҵ�һ�����ڵ���x����
				i++; 
			if(i < j)
				s[j--] = s[i];
		}
		s[i] = x;
		quickSort(s, l, i - 1); // �ݹ����
		quickSort(s, i + 1, r);
	}
}


bool in_array(int arr[],int v){
	for(int i=0; i<MAX_Vertex_Num; i++){
		if (arr[i] == v){
			return true;
			}
		}
	return false;
}

void CreateGraph()
{
	int v;//Ҫ���ҵ�Ԫ��,����Ҫ��vector<>����һ��
	int edges[arcnum][2] = { 0 };//����һ��arcnum*2�ľ������ڴ������
	vector<int> vexs_data;
	// int vexs_data[vexnum] = { 0 };
	int i=0;
	int k=0;
	string temp;
	string del =",";
	ifstream infile;//�����ȡ�ļ���������ڳ�����˵��in
	infile.open("test_data.txt");//���ļ�
	while(!infile.eof())
	{
		infile>>temp;
		vector<string> strs = split(temp,del);
		for (int j = 0; j < strs.size()-1; j++)
		{
			v=std::stoi(strs[j]);
			edges[i][j]=v;
			if(find(vexs_data.begin(), vexs_data.end(), v) == vexs_data.end()){
				vexs_data.emplace_back(v);
				k=k+1;
			}
		}
		i=i+1;
	}
	

	int len = sizeof(vexs_data) / sizeof(int);
	quickSort(vexs_data,0,len-1);
	cout<<vexnum<<endl;
    
	//vexnum���������� vexs_data�洢�˶���Ĳ�ͬ���� 
	for (int i=0; i<vexnum; i++)
	{
		vexs[i]=vexs_data[i];
	}
	
	for (int i=0; i<vexnum; i++)
	{
		for (int j=0; j<vexnum; j++)
		{
			arcs[i][j]=0;
		}
	}
	
	for (int i=0;i<arcnum;i++)
	{
		int x=get_position(vexs_data, edges[i][0], len);
		int y=get_position(vexs_data, edges[i][1], len);
		arcs[x][y]=1;//�������Ȩ�Ļ�������Ӧ����arc[x][y]=Ȩֵ
	}
}

void DFS(int x, bool visited[MAX_Vertex_Num], vector<int> &stack, int& top, bool inStack[MAX_Vertex_Num], int& cnt)
{
	vector<int> output;
	int insert_loc;
	visited[x] = true; //������һ���ڵ�x=0����visited[x]����Ϊtrue ��Ҳ���ýڵ㱻���� 
	//stack[++top]=x;	//top��-1��ʼ��++top=0,����x=0��ջ�� 
	stack.push_back(x);
	top++;
	inStack[x] = true;	//x=0��ջ�󣬰�instack[0]����Ϊtrue��˵��x=0��ջ�� 
	for (int i=0; i<vexnum; i++) //���ڴ������x���㣬�ӵ�1�����㿪ʼ���x�����к�̶��㡣 
	{
		if (arcs[x][i]!=0)//�бߣ��ҵ�x=0�ĺ�̣�1��2
		{
			if (!inStack[i])//�����ҵ�����1����Ȼ1����ջ�У����Զ�1����DFS��...visit[1]��stack[1]���������Ȼ����x=2�� 
			{
				// ��֦
				if(!visited[i])		DFS(i, visited, stack, top, inStack, cnt);
			}
			else //������������ʾ�±�Ϊx=2�Ķ��㵽 �±�Ϊi=0�Ķ����л�
			{
				int len_circle = 0;

				for (int j=t; j<=top; j++)
				{
					output.emplace_back(vexs[stack[j]]);
					stack.pop_back();
					len_circle++;
				}
			}
		}
	}
	//�����������ջ
	top--;
	inStack[x] = false;
}

void CheckCircle()
{
	//int cnt=0;//���ĸ���
	int top=-1;
	vector<int> stack;
	//int stack[MAX_Vertex_Num];//����һ������Ϊ5�����飬����һ��ջ 
	bool inStack[MAX_Vertex_Num]={false};// ����Ƿ������ջ���� 
	bool visited[MAX_Vertex_Num]={false};//�Ƿ񱻷��ʹ� 
	for (int i=0;i<vexnum;i++)
	{
		if (!visited[i]) //�ӵ�һ���ڵ㿪ʼ�����û�б����ʹ�������dfs���� 
		{
			DFS(i,visited,stack,top,inStack,cnt);
		}
	}
}

int main()
{
	CreateGraph();
	CheckCircle();
	cout<<cnt<<endl;
	system("pause");
	return 1;
}