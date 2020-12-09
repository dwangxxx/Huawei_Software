#include<iostream>
#include<thread>

using namespace std;

void function_1()
{
    cout<<"Function1()"<<endl;
}

int main()
{
    thread t1(function_1);
    cout<<"test"<<endl;
    cout<<"main"<<endl;
    t1.join();
    return 0;
}