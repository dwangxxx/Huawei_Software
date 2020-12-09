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
    struct circle_path *circles = new struct circle_path[5];

    // 用于记录中间节点路径
    vector<unordered_map<int, vector<int>>> path;

    // 环的数量
    int cnt = 0;

    vector<string> id_comma;
    vector<string> id_end;

    vector<int> cur_reach;

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

    void checkcircle_for()
    {
        vector<int> stack;
        cur_reach = vector<int>(nodes_cnt, -1);
        vector<int> temp;
        bool *visited = new bool[nodes_cnt];
        memset(visited, 0, sizeof(bool) * nodes_cnt);
        vector<int> &graph_cur = graph[0];
        vector<int>::iterator it;
        // 第一层
        for(int i = 0; i < nodes_cnt; i++)
        {
            if(!graph[i].empty())
            {
                for(auto &vec_it : path[i])
                {
                    int vec = vec_it.first;
                    cur_reach[vec] = 0;
                    temp.emplace_back(vec);
                    /*
                    if(vec > i)
                    {
                        auto &val = vec_it.second;
                        int reach_i = lower_bound(val.begin(), val.end(), i) - val.begin();
                        if(reach_i < val.size())    cur_reach[vec] = reach_i;
                        temp.emplace_back(vec);
                    }
                    */
                }
                visited[i] = true;
                stack.emplace_back(i);
                // 第二层
                for(auto &vec_it1 : graph[i])
                {
                    if(!visited[vec_it1] && vec_it1 > i)
                    {
                        visited[vec_it1] = true;
                        stack.emplace_back(vec_it1);
                        // 第三层
                        for(auto &vec_it2 : graph[vec_it1])
                        {
                            if(!visited[vec_it2] && vec_it2 > i)
                            {
                                visited[vec_it2] = true;
                                stack.emplace_back(vec_it2);
                                graph_cur = graph[vec_it2];
                                it = lower_bound(graph_cur.begin(), graph_cur.end(), i);
                                if(it != graph_cur.end() && *it == i)
                                {
                                    cnt++;
                                    circles[0].path_node.emplace_back(stack);
                                }
                                // 第四层
                                for(auto &vec_it3 : graph[vec_it2])
                                {
                                    if(!visited[vec_it3] && vec_it3 > i)
                                    {
                                        visited[vec_it3] = true;
                                        stack.emplace_back(vec_it3);
                                        graph_cur = graph[vec_it3];
                                        it = lower_bound(graph_cur.begin(), graph_cur.end(), i);
                                        if(it != graph_cur.end() && *it == i)
                                        {
                                            cnt++;
                                            circles[1].path_node.emplace_back(stack);
                                        }
                                        // 第五层
                                        for(auto &vec_it4 : graph[vec_it3])
                                        {
                                            if(!visited[vec_it4] && vec_it4 > i)
                                            {
                                                visited[vec_it4] = true;
                                                stack.emplace_back(vec_it4);
                                                graph_cur = graph[vec_it4];
                                                it = lower_bound(graph_cur.begin(), graph_cur.end(), i);
                                                if(it != graph_cur.end() && *it == i)
                                                {
                                                    cnt++;
                                                    circles[2].path_node.emplace_back(stack);
                                                }
                                                // 第六层
                                                for(auto &vec_it5 : graph[vec_it4])
                                                {
                                                    if(!visited[vec_it5] && vec_it5 > i)
                                                    {
                                                        visited[vec_it5] = true;
                                                        stack.emplace_back(vec_it5);
                                                        graph_cur = graph[vec_it5];
                                                        it = lower_bound(graph_cur.begin(), graph_cur.end(), i);
                                                        if(it != graph_cur.end() && *it == i)
                                                        {
                                                            cnt++;
                                                            circles[3].path_node.emplace_back(stack);
                                                        }
                                                        if(cur_reach[vec_it5] > -1)
                                                        {
                                                            int len = path[i][vec_it5].size();
                                                            auto cur_mid_path = path[i][vec_it5];
                                                            for(int i = cur_reach[vec_it5]; i < len; i++)
                                                            {
                                                                int k = cur_mid_path[i];
                                                                if(visited[k])  continue;
                                                                auto tmp = stack;
                                                                tmp.emplace_back(k);
                                                                circles[4].path_node.emplace_back(tmp);
                                                                cnt++;
                                                            }
                                                        }
                                                        visited[vec_it5] = false;
                                                        stack.pop_back();
                                                    }
                                                }
                                                visited[vec_it4] = false;
                                                stack.pop_back();
                                            }
                                        }
                                        visited[vec_it3] = false;
                                        stack.pop_back();
                                    }
                                }
                                visited[vec_it2] = false;
                                stack.pop_back();
                            }
                        }
                        visited[vec_it1] = false;
                    }
                }
                visited[i] = false;
                stack.pop_back();
                for(int &i : temp)
                    cur_reach[i] = -1;
                temp.clear();
            }
        }
        sort(circles[4].path_node.begin(), circles[4].path_node.end(), less_sort);
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

    void Save_Resules(string &outputFile)
    {
        FILE *fp = fopen(outputFile.c_str(), "wb");
        char buf[1024];

        cout<<cnt<<endl;
        int idx=sprintf(buf, "%d\n", cnt);
        buf[idx]='\0';
        fwrite(buf, idx , sizeof(char), fp );
        for(int i = 0; i < 5; i++) {
            for (auto &path_vec : circles[i].path_node) 
            {
                int len = path_vec.size();
                for(int j = 0; j < len - 1; j++)
                {
                    auto res = id_comma[path_vec[j]];
                    fwrite(res.c_str(), res.size() , sizeof(char), fp);
                }
                auto res = id_end[path_vec[len - 1]];
                fwrite(res.c_str(), res.size() , sizeof(char), fp);
            }
        }
        fclose(fp);
    }
};

int main()
{
    string test_file = "test_data_10000_60000.txt";
    string output_file = "output.txt";
    time_t start, end1, end2, end3, end4;
    solve s;
    start = clock();
    s.CreateGraph(test_file);
    end1 = clock();
    cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Search_2_Path();
    end2 = clock();
    cout<<"Time of Search: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    s.checkcircle_for();
    end3 = clock();
    cout<<"Time of DFS: "<<(double)(end3 - end2) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Save_Resules(output_file);
    end4 = clock();
    cout<<"Time of writing: "<<(double)(end4 - end3) / CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}