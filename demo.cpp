#include<bits/stdc++.h>
#include<algorithm>

using namespace std;

# define Max_Num_Vertex 30000

// ���ڴ��ͼ
map<int, set<int>> graph;

// ���ڴ�Ż�
map<int, vector<vector<int>>> circle;
int num_vertex = 0;

vector<string> split(const string& src, string separate_char)
{
    vector<string> strs;
    // �ָ��ַ����ĳ���
    int len_separate_char = separate_char.size();
    int last_Pos = 0, index = -1;
    while(-1 != (index = src.find(separate_char, last_Pos)))
    {
        strs.emplace_back(src.substr(last_Pos, index - last_Pos));
        last_Pos = index + len_separate_char;
    }
    // ��ȡ���һ���ָ����������
    string last_Str = src.substr(last_Pos);
    if(!last_Str.empty())
        strs.emplace_back(last_Str); //������һ���ָ����������ݣ������
    return strs;
}

void CreateGraph()
{
    int v;
    int start, end;
    string temp;
    string del = ",";
    // �ļ���
    ifstream infile;
    infile.open("test_data.txt");

    while(!infile.eof())
    {
        infile>>temp;
        vector<string> strs = split(temp, del);
        start = stoi(strs[0]);
        end = stoi(strs[1]);
        num_vertex = max(num_vertex, max(start, end));
        // �����ǰ�ڵ�û����ͼ�г��֣�������start�ڵ�Ϊ�������ͼ
        if(graph.find(start) == graph.end())
        {
            set<int> vertex_set;
            vertex_set.insert(end);
            graph.insert(pair<int, set<int>>(start, vertex_set));
        }
        // ����end�ڵ���뵽��start�ڵ�Ϊ��ʼ���ͼ��
        else
        {
            graph[start].insert(end);
        }
    }
}

void DFS(int n, bool *visited, vector<int> &stack, int &top, bool *inStack, int &cnt)
{
    vector<int> output;
    visited[n] = true;
    // ����ǰ�ڵ���ջ
    stack.emplace_back(n);
    top++;
    // ��ʾ�ڵ�n��ջ��
    inStack[n] = true;

    // �����뵱ǰ�ڵ������бߵĽڵ�
    for(auto &set_it : graph[n])
    {
        // ����ڵ�û��ջ�У����Դ˽ڵ����DFS
        if(!inStack[set_it])
        {
            if(!visited[set_it])    DFS(set_it, visited, stack, top, inStack, cnt);
        }
        // ����ڵ����ջ�У����ʾ��ǰ�л����������
        else 
        {
            cnt++;
            int len_circle = 0;
            for(int j = top; j >= 0 && stack[j] != set_it; j--)
            {
                output.emplace_back(stack[j]);
                len_circle++;
            }
            output.emplace_back(set_it);
            if(circle.find(len_circle) == circle.end())
            {
                vector<vector<int>> temp;
                temp.emplace_back(output);
                circle.insert(pair<int, vector<vector<int>>>(len_circle, temp));
            }
            else 
            {
                circle[len_circle].emplace_back(output);
            }
            output.clear();
        }
    }
    top--;
    stack.pop_back();
    inStack[n] = false;
}

void CheckCircle()
{
    int cnt;
    int top = -1;
    vector<int> stack;
    bool *inStack = new bool[num_vertex];
    bool *visited = new bool[num_vertex];
    memset(inStack, 0, sizeof(bool) * num_vertex);
    memset(visited, 0, sizeof(bool) * num_vertex);
    for(auto &map_it : graph)
    {
        if(!visited[map_it.first])
        {
            DFS(map_it.first, visited, stack, top, inStack, cnt);
        }
    }
    cout<<"һ����"<<cnt<<endl;
}

int main()
{
    int i = 1;
    CreateGraph();
    CheckCircle();
    /*
    for(auto &map_it : graph)
    {
        cout<<"Start: "<<map_it.first<<endl;
        for(auto &set_it : map_it.second)
            cout<<set_it<<"  ";
        cout<<endl;
    }
    */
    for(auto &map_it : circle)
    {
        cout<<"����Ϊ"<<map_it.first<<"�Ļ�Ϊ��"<<endl;
        for(auto &vec_it : map_it.second)
        {
            for(auto &vec : vec_it)     cout<<vec<<"->";
            cout<<endl;
        }
    }
    return 0;
}