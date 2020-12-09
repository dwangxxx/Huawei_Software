#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>
#include<pthread.h>
#include<sys/time.h>

using namespace std;

bool less_sort(const vector<int> &path1, const vector<int> &path2)
{
    if(path1.size() != path2.size())    return path1.size() < path2.size();
    for(int i = 0; i < 7; i++)
    {
        if(path1[i] != path2[i])    return path1[i] < path2[i];
    }
}

pthread_mutex_t p_mutex = PTHREAD_MUTEX_INITIALIZER;

int ans3[3 * 500000];
int ans4[4 * 500000];
int ans5[5 * 1000000];
int ans6[6 * 2000000];
int ans7[7 * 3000000];

int *ans[5] = {ans3, ans4, ans5, ans6, ans7};

vector<vector<int>> results[5];

string id_comma[280000];
string id_end[280000];
int graph[280000][55]={{0}};

int circles_count[5] = {0};

// 每个线程搜索环的计数
int circles_cnt[4] = {0};

// 用于存放节点编号
vector<int> nodes;
int nodes_cnt = 0;


// 用于记录中间节点路径
vector<unordered_map<int, vector<int>>> path;

void CreateGraph(string test_file)
{
    int inputs[2 * 280000] = {0};
    //vector<int> inputs;
    unordered_map<int, int> Hash;

    FILE* file = fopen(test_file.c_str(), "r");
    int start, end, cost;
    int edges_cnt = 0;
    // 输入
    while(fscanf(file, "%i,%i,%i", &start, &end, &cost)!=EOF){
        inputs[edges_cnt * 2] = start;
        inputs[edges_cnt * 2 + 1] = end;
        nodes.emplace_back(start);
        nodes.emplace_back(end);
        edges_cnt++;
    }
    sort(nodes.begin(), nodes.end());
    nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
    cout<<"Edges : "<<edges_cnt<<endl;

    // 计算节点映射(node : i)
    int i = 0;
    for(auto &set_it : nodes)
    {
        id_comma[i] = to_string(set_it) + ',';
        id_end[i++] = to_string(set_it) + '\n';
        Hash[set_it] = nodes_cnt++;
    }
    cout<<"Nodes : "<<nodes_cnt<<endl;
    // 构造图
    for(i = 0; i < edges_cnt * 2; i+=2)
    {
        auto tmp = Hash[inputs[i]];
        graph[tmp][0]++;
        graph[tmp][graph[tmp][0]] = Hash[inputs[i + 1]];
    }
    // 对领接表中的点进行排序
    for(i = 0; i < nodes_cnt; i++)
    {
        if(graph[i][0] != 0)
        {
            sort(graph[i] + 1, graph[i] + graph[i][0] + 1);
        }
    }
}

void DFS_v2(int n, int head, vector<int> &stack, int depth, bool *visited, vector<int> &cur_reach, int *circles_cnt)
{
    visited[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);
    // 处理3-6的环
    if(graph[n][graph[n][0]] >= head && graph[n][1] <= head)
    for(int i = 0; i < graph[n][0]; i++)
    {
        if(graph[n][i + 1] == head && depth >= 3 && depth < 7)
        {
            (*circles_cnt)++;
            pthread_mutex_lock(&p_mutex);
            results[depth - 3].emplace_back(stack);
            //for(int j = 0; j < depth; j++)
            //    ans[depth - 3][circles_count[depth - 3] * depth + j] = stack[j];
            //circles_count[depth - 3]++;
            pthread_mutex_unlock(&p_mutex);
            break;
        }
    }
    // 进行DFS
    if(depth < 6)
    {
        if(graph[n][graph[n][0]] > head)
        {
            for(int i = 0; i < graph[n][0]; i++)
                if(!visited[graph[n][i + 1]] && graph[n][i + 1] > head)    DFS_v2(graph[n][i + 1], head, stack, depth + 1, visited, cur_reach, circles_cnt);
        }
    }
    // 处理长度为7的环
    else if(cur_reach[n] > -1 && depth == 6)
    {
        int len = path[head][n].size();
        auto cur_mid_path = path[head][n];
        for(int i = cur_reach[n]; i < len; i++)
        {
            int k = cur_mid_path[i];
            if(visited[k])  continue;
            auto temp = stack;
            temp.emplace_back(k);
            pthread_mutex_lock(&p_mutex);
            results[4].emplace_back(temp);
            /*
            for(int j = 0; j < depth; j++)
                ans[4][circles_count[4] * 7 + j] = stack[j];
            circles_count[4]++;
            ans[4][circles_count[4] * 7 - 1] = k;
            */
            pthread_mutex_unlock(&p_mutex);
            (*circles_cnt)++;
        }
    }
    stack.pop_back();
    visited[n] = false;
    return;
}

void CheckCircle(int num_per_thread, int thread_id, int *circles_cnt)
{
    auto end1 = clock();
    vector<int> stack;
    vector<int> cur_reach;
    cur_reach = vector<int>(nodes_cnt, -1);
    vector<int> temp;
    bool *visited = new bool[nodes_cnt];
    memset(visited, 0, sizeof(bool) * nodes_cnt);
    for(int i = thread_id; i < nodes_cnt; i += 4)
    {
        if(graph[i][0] != 0)
        {
            for(auto &vec_it : path[i]){
                int vec = vec_it.first;
                if(vec > i){
                    auto &val = vec_it.second;
                    int reach_i = lower_bound(val.begin(), val.end(), i) - val.begin();
                    if(reach_i < val.size())    cur_reach[vec] = reach_i;
                    temp.emplace_back(vec);
                }
            }
            DFS_v2(i, i, stack, 1, visited, cur_reach, circles_cnt);
            for(int &i : temp)
                cur_reach[i] = -1;
            temp.clear();
        }
    }
    return;
}

void check_circle_thread()
{
    int num_per_thread = nodes_cnt / 4;
    thread t1(CheckCircle, num_per_thread, 0, &circles_cnt[0]);
    thread t2(CheckCircle, num_per_thread, 1, &circles_cnt[1]);
    thread t3(CheckCircle, num_per_thread, 2, &circles_cnt[2]);
    CheckCircle(num_per_thread, 3, &circles_cnt[3]);
    t1.join();
    t2.join();
    t3.join();
    for(int i = 0; i < 5; i++)
        sort(results[i].begin(), results[i].end(), less_sort);
    cout<<circles_cnt[0]+circles_cnt[1]+circles_cnt[2]+circles_cnt[3]<<endl;
}

void DFS_search(int n, bool *vis, vector<int> &stack, int depth)
{
    vis[n] = true;
    // 将当前节点入栈
    stack.emplace_back(n);

    if(depth == 1)
    {
        for(int i = 0; i < graph[n][0]; i++)
            if(!vis[graph[n][i + 1]])    
                DFS_search(graph[n][i + 1], vis, stack, depth + 1);
    }

    else if(depth == 2)
    {
        for(int i = 0; i < graph[n][0]; i++)
        {
            auto tmp = graph[n][i + 1];
            if(!vis[tmp] && tmp < stack[0] && tmp < n)
                path[tmp][stack[0]].emplace_back(n);
        }
    }

    stack.pop_back();
    vis[n] = false;
    return;
}

void Search_2_Path()
{
    path = vector<unordered_map<int, vector<int>>>(nodes_cnt);
    vector<int> stack;
    bool *vis = new bool[nodes_cnt];
    memset(vis, 0, sizeof(bool) * nodes_cnt);
    for(int i = 0; i < nodes_cnt; i++)
    {
        if(graph[i][0] != 0)
            DFS_search(i, vis, stack, 1);
    }
    delete vis;
    return;
}

void Save_Results(string &outputFile)
{
    FILE *fp = fopen(outputFile.c_str(), "wb");
    char buf[1024];

    int cnt = circles_cnt[0] + circles_cnt[1] + circles_cnt[2] + circles_cnt[3];
    cout<<cnt<<endl;
    int idx=sprintf(buf, "%d\n", cnt);
    buf[idx]='\0';
    fwrite(buf, idx , sizeof(char), fp );
    for(int i = 0; i < 5; i++)
    {
        int len = results[i].size();
        for(int j = 0; j < len; j++)
        {
            for(int k = 0; k < i + 2; k++)
            {
                auto res = id_comma[results[i][j][k]];
                fwrite(res.c_str(), res.size(), sizeof(char), fp);
            }
            auto res = id_end[results[i][j][i + 2]];
            fwrite(res.c_str(), res.size(), sizeof(char), fp);
        }
    }
    fclose(fp);
}

/*
void Save_Results(string &outputFile)
{
    FILE *fp = fopen(outputFile.c_str(), "wb");
    char buf[1024];

    int cnt = circles_cnt[0] + circles_cnt[1] + circles_cnt[2] + circles_cnt[3];
    cout<<cnt<<endl;
    int idx=sprintf(buf, "%d\n", cnt);
    buf[idx]='\0';
    fwrite(buf, idx , sizeof(char), fp );
    for(int i = 0; i < 5; i++)
    {
        int len = circles_count[i];
        for(int j = 0; j < len; j++)
        {
            for(int k = (i + 3) * j; k < (i + 3) * ( j + 1) - 1; k++)
            {
                auto res = id_comma[ans[i][k]];
                fwrite(res.c_str(), res.size(), sizeof(char), fp);
            }
            auto res = id_end[ans[i][(i + 3) * (j + 1) - 1]];
            fwrite(res.c_str(), res.size(), sizeof(char), fp);
        }
    }
    fclose(fp);
}
*/

int main()
{
    string test_file = "test_data.txt";
    string output_file = "output.txt";
    struct timeval us1, us2, us3;
    gettimeofday(&us1, NULL);
    CreateGraph(test_file);
    Search_2_Path();
    check_circle_thread();
    gettimeofday(&us2, NULL);
    cout<<"Time of DFS: "<<(double)(us2.tv_sec - us1.tv_sec + (us2.tv_usec - us1.tv_usec) / 1000000)<<"s"<<endl;
    Save_Results(output_file);
    gettimeofday(&us3, NULL);
    cout<<"Time of writing: "<<(double)(us3.tv_sec - us2.tv_sec + (us3.tv_usec - us2.tv_usec) / 1000000)<<"s"<<endl;
    //cout<<us1.tv_sec<<"s"<<us1.tv_usec<<endl;
    //cout<<us2.tv_sec<<"s"<<us2.tv_usec<<endl;
    //cout<<us3.tv_sec<<"s"<<us3.tv_usec<<endl;
    return 0;
}