#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>

using namespace std;

bool less_sort(const vector<int> &path1, const vector<int> &path2)
{
    if(path1.size() != path2.size())    return path1.size() < path2.size();
    for(int i = 0; i < 7; i++)
    {
        if(path1[i] != path2[i])    return path1[i] < path2[i];
    }
}

int ans3[3 * 500000];
int ans4[4 * 500000];
int ans5[5 * 1000000];
int ans6[6 * 2000000];
int ans7[7 * 3000000];

int *ans[5] = {ans3, ans4, ans5, ans6, ans7};

class solve
{
public:
    // 用于存放图
    vector<vector<int>> graph;

    // 用于存放节点编号
    vector<int> nodes;
    int nodes_cnt = 0;

    // 用于存放环
    // map<int, vector<vector<int>>> circle;
    vector<vector<int>> circles;

    // 用于记录中间节点路径
    vector<unordered_map<int, vector<int>>> path;

    // 环的数量
    int cnt = 0;

    vector<string> id_comma;
    vector<string> id_end;

    vector<int> cur_reach;
    int count[5] = {0};

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

        id_comma.reserve(nodes.size());
        id_end.reserve(nodes.size());
        // 计算节点映射(node : i)
        for(auto &set_it : nodes)
        {
            id_comma.emplace_back(to_string(set_it) + ',');
            id_end.emplace_back(to_string(set_it) + '\n');
            Hash[set_it] = nodes_cnt++;
        }
    
        //cout<<"Nodes : "<<nodes_cnt<<endl;
        graph = vector<vector<int>>(nodes_cnt);

        // 构造图
        for(int i = 0; i < inputs.size(); i+=2)
            graph[Hash[inputs[i]]].emplace_back(Hash[inputs[i + 1]]);
        // 对领接表中的点进行排序
        for(auto &vec_it : graph)   sort(vec_it.begin(), vec_it.end());
    }

    void DFS_v2(int n, int head, vector<int> &stack, int depth, bool *visited)
    {
        visited[n] = true;
        // 将当前节点入栈
        stack.emplace_back(n);
        auto &graph_cur = graph[n];
        auto it = lower_bound(graph_cur.begin(), graph_cur.end(), head);
        // 处理3-6的环
        if(it != graph_cur.end() && *it == head && depth >= 3 && depth < 7)
        {
            cnt++;
            for(int i = 0; i < depth; i++)
                ans[depth - 3][count[depth - 3] * depth + i] = stack[i];
            count[depth - 3]++;
            //circles.emplace_back(stack);
        }
        // 进行DFS
        if(depth < 6)
        {
            for(int &vec_it : graph[n])
                if(!visited[vec_it] && vec_it > head)    DFS_v2(vec_it, head, stack, depth + 1, visited);
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
                for(int i = 0; i < depth; i++)
                    ans[4][count[4] * 7 + i] = stack[i];
                count[4]++;
                ans[4][count[4] * 7 - 1] = k;
                cnt++;
            }
        }
        stack.pop_back();
        visited[n] = false;
        return;
    }

    void CheckCircle()
    {
        vector<int> stack;
        cur_reach = vector<int>(nodes_cnt, -1);
        vector<int> temp;
        bool *visited = new bool[nodes_cnt];
        memset(visited, 0, sizeof(bool) * nodes_cnt);
        for(int i = 0; i < nodes_cnt; i++)
        {
            if(!graph[i].empty())
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
                DFS_v2(i, i, stack, 1, visited);
                for(int &i : temp)
                    cur_reach[i] = -1;
                temp.clear();
            }
        }
        return;
    }

    void DFS_search(int n, bool *vis, vector<int> &stack, int depth)
    {
        vis[n] = true;
        // 将当前节点入栈
        stack.emplace_back(n);

        if(depth == 1)
        {
            for(int &vec_it : graph[n])
                if(!vis[vec_it])    
                    DFS_search(vec_it, vis, stack, depth + 1);
        }

        else if(depth == 2)
        {
            for(int &vec_it : graph[n])
            {
                if(!vis[vec_it] && vec_it < stack[0] && vec_it < n)
                    path[vec_it][stack[0]].emplace_back(n);
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
            DFS_search(i, vis, stack, 1);
        }
        delete vis;
        return;
    }

    void Save_Results(string &outputFile)
    {
        FILE *fp = fopen(outputFile.c_str(), "wb");
        char buf[1024];

        cout<<cnt<<endl;
        int idx=sprintf(buf, "%d\n", cnt);
        buf[idx]='\0';
        fwrite(buf, idx , sizeof(char), fp );
        for(int i = 0; i < 5; i++)
        {
            int len = count[i];
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
};

int main()
{
    string test_file = "test_data.txt";
    string output_file = "output.txt";
    //time_t start, end1, end2, end3, end4;
    solve s;
    //start = clock();
    s.CreateGraph(test_file);
    //end1 = clock();
    //cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Search_2_Path();
    //end2 = clock();
    //cout<<"Time of Search: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    s.CheckCircle();
    //end3 = clock();
    //cout<<"Time of DFS: "<<(double)(end3 - end2) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Save_Results(output_file);
    //end4 = clock();
    //cout<<"Time of writing: "<<(double)(end4 - end3) / CLOCKS_PER_SEC<<"s"<<endl;
    //cout<<"count: "<<s.count<<endl;
    return 0;
}