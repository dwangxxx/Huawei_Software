#include<bits/stdc++.h>
#include<algorithm>
#include<ctime>
#include<thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

char ans3[3 * 500000 * 10];
char ans4[4 * 500000 * 10];
char ans5[5 * 1000000 * 10];
char ans6[6 * 2000000 * 10];
char ans7[7 * 3000000 * 10];

char *ans[5] = {ans3, ans4, ans5, ans6, ans7};


int graph[280000][50] = {{0}};
int edges_cnt = 0;
int g_in[280000][50] = {{0}};

string id_comma[280000];
string id_end[280000];
int in_degrees[280000] = {0};
int out_degrees[280000] = {0};

int circles_count[5] = {0};

long int byte_size = 0;

int circles_cnt = 0;

template<typename T, int N>
class static_vector
{
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
        length -= (last - first);
        return first;
    }
};

static_vector<int, 560000> inputs;

class solve
{
public:
    // 用于存放节点编号
    //vector<int> nodes;

    int nodes_cnt = 0;

    int reachable[280000];
    bool visited[280000] = {false};
    //int inputs[2 * 280000] = {0};

    // 环的数量
    int cnt = 0;

    void Input_Mmap(const string &test_file)
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
                if (!judge)
                {
                    ++cnt;
                }
                else
                {
                    continue;
                }
                judge = true;
                if (cnt % 3 == 0)
                {

                    tmp = 0;
                    continue;
                }
                else
                {
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
        Input_Mmap(test_file);
        static_vector<int, 560000> nodes = inputs;
        sort(nodes.begin(), nodes.end());
        nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
        edges_cnt = inputs.size();
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
        for(i = 0; i < edges_cnt; i += 2)
        {
            int start = Hash[inputs[i]], end = Hash[inputs[i + 1]];
            graph[start][out_degrees[start]++] = end;
            g_in[end][in_degrees[end]++] = start;
        }

        memset(reachable, -1, sizeof(reachable));
        // 对领接表中的点进行排序
        for(i = 0; i < nodes_cnt; ++i)
        {
            sort(graph[i], graph[i] + out_degrees[i]);
        }
    }

    void saveChar(int *path, int depth)
    {
        int tmp_size = circles_count[depth - 2];
        string tmp_str = "";
        for(int i = 0; i <= depth; ++i)
        {
            if(i != depth)
            {
                tmp_str = id_comma[path[i]];
                strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                tmp_size += tmp_str.length();
            }
            else
            {
                tmp_str = id_end[path[i]];
                strcpy(ans[depth - 2] + tmp_size, tmp_str.c_str());
                tmp_size += tmp_str.length();
            }
        }
        circles_count[depth - 2] = tmp_size;
    }

    void DFS_Inverse_In(int head, int cur, int depth){
        for(int i = 0; i < in_degrees[cur]; ++i){
            int v = g_in[cur][i];
            if(v < head || visited[v]) continue;
            reachable[v] = head;
            if(depth >= 3)   continue;
            visited[v] = true;
            DFS_Inverse_In(head, v, depth + 1);
            visited[v] = false;
        }
    }

    void DFS_Forward_Out(int head, int cur, int depth){
    
        for(int i = 0; i < out_degrees[cur]; ++i){
            int v = graph[cur][i];
            if(v < head || visited[v]) continue;
            reachable[v] = head;
            if(depth >= 3)   continue;
            visited[v] = true;
            DFS_Forward_Out(head, v, depth + 1);
            visited[v] = false;
        }
    }

    void DFS(int head, int cur, int *cur_path, int depth){
        visited[cur] = true;
        cur_path[depth - 1] = cur;
        for(int i = 0; i < out_degrees[cur]; ++i){
            int v = graph[cur][i];
            // 如果当前点小于head或者其不能到达head, 则不进行DFS
            if(v < head || (reachable[v] != head && reachable[v] != -2)) continue;
            // 如果当前节点能直接到达head, 则有环
            if(reachable[v] == -2 && !visited[v] && depth >= 2) {
                cur_path[depth] = v;
                circles_cnt++;
                saveChar(cur_path, depth);
                /*
                for(int j = 0; j <= depth; j++)
                    ans[depth - 2][circles_count[depth - 2] * (depth + 1) + j] = path[j];
                circles_count[depth - 2]++;
                */
            }
            // 如果深度小于6且没有访问, 则进行DFS
            if (depth < 6 && !visited[v]) {
                DFS(head, v, cur_path, depth + 1);
            }
        }
        visited[cur] = false;
    }

    void CheckCircle() {
        int cur_path[7];
        for(int i = 0; i < nodes_cnt; i++){
            if(out_degrees[i] > 0 && in_degrees[i] > 0){
            
            // 判断是否i从正向能在三步之内到达v，或者v能在三步之内到达i
            // 正向DFS
            DFS_Forward_Out(i, i, 1);
            // 反向DFS
            DFS_Inverse_In(i, i, 1);
            
            // 筛选大于i的节点
            // g_in[i][j]表示从j到i联通, 用于6推7
            for(int j = 0; j < in_degrees[i]; ++j){
                if(g_in[i][j] > i)
                    reachable[g_in[i][j]] = -2;
            }
            
            DFS(i, i, cur_path, 1);
    
            for(int j = 0; j < in_degrees[i]; ++j){
                if(g_in[i][j] > i)
                    reachable[g_in[i][j]] = -1;
                }
            }
        }
    }

    void Save_Results(const string &output_file)
    {
        int saveSize = 0;
        for(int i = 0; i < 5; ++i)
        {
            byte_size += circles_count[i];
        }

        string result = to_string(circles_cnt);
        result.append("\n");

        int fd = open(output_file.c_str(), O_RDWR | O_CREAT, 0666);

        int size_of_new = (result.length() + byte_size) * sizeof(char);
        int status = ftruncate(fd, size_of_new);
        char *addr = (char *)mmap(NULL, size_of_new, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        memcpy(addr, result.c_str(), (result.size()) * sizeof(char));
        int size_of_now = (result.size()) * sizeof(char);

        for (int j = 0; j < 5; ++j)
        {
            memcpy(addr + size_of_now, ans[j], circles_count[j]);
            size_of_now += circles_count[j];
        }
        munmap(addr, size_of_new);
    }
};

int main()
{
    string test_file = "test_data_huawei.txt";
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