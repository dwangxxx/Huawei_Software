#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>

using namespace std;

bool less_sort(const vector<int> &path1, const vector<int> &path2)
{
    if(path1.size() != path2.size())    return path1.size() < path2.size();
    for(int i = 0; i < path1.size(); i++)
    {
        if(path1[i] != path2[i])    return path1[i] < path2[i];
    }
}

class solution{
public:
// 
    vector<int> stack;//首先定义一个stack,
    vector<vector<int>> res;
    vector<vector<int>> graph;
    vector<vector<int>> graph_reverse;


    //bool visited[280000]={false};// visited用于记录该节点是否已经被访问
    //int visited1[280000];//

    // 
    set<int> nodes;//结点是唯一的，因此用set来表示
    int nodes_cnt = 0;

    // 用于记录中间节点路径
    vector<unordered_map<int, vector<int>>> path;

    //
    int cnt = 0;

    vector<string> id_comma;
    vector<string> id_end;

    vector<int> cur_reach;

    void CreateGraph(const string &testfile)
    {
        vector<int> inputs;
        unordered_map<int, int> Hash;//极大提升数据搜索，插入，删除操作时间效率

        FILE* file = fopen(testfile.c_str(), "r");
        int start, end, cost;
        int edges_cnt = 0;
        //根据数据格式从输入流中读取数据，遇到空格和换行时结束
        while(fscanf(file, "%i,%i,%i", &start, &end, &cost)!=EOF){
            inputs.emplace_back(start);
            inputs.emplace_back(end);
            nodes.insert(start);
            nodes.insert(end);
            edges_cnt++;
        }
        //cout<<"Edges : "<<edges_cnt<<endl;

        id_comma.reserve(nodes.size());
        id_end.reserve(nodes.size());

        for(auto &set_it : nodes)
        {
            id_comma.emplace_back(to_string(set_it) + ',');
            id_end.emplace_back(to_string(set_it) + '\n');
            Hash[set_it] = nodes_cnt++;//为每一个节点都赋予数值，从0开始，结点为key,
        }
        
        //cout<<"Nodes : "<<nodes_cnt<<endl;

        //前面已经求得结点数，然后根据这个来建图，采取vector来创建邻接表
        graph = vector<vector<int>>(nodes_cnt);
        //逆邻接表
        graph_reverse = vector<vector<int>>(nodes_cnt);

        
        for(int i = 0; i < inputs.size(); i+=2){
            graph[Hash[inputs[i]]].emplace_back(Hash[inputs[i + 1]]);//邻接表
            graph_reverse[Hash[inputs[i+1]]].emplace_back(Hash[inputs[i]]);//逆邻接表
        }
    }


    void DFS(int k, int head, bool *visited, int *visited1, vector<vector<int>> &res, vector<int> &stack)
    {
        if(!graph[k].empty()){
            if(stack.size() == 6)
            {
                if(cur_reach[k] > -1)
                {
                    int len = path[head][k].size();
                    auto cur_mid_path = path[head][k];
                    for(int i = cur_reach[k]; i < len; i++)
                    {
                        int k = cur_mid_path[i];
                        if(visited[k])  continue;
                        auto temp = stack;
                        temp.emplace_back(k);
                        res.emplace_back(temp);
                    }
                }
                for(int &vec_it : graph[k])
                {
                    if(vec_it == head)  
                    {
                        res.emplace_back(stack);
                        continue;
                    }
                }
                return;
            }
            else 
            {
                for(int &vec_it : graph[k])//遍历当前节点k的后继结点
                {
                    if(vec_it == head){
                        if(stack.size() > 2){
                        res.emplace_back(stack);
                        }
                        continue;
                    }
                    if(visited[vec_it] == true || vec_it < head || visited1[vec_it] != head)
                        continue;
                    visited[vec_it] = true;
                    stack.emplace_back(vec_it);
                    DFS(vec_it, head, visited, visited1, res, stack);
                    stack.pop_back();
                    visited[vec_it] = false;
                }
            }
        }
    }

    //剪枝
    void cut(int k, int head, bool *visited, int *visited1,int depth)
    {
        if(graph[k].size()!=0){
            for(int &vec_it : graph[k])//遍历当前节点k的后继结点
            {
                if(vec_it <head || visited[vec_it] == true)
                    continue;//vec_it == stack[0]说明找到了环，但需要将深度限制在3,7之间
                visited1[vec_it] = head;
                if(depth == 3)  //继续判断深度是否已经达到了3
                    continue;
                visited[vec_it] = true;
                cut(vec_it, head, visited, visited1, depth + 1);
                visited[vec_it] = false;
            }
        }
        
    }
    void cut_reverse(int k, int head, bool *visited, int *visited1,int depth)
    {
        if(graph_reverse[k].size()!=0){
            for(int &vec_it : graph_reverse[k])//遍历当前节点k的后继结点
            {
                if(vec_it < head || visited[vec_it] == true)
                    continue;//vec_it == stack[0]说明找到了环，但需要将深度限制在3,7之间
                visited1[vec_it] = head;
                if(depth == 3)  //继续判断深度是否已经达到了3
                    continue;
                visited[vec_it] = true;
                cut_reverse(vec_it, head, visited, visited1, depth + 1);
                visited[vec_it] = false;
            }
        }
    }

    void CheckCircle()
    {
        cur_reach = vector<int>(nodes_cnt, -1);
        vector<int> temp;
        bool *visited = new bool[nodes_cnt];
        int *visited1 = new int[nodes_cnt];
        memset(visited, false, sizeof(bool) * nodes_cnt);
        memset(visited1, -1, sizeof(int) * nodes_cnt);
        for(int i = 0; i < nodes_cnt; i++)//从第一个节点开始进行dfs
        {
            if(!graph[i].empty())
            {
                stack.emplace_back(i);
                cut(i, i, visited, visited1, 1);
           
                cut_reverse(i, i, visited, visited1, 1);
                for(auto &vec_it : path[i]){
                    int vec = vec_it.first;
                    if(vec > i){
                        auto &val = vec_it.second;
                        int reach_i = lower_bound(val.begin(), val.end(), i) - val.begin();
                        if(reach_i < val.size())    cur_reach[vec] = reach_i;
                        temp.emplace_back(vec);
                    }
                }
                DFS(i, i, visited, visited1, res, stack);
                stack.pop_back();
                for(int &i : temp)
                    cur_reach[i] = -1;
                temp.clear();
            }
        }
        cout << res.size() << endl;
        sort(res.begin(), res.end(), less_sort);
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

    void Save_Resules(const string &outputfile)
    {
        FILE *fp = fopen(outputfile.c_str(), "wb");
        char buf[1024];

        int idx=sprintf(buf, "%d\n", res.size());
        buf[idx]='\0';
        fwrite(buf, idx , sizeof(char), fp );
        for(auto &vec_it : res)
        {
            int len = vec_it.size();
            for(int j = 0; j < len -1; j++)
            {
                auto k = id_comma[vec_it[j]];
                fwrite(k.c_str(), k.size(), sizeof(char), fp);
            }
            auto k = id_end[vec_it[len - 1]];
            fwrite(k.c_str(), k.size(), sizeof(char), fp);
        }
        fclose(fp);
    }
};

int main()
{
    string testfile = "test_data.txt";
    string outputfile = "output.txt";
    int i = 1;
    time_t start, end1, end2, end3, end4;
    start = clock();
    solution s;
    s.CreateGraph(testfile);
    end1 = clock();
    cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    //剪枝+搜索
    s.Search_2_Path();
    end2 = clock();
    cout<<"Time of search: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    s.CheckCircle();
    end3 = clock();
    cout<<"Time of Checking Circle: "<<(double)(end3 - end2) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Save_Resules(outputfile);
    end4 = clock();
    cout<<"Time of writing: "<<(double)(end4 - end3) / CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
