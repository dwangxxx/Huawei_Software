#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/time.h>

using namespace std;

const int max_nodes_num = 280000;
const int max_degress = 210;

struct node_and_cost
{
    int node;
    int cost;

    bool operator<(const node_and_cost& a) const
    {
        return node < a.node;
    }
};

char ans3[3 * 500000 * 4];
char ans4[4 * 500000 * 4];
char ans5[5 * 1000000 * 6];
char ans6[6 * 2000000 * 7];
char *ans[5] = {ans3, ans4, ans5, ans6};

char ans7[4][7 * 1500000 * 8];

//string id_comma[280000];
//string id_end[280000];

int num_ans3[4][3 * 250000];
int num_ans4[4][4 * 250000];
int num_ans5[4][5 * 400000];
int num_ans6[4][6 * 700000];
int num_ans7[4][7 * 1500000];

// int *num_ans[5] = {num_ans3, num_ans4, num_ans5, num_ans6, num_ans7};

node_and_cost graph[max_nodes_num][max_degress];
int g_in[max_nodes_num][max_degress] = {{0}};
int in_degrees[max_nodes_num] = {0};
int out_degrees[max_nodes_num] = {0};

int circles_count[5] = {0};
int circles_count_7[4] = {0};

long int byte_size = 0;

int circles_cnt[4][5] = {{0}};

int cnt[4] = {0}; 

template<typename T, int N>
class static_vector
{
private:
    
public:
    T data[N];
    int length;
    static_vector():length(0){}
    inline int size()const
    {
        return length;
    }
    inline void push_back(const T& v)
    {
        data[length++] = v;
    }
    inline void pop_back()
    {
        --length;
    }
    inline T& operator[](int index)
    {
        return data[index];
    }
    inline T& back()
    {
        return data[length-1];
    }
    static_vector& operator=(const static_vector& rhs)
    {
        if(this->data != rhs.data)
        {
            this->length = rhs.length;
            memcpy(this->data, rhs.data, rhs.length*sizeof(T));
        }
        return *this;
    }
    inline void clear()
    {
        length = 0;
    }
    inline bool empty()const
    {
        return 0 == length;
    }
    inline T* begin()
    {
        return data;
    }
    inline T* end()
    {
        return data + length;
    }
    T* erase(T* first, T* last = end())
    {
        length -= (last-first);
        return first;
    }
};

//static_vector<string, 280000> id_comma;
//static_vector<string, 280000> id_end;
string id_comma[max_nodes_num];
string id_end[max_nodes_num];
int nodes_cnt = 0;

vector<int> inputs(max_nodes_num * 2);

int edges_cnt = 0;

class solve
{
public:
    // 用于存放节点编号

    void Input_Mmap(const string &test_file, vector<int> &nodes)
    {
        char *data_ptr = nullptr;
        int fd = open(test_file.c_str(), O_RDONLY);
        long len = lseek(fd, 0, SEEK_END);
        data_ptr = (char *)mmap(nullptr, len, PROT_READ, MAP_SHARED, fd, 0);
        int cnt = 0, tmp = 0;
        bool judge = false;
        for(int i = 0; i < len; ++i)
        {
            if(data_ptr[i] >= '0' && data_ptr[i] <= '9')
            {
                tmp = tmp * 10 + data_ptr[i] - '0';
                judge = false;
            }
            else
            {
                if(!judge)
                {
                    ++cnt;
                }
                else
                {
                    continue;
                }
                judge = true;
                if(cnt > 2)
                {
                    cnt = 0;
                    inputs.push_back(tmp);
                    tmp = 0;
                }
                else
                {
                    nodes.push_back(tmp);
                    inputs.push_back(tmp);
                    tmp = 0;
                }
            }
        }
    }

    void CreateGraph(const string &test_file)
    {
        //vector<int> inputs;
        unordered_map<int, int> Hash;
        // 输入
        vector<int> nodes(280000);
        Input_Mmap(test_file, nodes);
        // static_vector<int, 560000> nodes = inputs;
        sort(nodes.begin(), nodes.end());
        nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
        edges_cnt = inputs.size();
        // 计算节点映射(node : i)
        int i = 0;
        for(auto &set_it : nodes)
        {
            id_comma[i] = to_string(set_it) + ',';
            id_end[i++] = to_string(set_it) + '\n';
            Hash[set_it] = nodes_cnt++;
        }

        cout<<"Nodes : "<<nodes_cnt<<endl;
        cout<<"Edges : "<<edges_cnt<<endl;
        // 构造图
        for(i = 0; i < edges_cnt; i += 3)
        {
            int start = Hash[inputs[i]], end = Hash[inputs[i + 1]], cost = inputs[i + 2];
            graph[start][out_degrees[start]].node = end;
            graph[start][out_degrees[start]++].cost = cost;
            g_in[end][in_degrees[end]++] = start;
        }

        // 对领接表中的点进行排序
        for(i = 0; i < nodes_cnt; i++)
        {
            sort(graph[i], graph[i] + out_degrees[i]);
        }
    }

    static bool check(int pre, int post)
    {
        uint32_t min_val = pre * 0.2;
        // uint32_t max_val = min((uint64_t)pre * 3, (uint64_t)numeric_limits<uint32_t>::max());
        uint32_t max_val = pre * 3;
        if(post < min_val || post > max_val)    return false;
        return true;
    }

    static void DFS_Inverse_In(int head, int cur, int depth, int thread_id, int *reachable, bool *visited){
        for (int i = 0; i < in_degrees[cur]; i++){
            int v = g_in[cur][i];
            if (v < head || visited[v]) continue;
            reachable[v] = head;
            if (depth >= 3)   continue;
            visited[v] = true;
            DFS_Inverse_In(head, v, depth + 1, thread_id, reachable, visited);
            visited[v] = false;
        }
    }

    static void DFS(int head, int cur, int* cur_path, int* cur_cost, int depth, int thread_id, int *reachable, bool *visited)
    {
        visited[cur] = true;
        cur_path[depth - 1] = cur;
        if(depth > 1)   cur_cost[depth - 2] = graph[cur_path[depth - 2]][cur_path[depth - 1]].cost;
        for(int i = 0; i < out_degrees[cur]; i++)
        {
            cout<<thread_id<<" "<<depth<<endl;
            node_and_cost v = graph[cur][i];
            // 如果当前点小于head或者其不能到达head, 则不进行DFS
            if (v.node < head || (depth > 3 && reachable[v.node] != head && reachable[v.node] != -2)) continue;
            // 如果当前节点能直接到达head, 则有环
            else if (reachable[v.node] == -2 && !visited[v.node] && depth >= 2) 
            {
                cur_path[depth] = v.node;
                cur_cost[depth - 1] = graph[cur_path[depth - 1]][cur_path[depth]].cost;
                if(!check(cur_cost[depth - 2], cur_cost[depth - 1]))    continue;
                if(depth == 6)
                {
                    for(int j = 0; j <= depth; j++)
                        num_ans7[thread_id][circles_cnt[thread_id][depth - 2] * (depth + 1) + j] = cur_path[j];
                    circles_cnt[thread_id][depth - 2]++;
                    cnt[thread_id]++;
                }
                else if(depth == 5)
                {
                    for(int j = 0; j <= depth; j++)
                        num_ans6[thread_id][circles_cnt[thread_id][depth - 2] * (depth + 1) + j] = cur_path[j];
                    circles_cnt[thread_id][depth - 2]++;
                    cnt[thread_id]++;
                }
                else if(depth == 4)
                {
                    for(int j = 0; j <= depth; j++)
                        num_ans5[thread_id][circles_cnt[thread_id][depth - 2] * (depth + 1) + j] = cur_path[j];
                    circles_cnt[thread_id][depth - 2]++;
                    cnt[thread_id]++;
                }
                else if(depth == 3)
                {
                    for(int j = 0; j <= depth; j++)
                        num_ans4[thread_id][circles_cnt[thread_id][depth - 2] * (depth + 1) + j] = cur_path[j];
                    circles_cnt[thread_id][depth - 2]++;
                    cnt[thread_id]++;
                }
                else
                {
                    for(int j = 0; j <= depth; j++)
                        num_ans3[thread_id][circles_cnt[thread_id][depth - 2] * (depth + 1) + j] = cur_path[j];
                    circles_cnt[thread_id][depth - 2]++;
                    cnt[thread_id]++;
                }
                
            }
            // 如果深度小于6且没有访问, 则进行DFS
            if (depth < 6 && !visited[v.node]) 
            {
                if(depth > 2 && !check(cur_cost[depth - 2], cur_cost[depth - 1]))    continue;
                DFS(head, v.node, cur_path, cur_cost, depth + 1, thread_id, reachable, visited);
            }
        }
        visited[cur] = false;
    }

    static void CheckCircle(int thread_id){
        int reachable[max_nodes_num];
        bool visited[max_nodes_num] = {false};
        memset(reachable, -1, sizeof(reachable));
        int cur_path[7];
        int cur_cost[7];
        cnt[thread_id] = 0;
        for(int i = thread_id; i < nodes_cnt; i += 4){
            if(out_degrees[i] > 0 && in_degrees[i] > 0){
            
            // 判断是否i从正向能在三步之内到达v，或者v能在三步之内到达i
            // 正向DFS
            //DFS_Forward_Out(i, i, 1);
            // 反向DFS
            DFS_Inverse_In(i, i, 1, thread_id, reachable, visited);
            
            // 筛选大于i的节点
            // g_in[i][j]表示从j到i联通, 用于6推7
            for(int j = 0; j < in_degrees[i]; ++j) 
            {
                if(g_in[i][j] > i)
                    reachable[g_in[i][j]] = -2;
            }
            
            DFS(i, i, cur_path, cur_cost, 1, thread_id, reachable, visited);
    
            for(int j = 0; j < in_degrees[i]; ++j)
            {
                if(g_in[i][j] > i)
                    reachable[g_in[i][j]] = -1;
            }
            }
        }
    }

    void DFS_thread()
    {
        thread t1(CheckCircle, 0);
        thread t2(CheckCircle, 1);
        thread t3(CheckCircle, 2);
        thread t4(CheckCircle, 3);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        // cout<<cnt[0]<<" "<<cnt[1]<<" "<<cnt[2]<<" "<<cnt[3]<<endl;
        return;
    }

    void saveChar_3_6(int depth)
    {
        int tmp_size = 0;
        string tmp_str = "";
        int cur_pos[4] = {1, 1, 1, 1};
        if(depth == 2)
        for(int k = 0; k < nodes_cnt; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans3[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans3[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans3[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else break;
            }
        }
        else if(depth == 3)
        for(int k = 0; k < nodes_cnt; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans4[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans4[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans4[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else break;
            }
        }
        else if(depth == 4)
        for(int k = 0; k < nodes_cnt; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans5[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans5[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans5[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else break;
            }
        }
        else if(depth == 5)
        for(int k = 0; k < nodes_cnt; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans6[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans6[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans6[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else break;
            }
        }
        else 
        for(int k = 0; k < nodes_cnt; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans7[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans7[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans7[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else break;
            }
        }
        circles_count[depth - 2] = tmp_size;
    }

    static void saveChar_7(int depth, int thread_id)
    {
        int start, end;
        if(thread_id == 0)
        {
            start = 0;
            end = nodes_cnt / 25;
        }
        else if(thread_id == 1)
        {
            start = nodes_cnt / 25;
            end = start + nodes_cnt / 20;
            if(start % 4 != 0)  start = start - (start % 4) + 4;
        }
        else if(thread_id == 2)
        {
            start = nodes_cnt / 25 + nodes_cnt / 20;
            end = start + (nodes_cnt * 8) / 100;
            if(start % 4 != 0)  start = start - (start % 4) + 4;
        }
        else 
        {
            start = nodes_cnt / 25 + nodes_cnt / 20 + (nodes_cnt * 8) / 100;
            end = nodes_cnt;
            if(start % 4 != 0)  start = start - (start % 4) + 4;
        }
        // cout<<start<<" "<<end<<endl;
        int tmp_size = 0;
        string tmp_str = "";
        int cur_pos[4] = {1, 1, 1, 1};
        int thread_count = 0;
        for(int k = start; k < end; k += 4)
        {
            for(int i = 0; i < 4; i++)
            while(cur_pos[i] <= circles_cnt[i][depth - 2])
            {
                if(num_ans7[i][(depth + 1) * (cur_pos[i] - 1)] == k + i)
                {
                    thread_count++;
                    for(int j = 0; j <= depth; j++)
                    {
                        if(j != depth)
                        {
                            tmp_str = id_comma[num_ans7[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans7[thread_id] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                        else
                        {
                            tmp_str = id_end[num_ans7[i][j + (depth + 1) * (cur_pos[i] - 1)]];
                            strcpy(ans7[thread_id] + tmp_size, tmp_str.c_str());
                            tmp_size += tmp_str.length();
                        }
                    }
                    cur_pos[i]++;
                }
                else if(num_ans7[i][(depth + 1) * (cur_pos[i] - 1)] < k + i)
                {
                    cur_pos[i]++;
                }
                else break;
            }
        }
        //cout<<thread_id<<" "<<thread_count<<endl;
        circles_count_7[thread_id] += tmp_size;
    }

    static void Save_Char(int thread_id)
    {
        if(thread_id == 0)
        {
            saveChar_7(6, 0);
        }
        else if(thread_id == 1)
        {
            saveChar_7(6, 1);
        }
        else if(thread_id == 2)
        {
            saveChar_7(6, 2);
        }
        else if(thread_id == 3)
        {
            saveChar_7(6, 3);
        }
    }

    void Save_Results(const string &output_file)
    {
        thread t1(Save_Char, 0);
        thread t2(Save_Char, 1);
        thread t3(Save_Char, 2);
        thread t4(Save_Char, 3);
        //thread t3(Save_Char, 2, circles_count_7);
        saveChar_3_6(2);
        saveChar_3_6(3);
        saveChar_3_6(4);
        saveChar_3_6(5);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        //t3.join();
        for(int i = 0; i < 4; ++i)
        {
            byte_size += circles_count[i];
            byte_size += circles_count_7[i];
        }

        string result = to_string(cnt[0] + cnt[1] + cnt[2] + cnt[3]);
        result.append("\n");

        int fd = open(output_file.c_str(), O_RDWR | O_CREAT, 0666);

        int size_of_new = (result.length() + byte_size) * sizeof(char);
        int status = ftruncate(fd, size_of_new);
        char *addr = (char *)mmap(NULL, size_of_new, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        memcpy(addr, result.c_str(), (result.size()) * sizeof(char));
        int size_of_now = (result.size()) * sizeof(char);

        for (int j = 0; j < 4; ++j)
        {
            memcpy(addr + size_of_now, ans[j], circles_count[j]);
            size_of_now += circles_count[j];
        }
        for (int j = 0; j < 4; ++j)
        {
            memcpy(addr + size_of_now, ans7[j], circles_count_7[j]);
            size_of_now += circles_count_7[j];
        }
        munmap(addr, size_of_new);
    }
};

int main()
{
    string test_file = "test_data_5000.txt";
    string output_file = "output.txt";
    timeval us1, us2, us3;
    solve s;
    gettimeofday(&us1, NULL);
    s.CreateGraph(test_file);
    gettimeofday(&us2, NULL);
    cout<<"Time of reading: "<<(double) (us2.tv_sec - us1.tv_sec) * 1000000 + (us2.tv_usec - us1.tv_usec)<<"us"<<endl;
    s.DFS_thread();
    s.Save_Results(output_file);
    gettimeofday(&us3, NULL);
    cout<<"Time of writing: "<<(double) (us3.tv_sec - us2.tv_sec) * 1000000 + (us3.tv_usec - us2.tv_usec)<<"us"<<endl;
    cout<<cnt[0] + cnt[1] + cnt[2] + cnt[3]<<endl;
    return 0;
}