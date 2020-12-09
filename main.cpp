#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>
#include <queue>
#include <set>
#include <cstring>
#include <sstream>
#include <list>
#include <deque>
#include <tuple>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include <pthread.h>

#define TEST
using namespace std;

/***********************此处为线程池实现代码*********************************/
//封装一个互斥量和条件变量作为状态
typedef struct condition
{
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
}condition_t;

//初始化
int condition_init(condition_t *cond)
{
    int status;
    if((status = pthread_mutex_init(&cond->pmutex, NULL)))
        return status;
    
    if((status = pthread_cond_init(&cond->pcond, NULL)))
        return status;
    
    return 0;
}

//加锁
int condition_lock(condition_t *cond)
{
    return pthread_mutex_lock(&cond->pmutex);
}

//解锁
int condition_unlock(condition_t *cond)
{
    return pthread_mutex_unlock(&cond->pmutex);
}

//等待
int condition_wait(condition_t *cond)
{
    return pthread_cond_wait(&cond->pcond, &cond->pmutex);
}

//固定时间等待
int condition_timedwait(condition_t *cond, const struct timespec *abstime)
{
    return pthread_cond_timedwait(&cond->pcond, &cond->pmutex, abstime);
}

//唤醒一个睡眠线程
int condition_signal(condition_t* cond)
{
    return pthread_cond_signal(&cond->pcond);
}

//唤醒所有睡眠线程
int condition_broadcast(condition_t *cond)
{
    return pthread_cond_broadcast(&cond->pcond);
}

//释放
int condition_destroy(condition_t *cond)
{
    int status;
    if((status = pthread_mutex_destroy(&cond->pmutex)))
        return status;
    
    if((status = pthread_cond_destroy(&cond->pcond)))
        return status;
        
    return 0;
}

//封装线程池中的对象需要执行的任务对象
typedef struct task
{
    void *(*run)(int args1,int args2);  //函数指针，需要执行的任务
    int arg1;              //参数1
	int arg2;              //参数2
    struct task *next;      //任务队列中下一个任务
}task_t;


//下面是线程池结构体
typedef struct threadpool
{
    condition_t ready;    //状态量
    task_t *first;       //任务队列中第一个任务
    task_t *last;        //任务队列中最后一个任务
    int counter;         //线程池中已有线程数
    int idle;            //线程池中kongxi线程数
    int max_threads;     //线程池最大线程数
    int quit;            //是否退出标志
}threadpool_t;

//创建的线程执行
void *thread_routine(void *arg)
{
    struct timespec abstime;
    int timeout;
    printf("thread %d is starting\n", (int)pthread_self());
    threadpool_t *pool = (threadpool_t *)arg;
    while(1)
    {
        timeout = 0;
        //访问线程池之前需要加锁
        condition_lock(&pool->ready);
        //空闲
        pool->idle++;
        //等待队列有任务到来 或者 收到线程池销毁通知
        while(pool->first == NULL && !pool->quit)
        {
            //否则线程阻塞等待
            printf("thread %d is waiting\n", (int)pthread_self());
            //获取从当前时间，并加上等待时间， 设置进程的超时睡眠时间
            clock_gettime(CLOCK_REALTIME, &abstime);  
            abstime.tv_sec += 2;
            int status;
            status = condition_timedwait(&pool->ready, &abstime);  //该函数会解锁，允许其他线程访问，当被唤醒时，加锁
            if(status == ETIMEDOUT)
            {
                printf("thread %d wait timed out\n", (int)pthread_self());
                timeout = 1;
                break;
            }
        }
        
        pool->idle--;
        if(pool->first != NULL)
        {
            //取出等待队列最前的任务，移除任务，并执行任务
            task_t *t = pool->first;
            pool->first = t->next;
            //由于任务执行需要消耗时间，先解锁让其他线程访问线程池
            condition_unlock(&pool->ready);
            //执行任务
            t->run(t->arg1,t->arg2);
            //执行完任务释放内存
            free(t);
            //重新加锁
            condition_lock(&pool->ready);
        }
        
        //退出线程池
        if(pool->quit && pool->first == NULL)
        {
            pool->counter--;//当前工作的线程数-1
            //若线程池中没有线程，通知等待线程（主线程）全部任务已经完成
            if(pool->counter == 0)
            {
                condition_signal(&pool->ready);
            }
            condition_unlock(&pool->ready);
            break;
        }
        //超时，跳出销毁线程
        if(timeout == 1)
        {
            pool->counter--;//当前工作的线程数-1
            condition_unlock(&pool->ready);
            break;
        }
        
        condition_unlock(&pool->ready);
    }
    
    printf("thread %d is exiting\n", (int)pthread_self());
    return NULL;
    
}


//线程池初始化
void threadpool_init(threadpool_t *pool, int threads)
{
    
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last =NULL;
    pool->counter =0;
    pool->idle =0;
    pool->max_threads = threads;
    pool->quit =0;
    
}


//增加一个任务到线程池
void threadpool_add_task(threadpool_t *pool, void *(*run)(int arg1,int arg2), int arg1, int arg2)
{
    //产生一个新的任务
    task_t *newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg1 = arg1;
	newtask->arg2 = arg2;
    newtask->next=NULL;//新加的任务放在队列尾端
    
    //线程池的状态被多个线程共享，操作前需要加锁
    condition_lock(&pool->ready);
    
    if(pool->first == NULL)//第一个任务加入
    {
        pool->first = newtask;
    }        
    else    
    {
        pool->last->next = newtask;
    }
    pool->last = newtask;  //队列尾指向新加入的线程
    
    //线程池中有线程空闲，唤醒
    if(pool->idle > 0)
    {
        condition_signal(&pool->ready);
    }
    //当前线程池中线程个数没有达到设定的最大值，创建一个新的线性
    else if(pool->counter < pool->max_threads)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, pool);
        pool->counter++;
    }
    //结束，访问
    condition_unlock(&pool->ready);
}

//线程池销毁
void threadpool_destroy(threadpool_t *pool)
{
    //如果已经调用销毁，直接返回
    if(pool->quit)
    {
    return;
    }
    //加锁
    condition_lock(&pool->ready);
    //设置销毁标记为1
    pool->quit = 1;
    //线程池中线程个数大于0
    if(pool->counter > 0)
    {
        //对于等待的线程，发送信号唤醒
        if(pool->idle > 0)
        {
            condition_broadcast(&pool->ready);
        }
        //正在执行任务的线程，等待他们结束任务
        while(pool->counter)
        {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}



/***********************以上为线程池实现代码*********************************/


vector<int> arrayStacks[4];		//数组模拟栈

unordered_map<int, bool> inStack[4];

struct classCompare
{
	bool operator()(const string& l, const string& r)
	{
		if (l.size() < r.size())
			return true;
		else if (l.size() > r.size())
			return false;
		for (int i = 0; i < l.size(); i++)
		{
			if (l[i] < r[i])
				return true;
			if (l[i] > r[i])
				return false;
		}
		return false;
	}
};
vector<string> retVec[4];
unordered_map<int, unordered_set<unsigned int>> outMap;		//出
unordered_map<int, unordered_set<unsigned int>> inMap;		//入
unordered_map<int, int> node;		//节点
unordered_set<int> exist;
unordered_set<int> inDoorSet;
void findIndoorHelp(int v)
{
	queue<int> tmpQue;
	set<int> exist2;
	tmpQue.push(v);
	while (!tmpQue.empty())
	{
		exist.insert(tmpQue.front());
		for (auto it = outMap[tmpQue.front()].begin(); it != outMap[tmpQue.front()].end(); it++)
		{
			if (exist2.find(*it) == exist2.end())
			{
				tmpQue.push(*it);
				exist2.insert(*it);
			}
		}
		tmpQue.pop();
		
	}
}
void findIndoor(void)		//找到每一个非连通域的入口
{
	for (auto iter = outMap.begin(); iter != outMap.end(); iter++)
	{
		if (exist.find(iter->first) == exist.end())
		{
			if(inMap[iter->first].size()!=0)	//既有入度也有出度
			{
				findIndoorHelp(iter->first);
				inDoorSet.insert(iter->first);
			}		
		}
	}
}
typedef struct 
{
	int value;
	int indexes;
}dfsArgs;
 


dfsArgs tmpnxArg;
dfsArgs* tmpArgs;
char tmp[30];

//void dfs(void* arg)
void *dfs(void* arg)
{
	int indexes;
	int v;
	tmpArgs=(dfsArgs*)arg;
	indexes=tmpArgs->indexes;
	v=tmpArgs->value;
	// if(indexes==0)
	// 	cout<<"<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>><>>"<<endl;
	//cout<<"index"<<indexes<<"v"<<v<<endl;
	// if(!arrayStacks[indexes].empty())
	// {
	// 	cout<<" v: "<<v<<endl;
	// 	cout<<" index: "<<indexes<<endl;
		
	// 	exit(1);
	// }
	// cout<<" v: "<<v<<endl;
	// cout<<" index: "<<indexes<<endl;
	#ifdef TEST
	cout<<tops[indexes]<<" ";
	
	#endif
	arrayStacks[indexes].push_back(v);
	inStack[indexes][v] = true;
	for (auto iter = outMap[v].begin(); iter != outMap[v].end(); iter++)
	{
		int num=*iter;
		if (inStack[indexes].find(num)==inStack[indexes].end()||inStack[indexes][num]==false)	//没有遍历过
		{
			
			tmpnxArg.indexes=indexes;tmpnxArg.value=num;
			dfs((void*)&tmpnxArg);
			
		}
		else					//找到环了
		{
			#ifdef TEST
			//cout<<"Find !!!!"<<endl;
			#endif
			
			
			int i = arrayStacks[indexes].size()-1;
			for (i; i >=0 && arrayStacks[indexes][i] != num; i--);
			// if(i==-1)
			// 	continue;
			if (arrayStacks[indexes].size() - i >= 3 && arrayStacks[indexes].size() - i <= 7)
			{
				int min=0;
				int index=0;
				vector<int> tmpVec;
				for(int j=i;j<arrayStacks[indexes].size();j++)
				{

					tmpVec.push_back(arrayStacks[indexes][j]);
					if(arrayStacks[indexes][j]<tmpVec[min])
					{
						min=index;
					}
					index++;
				}
				string tmpStr;
				for(int i=min;i<tmpVec.size();i++)
				{
					sprintf(tmp,"%d",tmpVec[i]);
					string tmptmp(tmp);
					tmpStr+=tmptmp+" ";
				}
				for(int i=0;i<min;i++)
				{
					sprintf(tmp,"%d",tmpVec[i]);
					string tmptmp(tmp);
					tmpStr+=tmptmp+" ";
				}
				tmpStr.pop_back();
				//cout<<tmpStr<<endl;
				retVec[indexes].push_back(tmpStr);
			}
			
		}
	}
	arrayStacks[indexes].pop_back();
	inStack[indexes][v] = false;
}
void* stackDfs(int v,int indexes)
{
	stack<int> Stack;
	unordered_map <int,std::unordered_set<unsigned int>::iterator> itMap;

	Stack.push(v);
	while (!Stack.empty())
	{
loopplace:
		int topNum = Stack.top();
		if (!inStack[indexes][topNum])
		{
			arrayStacks[indexes].push_back(topNum);
			inStack[indexes][topNum] = true;
		}
		auto fitstIter = outMap[topNum].begin();
		if(itMap.find(topNum) != itMap.end())	//不是第一次到这个点
		{
			fitstIter=itMap[topNum];
		}
		for (auto iter = fitstIter; iter != outMap[topNum].end(); iter++)
		{
			if (!inStack[indexes][*iter])
			{
				Stack.push(*iter);
				iter++;
				itMap[topNum] = iter;
				goto loopplace;
				
			}
			else
			{
				int num = *iter;
				int i = arrayStacks[indexes].size() - 1;
				for (i; i >= 0 && arrayStacks[indexes][i] != num; i--);
				
				if (arrayStacks[indexes].size() - i >= 3 && arrayStacks[indexes].size() - i <= 7)
				{
					int min = 0;
					int index = 0;
					vector<int> tmpVec;
					for (int j = i; j < arrayStacks[indexes].size(); j++)
					{

						tmpVec.push_back(arrayStacks[indexes][j]);
						if (arrayStacks[indexes][j] < tmpVec[min])
						{
							min = index;
						}
						index++;
					}
					string tmpStr;
					for (int i = min; i < tmpVec.size(); i++)
					{
						sprintf(tmp, "%d", tmpVec[i]);
						string tmptmp(tmp);
						tmpStr += tmptmp + " ";
					}
					for (int i = 0; i < min; i++)
					{
						sprintf(tmp, "%d", tmpVec[i]);
						string tmptmp(tmp);
						tmpStr += tmptmp + " ";
					}
					tmpStr.pop_back();
					//cout<<tmpStr<<endl;
					retVec[indexes].push_back(tmpStr);
				}
			}
		}
		topNum = Stack.top();
		Stack.pop();
		arrayStacks[indexes].pop_back();
		inStack[indexes][topNum] = false;
		itMap.erase(topNum);
	}
}

void readFile(void)
{
    
	auto fd = open("test_data_10000_60000.txt", O_RDONLY,0666);
	if (fd < 0)
    {
        cout << "Read File Failed!!" << endl;
        exit(1);
    }
    auto testFileSize = lseek(fd, 1, SEEK_END);
    auto dontShowWarning = write(fd, "\0", 1); /* 在文件最后添加一个空字符 */
    auto mmap_mem = mmap(NULL, testFileSize, PROT_READ, MAP_PRIVATE, fd, 0);
	char *readPointer = (char *)mmap_mem;
    string input;
    vector<string> temp;
    int val1=0;
    int val2=0;
    int val3=0;
    int flag=0;
	while (1)
	{
		if(*readPointer=='\0')
        {
            #ifdef TEST
            cout<<" val1 "<<val1<<" val2 "<<val2<<" val3 "<<val3<<endl;
            #endif
		    break;
        }
        else if(*readPointer==',')
        {
            flag++;
			
        }
		else if (*readPointer=='\n')
		{   
            #ifdef TEST
            cout<<" val1 "<<val1<<" val2 "<<val2<<" val3 "<<val3<<endl;
            #endif
			outMap[val1].insert(val2);
			inMap[val2].insert(val1);		
			// node[val1]++;
			// node[val2]++;
            flag=0;
            val1=0;
            val2=0;
            val3=0;
            //readPointer++;
		}
        else
        {
            switch(flag)
            {
                case 0:val1=val1*10+*readPointer-'0';break;
                case 1:val2=val2*10+*readPointer-'0';break;
                case 2:val3=val3*10+*readPointer-'0';break;
            }
        }
        
		readPointer++;
		
	}
	munmap(mmap_mem,testFileSize);
	close(fd);
}

void SaveFile(void)
{
	set<string,classCompare> retSet; 
	for(int i=0;i<4;i++)
	{
		for(auto it=retVec[i].begin();it!=retVec[i].end();it++)
		{
			retSet.insert(*it);
		}
	}
	
	int MMAP_BUFF_SIZE =4096*(retSet.size()/100+1);
	auto fd = open("output.txt", O_RDWR|O_CREAT ,0666);
	if (fd < 0)
    {
        cout << "Read File Failed!!" << endl;
        exit(1);
    }
	
    lseek(fd,MMAP_BUFF_SIZE,SEEK_SET);
	write(fd,"1",1);
    auto mmap_mem = mmap(NULL, MMAP_BUFF_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	char *writePointer = (char *)mmap_mem;
	
	char retSize[100];
	sprintf(retSize,"%d",retSet.size());
	
	for(int i=0;retSize[i]!='\0';i++)	
	{
		*writePointer=retSize[i];
		writePointer++;	
	}
	*writePointer='\r';
	writePointer++;
	*writePointer='\n';
	writePointer++;
	for(auto iter=retSet.begin();iter!=retSet.end();iter++)
	{
		for(int i=0;i<iter->size();i++)
		{
			*writePointer=iter->at(i);
			writePointer++;
		}	
		*writePointer='\r';
		writePointer++;
		*writePointer='\n';
		writePointer++;
	}
	// for(char * tmp =(char *)mmap_mem;tmp!=writePointer;tmp++)
	// 	cout<<*tmp;
	*writePointer='\0';
	
	munmap(mmap_mem,MMAP_BUFF_SIZE);
	ftruncate(fd,writePointer-(char*)mmap_mem);
	close(fd);
}


int main()
{
	auto begin=clock();
	readFile();
#ifdef TEST
	
    for(auto iter=outMap.begin();iter!=outMap.end();iter++)
        for(auto it2=iter->second.begin();it2!=iter->second.end();it2++)
            cout<<iter->first<<" -> "<<*it2<<endl;
	cout<<endl<<endl;

#endif

	


	findIndoor();
	
	#ifdef TEST
	cout<<"====================findIndoorSuccess========================="<<endl;
	for(auto iter = inDoorSet.begin();iter!=inDoorSet.end();iter++)
		cout<<*iter<<endl;
	cout<<"IndorSize"<<inDoorSet.size()<<endl;
	#endif
	
	/************直接运行 堆栈板dfs****************/
	// for (auto iter = inDoorSet.begin(); iter != inDoorSet.end(); iter++)
	// {
	// 	stackDfs(*iter,0);
	// }
	/************线程池运行**************************/
	threadpool_t pool;
    //初始化线程池，最多四个线程
    threadpool_init(&pool, 4);
	int i = 0;
	//加入所有任务
	for (auto iter = inDoorSet.begin(); iter != inDoorSet.end(); iter++)
	{
		threadpool_add_task(&pool,stackDfs,*iter,i);
		i++;
		if (i==4)
		{
			i=0;
		}
		
	}
 threadpool_destroy(&pool);


	/**********************直接运行××××××××××××××××××××××××××××××××××××××××*/
	// for(auto iter=inDoorSet.begin();iter!=inDoorSet.end();iter++)
	// {
	// 	dfsArgs tmpArg;
	// 	tmpArg.indexes=0;tmpArg.value=*iter;
	// 	dfs((void*)&tmpArg);
	// }
	/**********************单线程模拟×××××××××××××××××××××××××××××××××××××××*/
	// int flg=0;
	// for(auto iter=inDoorSet.begin();iter!=inDoorSet.end();iter++)
	// {
	// 	flg++;
	// 	dfsArgs tmpArg;
	// 	tmpArg.indexes=flg%4;tmpArg.value=*iter;
	// 	// thread tmpThread(dfs,tmpArg);
	// 	pthread_t tmpThread;
	// 	int ret=pthread_create(&tmpThread,NULL,dfs,(void*)&tmpArg);
	// 	pthread_join(tmpThread, NULL);
	// }
	
	/**********************多线程××××××××××××××××××××××××××××××××××××××××*/
	// auto iter = inDoorSet.begin();
	// int flag=0;
	// pthread_t threadVec[4];
	// while(1)
	// {
	// 	if(iter==inDoorSet.end())
	// 		break;
	// 	int tmpNum=*iter;
	// 	dfsArgs tmpArg;
	// 	tmpArg.value=tmpNum;
		
	// 	switch(flag)
	// 	{
			
	// 		case 0:	tmpArg.indexes=0;pthread_create(&threadVec[0],NULL,dfs,(void*)&tmpArg);break;
	// 		case 1: tmpArg.indexes=1;pthread_create(&threadVec[1],NULL,dfs,(void*)&tmpArg);break;
	// 		case 2: tmpArg.indexes=2;pthread_create(&threadVec[2],NULL,dfs,(void*)&tmpArg);break;
	// 		case 3: tmpArg.indexes=3;pthread_create(&threadVec[3],NULL,dfs,(void*)&tmpArg);
	// 		 		pthread_join(threadVec[0], NULL);
	// 				pthread_join(threadVec[1], NULL);
	// 				pthread_join(threadVec[2], NULL);
	// 				pthread_join(threadVec[3], NULL);
	// 				flag=-1;
	// 				break;
	// 	}
	// 	flag++;
	// 	iter++;
		
	// }
	
	// pthread_join(threadVec[0], NULL);
	// pthread_join(threadVec[1], NULL);
	// pthread_join(threadVec[2], NULL);
	// pthread_join(threadVec[3], NULL);
/************************************************************************************************/
	
#ifdef TEST
    cout<<retSet.size()<<endl;
	for(auto iter=retSet.begin();iter!=retSet.end();iter++)
		{
			cout<<*iter<<endl;
		}
#endif

	
	cout<<"===================================="<<endl;
	cout<<"end!!!"<<endl;
	cout<<retVec[0].size()<<endl;
	SaveFile();
	auto end=clock();
	cout<<(double)(end-begin)/CLOCKS_PER_SEC<<endl;
	return 0;
}