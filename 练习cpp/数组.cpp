#include <iostream>
using namespace std;

int 一维数组()
{
    int 分数[10]; 分数[0]=100; 分数[1]=98; 分数[2]=85;
    cout<<分数[0]<<endl;cout<<分数[2]<<endl;cout<<分数[1]<<"---"<<endl;

    int 分数2[10] = {100,90,80,70,};cout<<分数2[0]<<分数2[2]<<endl;
    for(int 甲=0;甲<10;甲++){cout<<分数2[甲]<<endl;}

    int 分数3[] = {20,34,55,62,4}; 
    for(int 乙=0;乙<10;乙++){cout<<分数3[乙]<<endl;}

    return 0;
}

int 数组名_一维数组()
{
    int 数组[10] = {1,2,3,4,5,6,7,8,9,45,}; 
    cout<<"数组占用内存空间:"<<sizeof(数组)<<endl;
    cout<<"单个元素占用内存空间:"<<sizeof(数组[0])<<endl;
    cout<<"数组有几个元素?"<<sizeof(数组)/sizeof(数组[0])<<endl;

    //获取地址
    cout<<"数组首地址:"<<数组<<endl;
    cout<<"第一个元素地址:"<<&数组[0]<<endl;
    cout<<"第3个元素地址:"<<&数组[3]<<endl;

    return 0;
}

int 冒泡排序()
{
    int 数组[9] = {4,2,8,0,5,7,1,3,9};
    for(int 甲=0;甲<0-1;甲++)
    {for(int 乙=0;乙<9-1-甲;乙++)
    {if(数组[乙]>数组[乙+1])
    {int 临时 = 数组[乙];数组[乙]=数组[乙+1];数组[乙+1]=临时;}}}
    for(int 甲=0;甲<9;甲++)
    {cout<<数组[甲]<<endl;} return 0;
}

int 二维数组()
{
    int 数组[2][3];数组[0][1]=1;数组[0][2]=2;数组[0][1]=3;数组[1][1]=4;
    数组[1][2]=5;数组[1][0]=6;
    for(int 甲=0;甲<2;甲++)
    {for(int 乙=0;乙<3;乙++)
    {cout<<数组[甲][乙]<<" "<<endl;}cout<<endl;}

    int 数组2[2][3]={{1,2,3},{4,5,6}};cout<<数组2[1][0]<<endl;

    int 数组3[2][3]={1,2,3,4,5,6};cout<<数组3[2][2]<<endl;

    int 数组4[][3]={1,2,3,4,5,6};cout<<数组4[0][2]<<endl;

    return 0;
}

int  数组名_二维数组()
{
    int 数组[2][3] = {{1,3,4},{5,6,67}};
    cout<<"二维数组大小:"<<sizeof(数组)<<endl;
    cout<<"二维数组一行大小:"<<sizeof(数组[0])<<endl;
    cout<<"二维数组元素大小:"<<sizeof(数组[0][0])<<endl;
    cout<<"二维数组行数:"<<sizeof(数组)/sizeof(数组[0])<<endl;
    cout<<"二维数组列数:"<<sizeof(数组[0]) /sizeof(数组[0][0])<<endl;

    cout<<"二维数组首地址:"<<数组<<endl;
    cout<<"二维数组第一行地址:"<<数组[0]<<endl;
    cout<<"二维数组第二行地址:"<<数组[1]<<endl;
    cout<<"二维数组第一元素地址:"<<&数组[0][0]<<endl;
    cout<<"二维数组第二元素地址:"<<&数组[0][1]<<endl;

    return 0;
}

int 二维数组案例()
{
    int 分数[3][3]={{100,100,100},{90,50,100},{60,70,80}};
    string 名称[3]={"张三","李四","王五"};
    for(int 甲=0;甲<3;甲++){int 求和=0; 
    for(int 乙=0;乙<3;乙++){求和+=分数[甲][乙];}
    cout<<名称[甲]<<"同学总成绩:"<<求和<<endl;} return 0;
}

int main()
{
    //一维数组();
    //数组名_一维数组();
    //冒泡排序();
    //二维数组();
    //数组名_二维数组();
    二维数组案例();

    return 0;
}