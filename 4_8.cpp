#include<bits/stdc++.h>
#include<algorithm>
using namespace std;

# define Max_Num_Vertex 30000

map<int, vector<int>> graph;

vector<string> split(const string& src, string separate_character)
{
	vector<string> strs;
	int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
	if (!lastString.empty())
		strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
	return strs;
}

void CreateGraph()
{
	int v;//要查找的元素,类型要与vector<>类型一致
	int edges[arcnum][2] = { 0 };//定义一个arcnum*2的矩阵，用于存放数据
	vector<int> vexs_data;
	// int vexs_data[vexnum] = { 0 };
	int i=0;
	int k=0;
	string temp;
	string del =",";
	ifstream infile;//定义读取文件流，相对于程序来说是in
	infile.open("test_data.txt");//打开文件
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
    
	//vexnum个顶点数， vexs_data存储了顶点的不同名字 
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
		arcs[x][y]=1;//如果是有权的话，这里应该是arc[x][y]=权值
	}
}

void DFS(int x, bool visited[MAX_Vertex_Num], vector<int> &stack, int& top, bool inStack[MAX_Vertex_Num], int& cnt)
{
	vector<int> output;
	int insert_loc;
	visited[x] = true; //当进入一个节点x=0，将visited[x]设置为true ，也即该节点被访问 
	//stack[++top]=x;	//top从-1开始，++top=0,所以x=0入栈。 
	stack.push_back(x);
	top++;
	inStack[x] = true;	//x=0入栈后，把instack[0]设置为true，说明x=0在栈中 
	for (int i=0; i<vexnum; i++) //现在处理的是x顶点，从第1个顶点开始检查x的所有后继顶点。 
	{
		if (arcs[x][i]!=0)//有边，找到x=0的后继，1和2
		{
			if (!inStack[i])//首先找到的是1，显然1不在栈中，所以对1进行DFS，...visit[1]和stack[1]被填充满，然后是x=2， 
			{
				// 剪枝
				if(!visited[i])		DFS(i, visited, stack, top, inStack, cnt);
			}
			else //条件成立，表示下标为x=2的顶点到 下标为i=0的顶点有环
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
	//处理完结点后，退栈
	top--;
	inStack[x] = false;
}

void CheckCircle()
{
	//int cnt=0;//环的个数
	int top=-1;
	vector<int> stack;
	//int stack[MAX_Vertex_Num];//定义一个长度为5的数组，当做一个栈 
	bool inStack[MAX_Vertex_Num]={false};// 检查是否出现在栈里面 
	bool visited[MAX_Vertex_Num]={false};//是否被访问过 
	for (int i=0;i<vexnum;i++)
	{
		if (!visited[i]) //从第一个节点开始，如果没有被访问过，进行dfs搜索 
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