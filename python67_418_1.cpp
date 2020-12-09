#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>

using namespace std;


class solution{
public:
    //vector<vector<int>> results;
    vector<vector<int>> graph;
    vector<unordered_set<int>> reach;

    // 
    vector<int> nodes;//用于存放节点编号
    int nodes_cnt = 0;

    //
    int cnt = 0;

    void CreateGraph()
    {
        vector<int> inputs;
        unordered_map<int, int> Hash;//极大提升数据搜索，插入，删除操作时间效率
        string testfile = "test_data_10000_60000.txt";

        FILE* file = fopen(testfile.c_str(), "r");
        int start, end, cost;
        int edges_cnt = 0;
        //根据数据格式从输入流中读取数据，遇到空格和换行时结束
        while(fscanf(file, "%i,%i,%i", &start, &end, &cost)!=EOF){
            inputs.emplace_back(start);
            inputs.emplace_back(end);
            nodes.emplace_back(start);
            nodes.emplace_back(end);
            edges_cnt++;
        }
        cout<<"Edges : "<<edges_cnt<<endl;

        sort(nodes.begin(), nodes.end());
        nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
        for(auto &set_it : nodes)
            Hash[set_it] = nodes_cnt++;//为每一个节点都赋予数值，从0开始，结点为key,
        
        cout<<"Nodes : "<<nodes_cnt<<endl;

        //前面已经求得结点数，然后根据这个来建图，采取vector来创建邻接表
        graph = vector<vector<int>>(nodes_cnt);
        reach=vector<unordered_set<int>>(nodes_cnt,unordered_set<int> (1000));

        for(int i = 0; i < inputs.size(); i+=2)
            graph[Hash[inputs[i]]].emplace_back(Hash[inputs[i + 1]]);
        // 对邻接表中的点进行排序
        for(auto &vec_it : graph)   sort(vec_it.begin(), vec_it.end());
    }

    //BFS剪枝
    void BFS()
    {
        for(int k = 0; k < nodes_cnt; k++)//从第一个节点开始进行bfs
        {
            if(graph[k].size() != 0)
            {
                vector<int> que;
                que.emplace_back(k);
                for (int i = 0; i < 3; i++)
                {
                    vector<int> que_new;
                    for (int &j : que)//遍历当前que中有的元素
                    {
                        if(graph[j].size() != 0){ //如果当前节点已经在邻接表中
                            for(auto &each_node : graph[j]){ //从小到大的遍历它的后继元素
                                que_new.emplace_back(each_node);
                                if(each_node < k && each_node < j)
                                    reach[each_node].insert(k);
                            }
                        }
                    }
                    que = que_new;
                }
                    
            }
        }
    }
    //DFS
    void DFS(vector<int> path,int v, int depth)
    {
        path.emplace_back(v);
        if(depth>7)
            return;
        int node = path.back(); //取path的最后一个元素
        //cout << path.size() << endl;
        if(graph[node].size()!=0){

            for (auto &each_node :graph[node])
            {
                if(each_node==path[0]){
                    if(depth>2){
                    cnt++;
                    //results[depth - 3].emplace_back(path);
                    }
                }
                else if (each_node>path[0] && find(path.begin(),path.end(),each_node)==path.end())
                {
                    if (depth >3 && !reach[path[0]].count(each_node))// !visited[path[0]].count(each_node)
                        continue;
                    DFS(path,each_node,depth + 1);
                }
                
            }
            

        }
        return;
    }

   
    void CheckCircle()
    {
        time_t start = clock();
        BFS();
        time_t end1 = clock();
        cout<<"Time of BFS: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
        for (int k = 0; k < nodes_cnt; k++)
        {
            vector<int> stack;
            DFS(stack,k,1);
            //stack.clear();
        }
        time_t end2 = clock();
        cout<<"Time of DFS: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
        cout << cnt << endl;

    }
};
int main()
{
    time_t start, end1, end2;
    start = clock();
    solution s;
    s.CreateGraph();
    end1 = clock();
    cout<<"Time of creating graph: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    //剪枝+搜索
    s.CheckCircle();
    //end2 = clock();
    //cout<<"Time of Checking Circle: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    system("pause");
    return 0;
}
