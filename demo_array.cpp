#pragma GCC("-W1,--stack=128000000")
#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>

using namespace std;

# define Max_Num_Vertex 10000

// 用于存放图
map<int, set<int>> graph_map;

// 用于存放环
// map<int, vector<vector<int>>> circle;
int cnt_circle[5] = {0};
int circle[5][Max_Num_Vertex];

// 节点编号最大值
int num_vertex = 0;

// 环的数量
int cnt = 0;

vector<string> split(const string& src, string separate_char)
{
    vector<string> strs;
    // 分割字符串的长度
    int len_separate_char = separate_char.size();
    int last_Pos = 0, index = -1;
    while(-1 != (index = src.find(separate_char, last_Pos)))
    {
        strs.emplace_back(src.substr(last_Pos, index - last_Pos));
        last_Pos = index + len_separate_char;
    }
    // 截取最后一个分隔符后的内容
    string last_Str = src.substr(last_Pos);
    if(!last_Str.empty())
        strs.emplace_back(last_Str); //如果最后一个分隔符后还有内容，则入队
    return strs;
}

void CreateGraph()
{
    int v;
    int start, end;
    string temp;
    string del = ",";
    // 文件流
    ifstream infile;
    infile.open("test_data_1.txt");

    while(!infile.eof())
    {
        infile>>temp;
        vector<string> strs = split(temp, del);
        start = stoi(strs[0]);
        end = stoi(strs[1]);
        num_vertex = max(num_vertex, max(start, end));
        // 如果当前节点没有在图中出现，则建立以start节点为出发点的图
        if(graph_map.find(start) == graph_map.end())
        {
            set<int> vertex_set;
            vertex_set.insert(end);
            graph_map.insert(pair<int, set<int>>(start, vertex_set));
        }
        // 否则将end节点插入到以start节点为起始点的图中
        else
        {
            graph_map[start].insert(end);
        }
    }
    return;
}

void DFS_thread(int n, bool *visited, vector<int> &stack, int &top, int depth, int &count)
{
    if(depth > 8)   return;
    int output[7];
    if(visited[n] && n == stack[0])
    {
        if(top > 1)
        {
            count++;
        }
        return;
    }
    if(visited[n])
    {
        return;
    }

    visited[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);
    top++;
    for(auto &set_it : graph_map[n])
    {
        if(set_it >= stack[0])
            DFS_thread(set_it, visited, stack, top, depth + 1, count);
    }
    top--;
    stack.pop_back();
    visited[n] = false;
    return;
}


void thread_checkcircle(int &count, map<int, set<int>>::iterator start, map<int, set<int>>::iterator end)
{
    int top = -1;
    vector<int> stack;
    bool *visited = new bool[num_vertex + 1];
    memset(visited, 0, sizeof(visited));
    int per = num_vertex / 4;
    for(auto it = start; it != end; ++it)
        DFS_thread(it->first, visited, stack, top, 1, count);
}

void CheckCircle()
{
    int count[4];
    int per = num_vertex / 4;

    thread first(thread_checkcircle, std::ref(count[0]), 0, graph_map.begin(), graph_map.begin() + per);
    thread second(thread_checkcircle, std::ref(count[1]), 1, graph_map.begin() + per, graph_map.begin() + (2 * per));
    thread third(thread_checkcircle, std::ref(count[2]), 2, graph_map.begin() + (2 * per), graph_map.begin() + (3 * per));
    thread fourth(thread_checkcircle, std::ref(count[3]), 3, graph_map.begin() + (3 * per), graph_map.end());

    first.join();
    second.join();
    third.join();
    fourth.join();

    cout<<count[0] + count[1] + count[2] + count[3]<<endl;
}

/*
void DFS(int n, bool *visited, vector<int> &stack, int &top, int depth)
{
    if(depth > 8)   return;
    int output[7];
    if(visited[n] && n == stack[0])
    {
        if(top > 1)
        {
            cnt++;
        }
        return;
    }
    if(visited[n])
    {
        return;
    }

    visited[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);
    top++;
    for(auto &set_it : graph_map[n])
    {
        if(set_it >= stack[0])
            DFS(set_it, visited, stack, top, depth + 1);
    }
    top--;
    stack.pop_back();
    visited[n] = false;
    return;
}

void CheckCircle()
{
    int top = -1;
    vector<int> stack;
    bool *visited = new bool[num_vertex + 1];
    memset(visited, 0, sizeof(bool) * (num_vertex + 1));
    for(auto &map_it : graph_map)
    {
        if(!visited[map_it.first])
        {
            DFS(map_it.first, visited, stack, top, 1);
        }
    }
    return;
}
*/

int main()
{
    int i = 1;
    time_t start, end1, end2;
    start = clock();
    CreateGraph();
    end1 = clock();
    cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    CheckCircle();
    end2 = clock();
    cout<<cnt<<endl;
    cout<<"Time of DFS: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}