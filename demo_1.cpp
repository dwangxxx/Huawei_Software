#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>

using namespace std;

struct circle_path{
    vector<vector<int>> path_node;

    circle_path(){};
};

bool less_sort(const vector<int> &path1, const vector<int> &path2)
{
    for(int i = 0; i < 7; i++)
    {
        if(path1[i] != path2[i])    return path1[i] < path2[i];
    }
}

// 用于存放图
vector<vector<int>> graph;

// 用于存放节点编号
vector<int> nodes;
int nodes_cnt = 0;

// 用于存放环
// map<int, vector<vector<int>>> circle;
struct circle_path *circles = new struct circle_path[5];

// 用于记录中间节点路径
vector<unordered_map<int, vector<int>>> path;

// 环的数量
int cnt = 0;

void CreateGraph(string test_file)
{
    vector<int> inputs;
    unordered_map<int, int> Hash;

    FILE* file = fopen(test_file.c_str(), "r");
    int start, end, cost;
    int edges_cnt = 0;
    // 输入
    while(fscanf(file, "%i,%i,%i", &start, &end, &cost)!=EOF){
        inputs.emplace_back(start);
        inputs.emplace_back(end);
        nodes.emplace_back(start);
        nodes.emplace_back(end);
        edges_cnt++;
    }
    sort(nodes.begin(), nodes.end());
    nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
    //cout<<"Edges : "<<edges_cnt<<endl;

    // 计算节点映射(node : i)
    for(auto &set_it : nodes)
        Hash[set_it] = nodes_cnt++;
    
    //cout<<"Nodes : "<<nodes_cnt<<endl;
    graph = vector<vector<int>>(nodes_cnt);

    // 构造图
    for(int i = 0; i < inputs.size(); i+=2)
        graph[Hash[inputs[i]]].emplace_back(Hash[inputs[i + 1]]);
    for(auto &vec_it : graph)   sort(vec_it.begin(), vec_it.end());
}

void DFS(int n, bool *visited, vector<int> &stack, int depth)
{
    visited[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);
    if(depth == 6)
    {
        int temp;
        int len = path[stack[5]][stack[0]].size();
        for(int i = 0; i < len; i++)
        {
            temp = path[stack[5]][stack[0]][i];
            if(!visited[temp])
            {
                cnt++;
                stack.emplace_back(temp);
                circles[4].path_node.emplace_back(stack);
                stack.pop_back();
            }
        }
        for(int &vec_it : graph[n])
        {
            if(vec_it == stack[0])
            {
                cnt++;
                circles[3].path_node.emplace_back(stack);
            }
        }
        stack.pop_back();
        visited[n] = false;
        return;
    }
    for(int &vec_it : graph[n])
    {
        if(vec_it == stack[0] && depth >= 3)
        {
            cnt++;
            circles[depth - 3].path_node.emplace_back(stack);
        }
        else if(depth < 6 && !visited[vec_it] && vec_it > stack[0])
            DFS(vec_it, visited, stack, depth + 1);
    }
    stack.pop_back();
    visited[n] = false;
    return;
}

void CheckCircle()
{
    vector<int> stack;
    bool *visited = new bool[nodes_cnt];
    memset(visited, 0, sizeof(bool) * nodes_cnt);
    for(int i = 0; i < nodes_cnt; i++)
    {
        DFS(i, visited, stack, 1);
    }
    // 将第七层排序
    sort(circles[4].path_node.begin(), circles[4].path_node.end(), less_sort);
    return;
}

void DFS_search(int n, bool *visited, vector<int> &stack, int depth)
{
    visited[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);

    if(depth == 1)
    {
        for(int &vec_it : graph[n])
            if(!visited[vec_it])    
                DFS_search(vec_it, visited, stack, depth + 1);
    }

    else if(depth == 2)
    {
        for(int &vec_it : graph[n])
        {
            if(!visited[vec_it] && vec_it < stack[0] && vec_it < n)
                path[stack[0]][vec_it].emplace_back(n);
        }
    }

    stack.pop_back();
    visited[n] = false;
    return;
}

void Search_Path()
{
    path = vector<unordered_map<int, vector<int>>>(nodes_cnt);
    vector<int> stack;
    bool *visited = new bool[nodes_cnt];
    memset(visited, 0, sizeof(bool) * nodes_cnt);
    for(int i = 0; i < nodes_cnt; i++)
    {
        DFS_search(i, visited, stack, 1);
    }
    delete visited;
    return;
}

int Save_Results(string &outputFile)
{
    time_t start = clock();
    ofstream out(outputFile);
    out<<cnt<<endl;
    for(int i = 0; i < 5; i++)
    {
        for(auto &path_vec : circles[i].path_node)
        {
            int len = path_vec.size();
            out<<nodes[path_vec[0]];
            for(int j = 1; j < len; j++)
                out<<","<<nodes[path_vec[j]];
            out<<endl;
        }
    }
    time_t end = clock();
    cout<<end - start<<endl;
    exit(0);
}

int main()
{
    string test_file = "test_data_huawei.txt";
    string output_file = "output.txt";
    time_t start, end1, end2, end3, end4;
    start = clock();
    CreateGraph(test_file);
    end1 = clock();
    Search_Path();
    end2 = clock();
    CheckCircle();
    end3 = clock();
    cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"Time of Search: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"Time of DFS: "<<(double)(end3 - end2) / CLOCKS_PER_SEC<<"s"<<endl;
    Save_Results(output_file);
    exit(0);
}