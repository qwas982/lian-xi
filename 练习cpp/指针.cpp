#include <iostream>
using namespace std;

//指针; 间接访问内存,
//内存地址用十六进制表示,
//指针是个变量,保存内存地址,

int 指针变量的定义和使用()
{
    int 甲 = 10;  int* 针; //指针也是变量, 指向某种类型的变量,
    针 = &甲; //&是成员访问, 访问 甲 变量的地址,读取并存储到 针 变量里,
    cout<<&甲<<endl; cout<<针<<endl;

    //*号 也是成员访问, 用来访问指针变量内 地址指向的值,
    cout<<"*针= "<<*针<<endl;

    return 0;

}

int 指针所占内存空间()
{
    int 甲 = 10; int* 针; 针 = &甲; cout<<*针<<endl;
    cout<<sizeof(针)<<endl; cout<<sizeof(char*)<<endl;
    cout<<sizeof(float*)<<endl; cout<<sizeof(double*)<<endl;

    return 0;
}

int 空指针和野指针()
{
    //空指针
    int* 针 = NULL; //cout<<*针<<endl; //不能读写系统保护区

    //野指针
    int* 针1 = (int*)0x1100; //cout<<*针1<<endl; //不能读写其它程序所占内存,

    return 0;
}

int 修饰符const()
{
    int 甲 = 9, 乙 = 9; const int* 针1 =&甲; 针1 = &乙; //*针1 = 100;

    int* const 针2 = &甲; *针2 = 100; //针2 = &乙;

    const int* const 针3 = &甲; //针3 = &乙; *针3 = 100; 
    return 0;

}

int 指针和数组()
{
    int 数组[] = {1,2,3,4,5,6,7,8,9}; int* 针 = 数组;
    cout<<"第一个元素:"<<数组[0]<<endl;
    cout<<"指针访问第一个元素:"<<*针<<endl;
    for(int 甲; 甲<9; 甲++)
    {cout<<*针<<针<<endl; 针++;}
    return 0;
}

int 交换1(int 甲,int 乙){
    int 临时 = 甲; 甲 = 乙; 乙 = 临时;
}

int 交换2(int* 针1,int* 针2){
    int 临时 = *针1; *针1 = *针2; *针2 = 临时;
}

int 指针和函数()
{
    int 甲, 乙; cin>>甲; cin>>乙;
    交换1(甲,乙); 交换2(&甲,&乙);
    cout<< "甲:"<<甲<<endl; cout<<"乙:"<<乙<<endl; return 0;
}

void 冒泡排序(int* 数组,int 长度){
    for(int 甲=0;甲<长度-1;甲++)
    {for(int 乙=0;乙<长度-1-甲;乙++)
    {if(数组[乙]>数组[乙+1])
    {int 临时=数组[乙]; 数组[乙]=数组[乙+1];数组[乙+1]=临时;}}}
}

void 打印数组(int 数组[],int 长度){
    for(int 甲=0;甲<长度;甲++)
    {cout<<数组[甲]<<endl;}
}

int 封装一个函数()
{
    int 数组[11]={4,3,6,9,1,2,11,8,7,5}; int 长度=sizeof(数组)/sizeof(int);
    冒泡排序(数组,长度); 打印数组(数组,长度); return 0;
}

int main()
{
    //指针变量的定义和使用();
    //指针所占内存空间();
    //空指针和野指针();
    //修饰符const();
    //指针和数组();
    //指针和函数();
    封装一个函数();

    return 0;
}