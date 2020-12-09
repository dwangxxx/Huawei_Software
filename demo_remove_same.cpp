#include<bits/stdc++.h>
#include<algorithm>

using namespace std;

# define Max_Num_Vertex 30000

// ���ڴ��ͼ
map<int, set<int>> graph;

// ���ڴ�Ż�
map<int, vector<vector<int>>> circle;

// �ڵ������ֵ
int num_vertex = 0;

// ��������
int cnt = 0;

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
    infile.open("test_data_huawei.txt");

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
    cout<<"CreateGraph successfully!!!"<<endl;
    return;
}

void Remove_Same(vector<vector<int>> &circle_of_fixed_length, vector<int> &compare)
{
    bool judge = false;
    for(auto &vec_it : circle_of_fixed_length)
    {
        if(vec_it == compare)
        {
            judge = true;
            break;
        }
    }
    if(judge == false)  
    {
        cnt++;
        circle_of_fixed_length.emplace_back(compare);
    }
    return;
}

void DFS(int n, bool *visited, vector<int> &stack, int &top, bool *inStack)
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
            DFS(set_it, visited, stack, top, inStack);
        }
        // ����ڵ����ջ�У����ʾ��ǰ�л����������
        else 
        {
            int len_circle = 0;
            for(int j = top; j >= 0 && stack[j] != set_it && len_circle < 8; j--)
            {
                output.emplace_back(stack[j]);
                len_circle++;
            }
            len_circle++;
            output.emplace_back(set_it);
            // ��output��ת
            if(len_circle > 2 && len_circle < 8)
            {
                reverse(output.begin(), output.end());
                vector<int> change;
                vector<int>::iterator index_of_min = min_element(output.begin(), output.end());
                for(auto it = index_of_min; it != output.end(); ++it)
                    change.push_back(*it);
                for(auto it = output.begin(); it != index_of_min; ++it)
                    change.push_back(*it);
                if(circle.find(len_circle) == circle.end())
                {
                    vector<vector<int>> temp;
                    temp.emplace_back(change);
                    circle.insert(pair<int, vector<vector<int>>>(len_circle, temp));
                    cnt++;
                }
                else 
                {
                    // ȥ���ظ������ҽ������뵽circle���ݽṹ��
                    Remove_Same(circle[len_circle], change);
                }
            }
            output.clear();
        }
    }
    top--;
    stack.pop_back();
    inStack[n] = false;
    return;
}

void CheckCircle()
{
    int top = -1;
    vector<int> stack;
    bool *inStack = new bool[num_vertex + 1];
    bool *visited = new bool[num_vertex + 1];
    memset(inStack, 0, sizeof(bool) * (num_vertex + 1));
    memset(visited, 0, sizeof(bool) * (num_vertex + 1));
    for(auto &map_it : graph)
    {
        if(!visited[map_it.first])
        {
            DFS(map_it.first, visited, stack, top, inStack);
        }
    }
    cout<<"һ����"<<cnt<<"����"<<endl;
    return;
}

int main()
{
    int i = 1;
    CreateGraph();
    CheckCircle();
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