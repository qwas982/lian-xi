#include "函数声明.h"

#include <iostream>
using namespace std;

int 输入两个数号(){
    int 甲,乙;
    cout<<"输入提示:"<<endl;
    cin>>甲;cin>>乙;
    return 甲,乙;
}

int 一个加法函数(int 数号1,int 数号2)
{
    int 求和 = 数号1+数号2;
    return 求和;
}

int 调用函数()
{
    cout<<"输入两个数:"<<endl;
    int 甲,乙; cin>>甲; cin>>乙;
    
    int 求和 = 一个加法函数(甲,乙);
    cout<<"求和="<<求和<<endl;
    return 0;
}

int 函数值传递(int 数号1,int 数号2){
    cout<<" 输入提示:"<<endl;
    cin>>数号1;cin>>数号2;
    //void 交换(int 数号1,int 数号2){
        cout<<"交换前:"<<endl; cout<<"数号1="<<数号1<<endl;
        cout<<"数号2="<<数号2<<endl;
        int 临时 = 数号1;数号1=数号2;数号2=临时;
        cout<<"交换后:"<<endl;cout<<"数号1="<<数号1<<endl;
        cout<<"数号2="<<数号2<<endl;
    //}
    //交换(数号1,数号2);
    cout<<"被交换的数号:"<<数号1<<endl;cout<<"被交换的数号:"<<数号2<<endl;
    return 0;
}

//void 函数的常见样式()
//{
    void 无参无返(){int 甲 = 10; cout<<"这是无参无返"<<endl;/*无参无返();*/}
    void 有参无返(int 甲){cout<<"这是有参无返"<<endl;cout<<"甲;"<<甲<<endl;}
    int 无参有返(){cout<<"这是无参有返"<<endl;return 0;}
    int 有参有返(int 甲,int 乙){cout<<"这是有参有返"<<endl;int 求和=甲+乙;return 求和;}

//}

int 函数的声明(int 甲, int 乙);
int 函数的定义(int 甲,int 乙){
    return 甲>乙 ? 甲 : 乙;
    
}

int 最大(){
    输入两个数号();
    cout<<函数的定义(2,4)<<endl;
}



int main()
{
    //一个加法函数(1,2);
    //调用函数();
    //函数值传递(44,4);
    //无参无返(); 无参有返(); 有参无返(1); 有参有返(2,9);
    //函数的定义(2 ,6);
    //最大();
    交换(3,4);

    return 0;
}