//#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <string>
#include <queue>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <chrono>
#include <iostream>

using namespace std;

#define TEST

typedef unsigned int uint32_t;
struct Cycle
{
    int length;
    uint32_t cycle[7];

    Cycle() { length = 0; }

    bool operator<(const Cycle &rhs) const
    {
        if (length != rhs.length)
            return length < rhs.length;
        for (int i = 0; i < length; ++i)
        {
            if (cycle[i] != rhs.cycle[i])
                return cycle[i] < rhs.cycle[i];
        }
    }

    inline void push(uint32_t v)
    {
        cycle[length++] = v;
    }

    inline void pop()
    {
        length--;
    }

    inline void clear(){
        length=0;
    }
};

struct StackPair
{
    int cur;
    int depth;
    bool RETURN; // to fetch return value or not
    StackPair(){cur=0;depth=0;RETURN=false;}
    StackPair(int c,int d,bool r){cur=c;depth=d;RETURN=r;}
};

template <class T>
struct SmallStack{
    T value[7];
    int length=0;

    inline T pop(){return value[--length];}
    inline void push(T v){value[length++]=v;}
    inline bool empty(){return length==0;}
    inline void clear(){length=0;}
};

template <class T>
struct BigStack{
    T *value;
    int length;
    int maxSize;

    BigStack(){length=0;maxSize=100; value = new T[maxSize];}
    ~BigStack(){delete []value;};
    inline T pop(){return value[--length];}
    inline void push(T v){
        if(length>=maxSize)
            doubleSize();
        value[length++]=v;
    }
    inline bool empty(){return length==0;}
    inline void clear(){length=0;}
    void doubleSize(){
        maxSize= maxSize*2;
        T *nvalue = new T[maxSize];
        for (int i=0;i<length;++i){
            nvalue[i] = value[i];
        }
        delete []value;
        value = nvalue;
    }
};

struct PathRecorder
{
    int end;
    unsigned depth;
    bool twoDis2Head;
    PathRecorder()
    {
        end = -1;
        depth = 7;
        twoDis2Head = false;
    }
    PathRecorder(int c, int d)
    {
        end = c;
        depth = d;
        twoDis2Head = false;
    }
};

template<typename T, int N>
class staticVector
{
private:
    
public:
    T data[N];
    int length;
    staticVector():length(0){}
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
    staticVector& operator=(const staticVector& rhs)
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

class staticQueue
{
private:
    static const int MAX_SIZE = 10000;
    int data[MAX_SIZE];
    int _front;
    int _rear;
public:
    staticQueue(): _front(0), _rear(0)
    {
    }
    inline void push(int v)
    {
        data[_rear] = v;
        _rear = (_rear + 1) % MAX_SIZE;
    }
    inline int front()
    {
        return data[_front];
    }
    inline void pop()
    {
        _front = (_front + 1) % MAX_SIZE;
    }
    inline bool empty()const
    {
        return _front == _rear;
    }
};

class Digraph
{
public:
    // vector<vector<int> > G_adjacency; //邻接�?
    // int G_adjacency[280000][50]; //G[u][v]
    staticVector<staticVector<int, 50>, 280000> G_adjacency;
    // int G_inverse[280000][50]; //G[v][u]
    // staticVector<staticVector<int, 50>, 280000> G_inverse;
    // int* AdjSize = nullptr;
    staticVector<uint32_t, 560000> inputPairs;
    // PathRecorder pathRecorder[280000];

    char ans3[3*500000*10];
    char ans4[4*500000*10];
    char ans5[5*1000000*10];
    char ans6[6*2000000*10];
    char ans7[7*3000000*10];

    unordered_map<uint32_t, int> IDmap;
    vector<unordered_map<int, vector<int> > > P2;
    // vector<uint32_t> inputPairs;
    
    vector<int> inDegrees;
    // vector<int> outDegrees;
    vector<string> stringFront;
    vector<string> stringEnd;
    //vector<Cycle> answerALL[5];
    long int byteSize;

    char *ans[5]={ans3,ans4,ans5,ans6,ans7};
    int ansVecSize[5]={0};
    bool *visit = nullptr;
    PathRecorder *pathRecorder = nullptr;
    
    BigStack<StackPair> nodeStack;
    int nodeNums;
    int ansSize;
    

    ~Digraph()
    {
        // if(AdjSize != nullptr)
        //     delete[] AdjSize;
        delete[] pathRecorder;
        delete[] visit;
    }

    void inputMmap(string &testFile)
    {
        char *data = nullptr;
        int fd = open(testFile.c_str(), O_RDONLY);
        long size = lseek(fd, 0, SEEK_END);
        data = (char *)mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        int cnt = 0;
        uint32_t tempNum = 0;
        bool state = false;
        for (int i = 0; i < size; ++i)
        {
            if (data[i] >= '0' && data[i] <= '9')
            {
                tempNum = tempNum * 10 + data[i] - '0';
                state = false;
            }
            else
            {
                if (!state)
                {
                    ++cnt;
                }
                else
                {
                    continue;
                }
                state = true;
                if (cnt % 3 == 0)
                {

                    tempNum = 0;
                    continue;
                }
                else
                {
                    inputPairs.push_back(tempNum);
                    tempNum = 0;
                }
            }
        }
    }

    void loadNodes()
    {

        // vector<uint32_t> tmp = inputPairs;
        staticVector<uint32_t, 560000> tmp = inputPairs;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        nodeNums = tmp.size();

        stringFront.reserve(nodeNums);
        stringEnd.reserve(nodeNums);

        nodeNums = 0;

        for (uint32_t &x : tmp)
        {
            stringFront.push_back(to_string(x) + ',');
            stringEnd.push_back(to_string(x) + '\n');
            IDmap[x] = nodeNums++;
        }

        int sz = inputPairs.size();
        // G_adjacency = vector<vector<int> >(nodeNums);
        P2 = vector<unordered_map<int, vector<int> > >(nodeNums);
        inDegrees = vector<int>(nodeNums, 0);
        // outDegrees = vector<int>(nodeNums, 0);
        // for (int j = 0; j < 5; ++j)
        // {
        //     answerALL[j].reserve(inputPairs.size() * (j + 1));
        // }
        // AdjSize = new int[nodeNums]();
        for (int i = 0; i < sz; i += 2)
        {
            int u = IDmap[inputPairs[i]], v = IDmap[inputPairs[i + 1]];
            G_adjacency[u].push_back(v);
            // G_adjacency[u][AdjSize[u]++] = v;
            // G_inverse[v].push_back(u);
            ++inDegrees[v];
            // ++outDegrees[u];
        }
        pathRecorder = new PathRecorder[nodeNums];
        visit = new bool[nodeNums];
        /*
        for (int i = 0; i < nodeNums; ++i)
        {
            visit[i] = false;
            // for(int j = 0; j < AdjSize[i]; ++j)
            for (int &v : G_adjacency[i])
            {
                // int v = G_adjacency[i][j];
                // for(int k = 0; k < AdjSize[v]; ++k)
                for (int &vv : G_adjacency[v])
                {
                    P2[vv][i].push_back(v);
                    // P2[G_adjacency[v][k]][i].push_back(v);
                    // sort(P2[vv][i].begin(), P2[vv][i].end());
                }
            }
        }
        //排序
        for(auto& m : P2)
        {
            for(auto& vec : m)
            {
                sort(vec.second.begin(), vec.second.end());
            }
        }
        */
        ansSize = 0;
        byteSize=0;
    }

    void clearNodes(vector<int> &degs, bool sortFlag)
    {
        // queue<int> Q;
        staticQueue Q;
        // auto& G = (degs == inDegrees) ? G_adjacency : G_inverse;
        auto& G = G_adjacency;
        for (int i = 0; i < nodeNums; ++i)
        {
            if (0 == degs[i])
                Q.push(i);
        }
        while (!Q.empty())
        {
            int u = Q.front();
            Q.pop();
            // for (int j = 0; j < AdjSize[u]; ++j)
            for (int &v : G[u])
            {
                // int v = G_adjacency[u][j];
                if (0 == --degs[v])
                    Q.push(v);
            }
        }
        // int cnt = 0;

        for (int i = 0; i < nodeNums; ++i)
        {
            if (degs[i] == 0)
            {
                G_adjacency[i].clear();
                // AdjSize[i] = 0;
                // cnt++;
            }
            else if (sortFlag)
            {
                // sort(G_adjacency[i], G_adjacency[i] + AdjSize[i]);
                sort(G_adjacency[i].begin(), G_adjacency[i].end());
            }
        }
        // #ifdef TEST
        //         printf("%d Nodes eliminated in TopoSort\n", cnt);
        // #endif
    }

    //cost a lot
    void saveString(Cycle &path)
    {
        int tempSize=ansVecSize[path.length-3]; //
        //string write
        string temp="";
        for(int i=0;i<path.length;++i)
        {   
            // ans[path.length-3][temp*path.length+i]=path.cycle[i];
            temp+=to_string(IDmap[path.cycle[i]]);//real ID
            if(i!=path.length-1)
            {
                temp+=',';
            }
            else
            {
                temp+='\n';
            }
        }
        for(int i=0;i<temp.length();++i)
        {
            ans[path.length-3][tempSize+i]=temp[i];
        }
        ansVecSize[path.length-3]+=temp.length();
    }

    void saveChar(Cycle &path)
    {
        int tempSize=ansVecSize[path.length-3]; //
        //string write
        string temp="";
        for(int i=0;i<path.length;++i)
        {   
            // ans[path.length-3][temp*path.length+i]=path.cycle[i];
            // temp+=to_string(IDmap[path.cycle[i]]);//real ID
            if(i!=path.length-1)
            {
                // temp+=',';
                temp=stringFront[path.cycle[i]];
                strcpy(ans[path.length-3]+tempSize,temp.c_str());
                tempSize+=temp.length();
            }
            else
            {
                //temp+='\n';
                temp=stringEnd[path.cycle[i]];
                strcpy(ans[path.length-3]+tempSize,temp.c_str());
                tempSize+=temp.length();
            }
        }
        // for(int i=0;i<temp.length();i++)
        // {
        //     ans[path.length-3][tempSize+i]=temp[i];
        // }
        ansVecSize[path.length-3]=tempSize;
    }

    void dfs_stack(int head, int cur,int depth,Cycle &path) {
        int child;
        bool resultRecorder[8];
        for (int i=0;i<8;++i)resultRecorder[i]= false;
        StackPair tmpPair(cur, depth, false);
        nodeStack.push(tmpPair);
        while (!nodeStack.empty()) {
            tmpPair = nodeStack.pop();
            cur = tmpPair.cur;
            depth = tmpPair.depth;
            resultRecorder[0]= false;
            if (tmpPair.RETURN) {
                if(resultRecorder[depth+1]||resultRecorder[depth]) {
                    resultRecorder[depth]= true;
                    resultRecorder[depth+1]=false;
                }else {
                    if (pathRecorder[cur].end != head) {
                        pathRecorder[cur].twoDis2Head = false;
                        pathRecorder[cur].end = head;
                    }
                    pathRecorder[cur].depth = depth;
                }
                visit[cur]= false;
                path.pop();
            } else {
                visit[cur]=true;
                path.push(cur);
                if (depth >= 5 && (pathRecorder[cur].end == head && pathRecorder[cur].twoDis2Head))
                {
                    vector<int> &vtmp = P2[head][cur];
                    for (int &vv : vtmp)
                    {
                        if (vv > head && !visit[vv])
                        {
                            path.push(vv);
                            //saveString(path);
                            saveChar(path);
                            //answerALL[path.length - 3].push_back(path);
                            ++ansSize;
                            path.pop();
                            resultRecorder[0]= true;
                        }
                        else if (vv > head)
                        {
                            resultRecorder[0]= true;
                        }
                    }
                }
                if(depth<6) {
                    tmpPair.RETURN= true;
                    nodeStack.push(tmpPair);
                    // for(int j = 0; j < AdjSize[cur]; ++j)
                    for (int &v : G_adjacency[cur]) 
                    {
                        // int v = G_adjacency[cur][j];
                        if (v == head && depth >= 3 && depth <= 7) {
                            // mtx.lock();
                            //answerALL[path.length - 3].push_back(path);
                            ++ansSize;
                            //saveString(path);
                            saveChar(path);
                            // mtx.unlock();
                            resultRecorder[0] = true;
                        }
                        if (depth < 7 && !visit[v] && v > head &&
                            (pathRecorder[v].end != head || pathRecorder[v].depth > depth + 1)) {
                            nodeStack.push(StackPair(v, depth + 1, false));
                        } else if (visit[v]) {
                            for (int i = 0; i < path.length; ++i) {
                                if (path.cycle[i] == v) {
                                    resultRecorder[0] = true;
                                    break;
                                }
                            }
                        }
                    }
                }else{
                    path.pop();
                    visit[cur]=false;
                }
                resultRecorder[depth]=(resultRecorder[0]||resultRecorder[depth]);
            }
        }
    }

    bool dfs(int head, int cur, int depth, Cycle &path)
    {
        bool found = false;
        visit[cur] = true;
        path.push(cur);
        if (depth >= 5 && (pathRecorder[cur].end == head && pathRecorder[cur].twoDis2Head))
        {
            vector<int> &vtmp = P2[head][cur];
            for (int &vv : vtmp)
            {
                if (vv > head && !visit[vv])
                {
                    path.push(vv);
                    // mtx.lock();
                    //answerALL[path.length - 3].push_back(path);
                    //saveString(path);
                    saveChar(path);
                    ++ansSize;
                    // mtx.unlock();
                    path.pop();
                    if (!found)
                        found = true;
                }
                else if (vv > head)
                {
                    found = true;
                }
            }
        }
        if (depth < 6)
        {
            // for(int j = 0; j < AdjSize[cur]; ++j)
            for (int &v : G_adjacency[cur])
            {
                // int v = G_adjacency[cur][j];
                if (v == head && depth >= 3 && depth <= 7)
                {
                    // mtx.lock();
                    // answerALL[path.length - 3].push_back(path);
                    //saveString(path);
                    saveChar(path);
                    ++ansSize;
                    // mtx.unlock();

                    found = true;
                }
                if (depth < 7 && !visit[v] && v > head &&
                    (pathRecorder[v].end != head || pathRecorder[v].depth > depth + 1))
                {
                    if (dfs(head, v, depth + 1, path))
                    {
                        found = true;
                    }
                }
                else if (visit[v])
                {
                    for (int i = 0; i < path.length; ++i)
                    {
                        if (path.cycle[i] == v)
                        {
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!found)
        {
            auto& pathRecorder_cur = pathRecorder[cur];
            if (pathRecorder_cur.end != head)
                pathRecorder_cur.twoDis2Head = false;
            pathRecorder_cur.end = head;
            pathRecorder_cur.depth = depth;
        }
        visit[cur] = false;
        path.pop();
        return found;
    }

    // void solve_new(int thread_id)
    void solve_new()
    {
        Cycle path;

        for (int i = 0; i < nodeNums; ++i)
        {
            // if(!visit[i] && 0 != AdjSize[i])
            if (!visit[i] && !G_adjacency[i].empty())
            {
                for (auto &tmp : P2[i])
                {
                    int id_tmp = tmp.first;
                    if (id_tmp > i)
                    {
                        pathRecorder[id_tmp].end = i;
                        pathRecorder[id_tmp].depth = 7;
                        pathRecorder[id_tmp].twoDis2Head = true;
                    }
                }
                //dfs_stack(i, i, 1, path);
                dfs(i, i, 1, path);
            }
        }

        //sort(answerALL.begin(), answerALL.end());
    }

    void saveMmap_final(const string &outputFile)
    {
        int saveSize = 0;
        for(int i=0;i<5;++i)
        {
            byteSize+=ansVecSize[i];
        }
        // #ifdef TEST
        //     for(int i=0;i<ansVecSize[0];i++)
        //     {
        //         cout<<ans[0][i];
        //     }
        //     cout<<byteSize<<endl;
        // #endif
        string result = to_string(ansSize);
        result.append("\n");
        // int fd = open(outputFile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0664);
        int fd = open(outputFile.c_str(), O_RDWR | O_CREAT, 0666);
        // int saveSize=result.size()*sizeof(char);
        
        //这句比较关键
        // const int pageSize = int(ansSize * 80);
        //const int pageSize=1024*1024*200;
        int newSize =(result.length()+byteSize)*sizeof(char);
        int status=ftruncate(fd, newSize);
        //int state;
        // auto t=clock();
        // state = ftruncate(fd, newSize); //
        // cout<<clock()-t<<endl;
        char *addr = (char *)mmap(NULL, newSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);
        memcpy(addr, result.c_str(), (result.size()) * sizeof(char));
        int nowSize = (result.size()) * sizeof(char);

        for (int j = 0; j < 5; ++j)
        {
            // memcpy(addr + nowSize, res.c_str(), (res.size()) * sizeof(char));
            // int index=0;
            // for(int i=nowSize;i<nowSize+ansVecSize[j]*sizeof(char);i++)
            // {
            //     addr[i]=ans[j][index];
            //     index++;
            // }
            // nowSize += ansVecSize[j]*sizeof(char);
            // strcpy(addr+nowSize,ans[j])
            memcpy(addr+nowSize,ans[j],ansVecSize[j]);
            nowSize+=ansVecSize[j];
        }

        munmap(addr, newSize);
    }

    // void saveWrite(const string &outputFile)
    // {
    //     FILE *fp = fopen(outputFile.c_str(), "wb");
    //     char buf[1024];
    //     int idx = sprintf(buf, "%d\n", int(ansSize));
    //     buf[idx] = '\0';
    //     fwrite(buf, idx, sizeof(char), fp);
    //     string result;
    //     for (int j = 0; j < 5; j++)
    //     {
    //         sort(answerALL[j].begin(), answerALL[j].end());
    //         for (auto &x : answerALL[j])
    //         {
    //             int sz = x.length;
    //             for (int i = 0; i < sz - 1; i++)
    //             {
    //                 auto res = stringFront[x.cycle[i]];
    //                 fwrite(res.c_str(), res.size(), sizeof(char), fp);
    //             }
    //             auto res1 = stringEnd[x.cycle[sz - 1]];
    //             fwrite(res1.c_str(), res1.size(), sizeof(char), fp);
    //         }
    //     }
    //     fclose(fp);
    // }

};

// int Digraph::G_adjacency[280000][50];

int main()
{
    // string testFile = "data/test_data_BT.txt";
    // string outputFile = "data/myresult_BT.txt";
    string testFile = "test_data.txt";
    string outputFile = "output.txt";
    static Digraph digraph;

#ifdef TEST
    auto timeStartInput = chrono::steady_clock::now();
#endif
    digraph.inputMmap(testFile);
    digraph.loadNodes();
    auto timeEndInput = chrono::steady_clock::now();
    auto timeCostInput = chrono::duration_cast<chrono::microseconds>(timeEndInput - timeStartInput);
    std::cout << "The Input costs:  " << (double)timeCostInput.count() / 1000000 << "  s" << std::endl;
    /*
#ifdef TEST

    auto timeStartCal = chrono::steady_clock::now();
#endif
    digraph.clearNodes(digraph.inDegrees, true);
    // digraph.clearNodes(digraph.outDegrees, true);
    digraph.solve_new();
#ifdef TEST
    printf("cycle nums:  %d\n", digraph.ansSize);
    auto timeEndCal = chrono::steady_clock::now();
    auto timeCostCal = chrono::duration_cast<chrono::microseconds>(timeEndCal - timeStartCal);
    cout << "The gai Algorithm costs:  " << (double)timeCostCal.count() / 1000000 << "  s" << endl;
    auto timeStartOutout = chrono::steady_clock::now();
    cout << "The maxSize of Stack: " << digraph.nodeStack.maxSize<<"\n";
#endif
    digraph.saveMmap_final(outputFile);
    //digraph.saveWrite(outputFile);
#ifdef TEST
    auto timeEndOutput = chrono::steady_clock::now();
    auto timeCostOutput = chrono::duration_cast<chrono::microseconds>(timeEndOutput - timeStartOutout);
    cout << "The Output costs:  " << (double)timeCostOutput.count() / 1000000<< "  s" << endl;
#endif
    */
    exit(0);
    return 0;
}