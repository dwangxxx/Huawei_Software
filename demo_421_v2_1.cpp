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

string id_comma[280000];
string id_end[280000];

int graph[280000][50] = {{0}};
int g_in[280000][50] = {{0}};
int in_degrees[280000] = {0};
int out_degrees[280000] = {0};

bool visited[280000] = {false};

int isValid[280000];

int circles_count[5] = {0};

class solve
{
public:
    // 用于存放节点编号
    vector<int> nodes;
    int nodes_cnt = 0;

    // 环的数量
    int cnt = 0;

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
        //cout<<"Edges : "<<edges_cnt<<endl;

        // 计算节点映射(node : i)
        int i = 0;
        for(auto &set_it : nodes)
        {
            id_comma[i] = to_string(set_it) + ',';
            id_end[i++] = to_string(set_it) + '\n';
            Hash[set_it] = nodes_cnt++;
        }
        //cout<<"Nodes : "<<nodes_cnt<<endl;
        // 构造图
        for(i = 0; i < edges_cnt * 2; i += 2)
        {
            int start = Hash[inputs[i]], end = Hash[inputs[i + 1]];
            graph[start][out_degrees[start]++] = end;
            g_in[end][in_degrees[end]++] = start;
        }
        memset(isValid, -1, sizeof(isValid));
        // 对领接表中的点进行排序
        for(i = 0; i < nodes_cnt; i++)
        {
            sort(graph[i], graph[i] + out_degrees[i]);
        }
    }

    void DFS_forward_out(int head, int cur, int depth){
    
        for(int i = 0; i < out_degrees[cur]; i++){
            int v = graph[cur][i];
            if (v < head || visited[v]) continue;
            isValid[v] = head;
            if (depth >= 3)   continue;
            visited[v] = true;
            DFS_forward_out(head, v, depth + 1);
            visited[v] = false;
        }
    }

    void DFS_inverse_in(int head, int cur, int depth){
        for (int i = 0; i < in_degrees[cur]; i++){
            int v = g_in[cur][i];
            if (v < head || visited[v]) continue;
            isValid[v] = head;
            if (depth >= 3)   continue;
            visited[v] = true;
            DFS_inverse_in(head, v, depth + 1);
            visited[v] = false;
        }
    }

    void DFS(int head, int cur, int depth, int path[]){
        visited[cur] = true;
        path[depth - 1] = cur;
        for (int i = 0; i < out_degrees[cur]; i++) {
            int v = graph[cur][i];
            // 如果当前点小于head或者其不能到达head, 则不进行DFS
            if (v < head || (isValid[v] != head && isValid[v] != -2)) continue;
            //如果当前节点能直接到达head, 则有环
            if (isValid[v] == -2 && !visited[v] && depth >= 2) {
                path[depth] = v;
                for(int j = 0; j <= depth; j++)
                    ans[depth - 2][circles_count[depth - 2] * (depth + 1) + j] = path[j];
                circles_count[depth - 2]++;
            }
            // 如果深度小于6且没有访问, 则进行DFS
            if (depth < 6 && !visited[v]) {
                DFS(head, v, depth + 1, path);
            }
        }
        visited[cur] = false;
    }

    void CheckCircle() {
        int path[7];
        for (int i = 0; i < nodes_cnt; i++){
            if (out_degrees[i] > 0 && in_degrees[i] > 0) {
            
            // 判断是否i从正向能在三步之内到达v，或者v能在三步之内到达i
            // 正向DFS
            DFS_forward_out(i, i, 1);
            // 反向DFS
            DFS_inverse_in(i, i, 1);
            
            // 筛选大于i的节点
            // g_in[i][j]表示从j到i联通, 用于6推7
            for (int j = 0; j < in_degrees[i]; ++j) {
                if (g_in[i][j] > i)
                    isValid[g_in[i][j]] = -2;
            }
            
            DFS(i, i, 1, path);
    
            for (int j = 0; j < in_degrees[i]; ++j) {
                if (g_in[i][j] > i)
                    isValid[g_in[i][j]] = -1;
                }
            }
        }
    }

    void Save_Results(string &outputFile)
    {
        FILE *fp = fopen(outputFile.c_str(), "wb");
        char buf[1024];

        int cnt = circles_count[0] + circles_count[1] + circles_count[2] + circles_count[3] + circles_count[4];
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
};

int main()
{
    string test_file = "test_data.txt";
    string output_file = "output.txt";
    time_t start, end1, end2, end3, end4;
    solve s;
    start = clock();
    s.CreateGraph(test_file);
    end1 = clock();
    cout<<"Time of creating: "<<(double)(end1 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    s.CheckCircle();
    end2 = clock();
    cout<<"Time of DFS: "<<(double)(end2 - end1) / CLOCKS_PER_SEC<<"s"<<endl;
    s.Save_Results(output_file);
    end3 = clock();
    cout<<"Time of total: "<<(double)(end3 - start) / CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}