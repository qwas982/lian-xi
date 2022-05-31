#include <iostream>

using namespace std;

int sizeof关键字()
{
    cout<<"short类型所占内存空间为:"<<sizeof(short)<<"字节"<<endl;
    cout<<"int类型所占内存空间为:"<<sizeof(int)<<"字节"<<endl;
    cout<<"long类型所占内存空间为:"<<sizeof(long)<<"字节"<<endl;
    cout<<"long long类型所占内存空间为:"<<sizeof(long long)<<"字节"<<endl;
    return 0;
}

/*int 实数浮点型()
{
    float 浮1 = 3.14f;
    double 双1 = 3.14;

    cout<<浮1<<endl;
    cout<<双1<<endl;

    cout<<"浮点求大小="<<sizeof(浮1)<<endl;
    cout<<"双浮求大小="<<sizeof(双1)<<endl;

    //科学记数法
    float 浮2 = 3e2;
    cout<<"浮2="<<浮2<<endl;

    float 浮3 = 3e-2;
    cout<<"浮3="<<浮3<<endl;

    return 0;
}*/

/*int 字符型()
{
    char *印刻 = U"甲";
    cout<<印刻<<endl;
    cout<<sizeof(印刻)<<endl;

    cout<<(int*)印刻<<endl;
    印刻 = 97;
    cout<<印刻<<endl;

    return 0;
}*/

int 转义字符()
{
    cout<<"\\"<<endl;
    cout<<"\t你好"<<endl;
    cout<<"\n"<<endl;
    return 0;
}

int 字符串型()
{
    char 串1[] = "你好";
    cout<<串1[1]<<endl;

    //C++风格
    string 串 = "你好C++世界";
    cout<<串<<endl;

    return 0;


}

int 布尔型()
{
    bool 旗 = true;
    cout<<旗<<endl;

    旗 = false;
    cout<<旗<<endl;

    cout<<"布尔的大小"<<sizeof(bool)<<endl;
    return 0;
}

int 数据的输入()
{
    int 甲 = 0;
    cout<<"请输入整型变量:"<<endl;
    cin>>甲;
    cout<<甲<<endl;

    double 乙 = 0;
    cout<<"请输入浮点型变量:"<<endl;
    cin>>乙;
    cout<<乙<<endl;

    char 字 = 0;
    cout<<"请输入字符型变量:"<<endl;
    cin>>字;
    cout<<字<<endl;

    string 串 = 0;
    cout<<"请输入字符串型变量:"<<endl;
    cin>>串;
    cout<<串<<endl;

    bool 布尔 = 0;
    cout<<"请输入布尔型变量:"<<endl;
    cin>>布尔;
    cout<<布尔<<endl;

    return EXIT_SUCCESS;
}

int 算术运算符()
{
    int 甲1 = 10;
    int 乙1 = 3;

    cout<< 甲1+乙1<<endl;
    cout<< 甲1-乙1<<endl;
    cout<< 甲1*乙1<<endl;
    cout<< 甲1/乙1<<endl;

    int 甲2 = 10;
    int 乙2 = 20;
    cout<<甲2/乙2<<endl;
    
    int 甲3 = 10;
    int 乙3 = 0;
    cout << 甲3 / 乙3 <<endl;

    double 双1 = 0.5;
    double 双2 = 0.25;
    cout <<"浮点:"<<双1 /双2<<endl;

    return 0;
}

int 取模余数()
{
    int 甲1 = 10, 丙1 = 3;
    cout << 甲1 % 丙1 << endl;

    int 甲2 = 10, 丙2 = 20;
    cout << 甲2 % 丙2 << endl;

    int 甲3 = 10, 丙3 = 0;
    cout << 甲3 % 丙3 << endl;

    double 双1 = 3.14, 双2 = 1.1;
    //cout << 双1 % 双2 << endl;

    return 0;
}

int 递增()
{
    int 甲 = 10; 甲++; cout<<甲<<endl;
    int 乙 = 10; ++乙; cout<<乙<<endl;
    int 甲2 = 10; int 乙2 = ++甲2 * 10; cout<<乙2<<endl;
    int 甲3 = 10; int 乙3 = 甲3++ * 10; cout<<乙3<<endl;
    return 0;
}

int 赋值运算符()
{
    int 甲 = 10; 甲 = 100; cout<<"甲= "<<甲<<endl;
    甲 = 10; 甲+= 2; cout<<"甲="<<甲<<endl;
    甲 = 10; 甲-=2; cout<<"甲="<<甲<<endl;
    甲 = 10; 甲*=2; cout<<"甲="<<甲<<endl;
    甲 = 10; 甲/=2; cout<<"甲="<<甲<<endl;
    甲 = 10; 甲%=2; cout<<"甲="<<甲<<endl;
    return 0;
}

int 对比运算符()
{
    int 甲 = 10, 乙 = 20; 
    cout<<(甲==乙)<<endl;
    cout<<(甲!=乙)<<endl;
    cout<<(甲<乙)<<endl;
    cout<<(甲>乙)<<endl;
    cout<<(甲<=乙)<<endl;
    cout<<(甲>=乙)<<endl;
    return 0;
}

int 逻辑运算符()
{
    int 甲 = 10; cout<<!甲<<endl; cout<<!!甲<<endl; return 0;
    int 乙 = 10; cout<<(甲&&乙)<<endl; 乙 = 0; cout<<(甲&&乙)<<endl;
    甲 = 0, 乙 = 0; cout<<(甲&&乙)<<endl; return 0;
    甲 = 10, 乙 = 10; cout<<(甲 || 乙)<<endl;
    乙 = 0; cout<<(甲||乙)<<endl;
    甲 = 0; cout<<(甲||乙)<<endl; return 0;
}

int 选择结构单行if语句()
{
    int 分数 = 0; cout<<"请输入一个分数: "<<endl; cin>>分数;
    cout<<"你输入的分数为: "<<分数<<endl;
    if(分数>600){cout<<"我考上了一本大学!!!"<<endl;} return 0;
}

int 选择结构多行if语句()
{
    int 分数 = 0; cout<<"请输入考试分数: "<<endl; cin>>分数;
    if(分数>600){cout<<"我考上了一本大学"<<endl;}else{cout<<"我没考上"<<endl;} return 0;

}

int 选择结构多条件if语句()
{
    int 分数 = 0; cout<<"请输入分数: "<<endl; cin>>分数;
    if(分数>600){cout<<"我考上一本"<<endl;}
    else if(分数>500){cout<<"我考上二本"<<endl;}
    else if(分数>400){cout<<"我考上三本"<<endl;}
    else{cout<<"我没考上本科"<<endl;} return 0;
}

int 选择结构嵌套if语句()
{
    int 分数 = 0; cout<<"输入分数: "<<endl; cin>>分数;
    if(分数>600){cout<<"考上一本"<<endl; if(分数>700){cout<<"考上北大"<<endl;}
    else if(分数>650){cout<<"考上清华"<<endl;}else{cout<<"考上人大"<<endl;}}
    else if(分数>500){cout<<"考上二本"<<endl;}
    else if(分数>400){cout<<"考上三本"<<endl;}else{cout<<"没考上"<<endl;} return 0;
}

int 三目运算符()
{
    int 甲 = 10, 乙 = 20, 丙 = 0; 丙 = 甲>乙 ? 甲 : 乙;
    cout<<"丙= "<<丙<<endl;
    (甲>乙 ? 甲 : 乙) = 200; cout<<"甲= "<<甲<<endl; cout<<"乙= "<<乙<<endl;
    cout<<"丙= "<<丙<<endl; return 0;
}

int 选择结构switch语句()
{
    int 分数 = 0; cout<<"给电影打分"<<endl; cin>>分数;
    switch(分数)
    {
        case 10:
        case 9: cout<<"经典"<<endl; break;
        case 8: cout<<"非常好"<<endl; break;
        case 7:
        case 6: cout<<"一般般"<<endl; break;
        default: cout<<"烂片"<<endl; break;
    }
    return 0;
}

int 循环结构while语句()
{
    int 数号 = 0; while(数号<10){cout<<"数号= "<<数号<<endl;数号++;}
    return 0;
}

int 循环结构do_while语句()
{
    int 数号 = 0; do{cout<<数号<<endl; 数号++;}while(数号<10);
    return 0;
}

int 循环结构for语句()
{
    for(int 甲 = 0; 甲<10; 甲++){cout<<甲<<endl;} return 0;
}

int 嵌套循环()
{
    for(int 甲=0;甲<=9;甲++)
    {for(int 乙=0;乙<=9;乙++)
    {int 丙 = 甲*乙;cout<<甲<<"X"<<乙<<"="<<丙<<" ";}cout<<endl;}
    return 0;
}

int 跳转break语句()
{
    int 甲;
    while(甲<3){if(甲==8){break;}
    cout<<"选择难度"<<endl;cout<<"1-普通"<<endl;cout<<"2-中等"<<endl;
    cout<<"3-困难"<<endl; int 数号 = 0; cin>>数号;
    switch(数号){case 1:cout<<"选择了普通"<<endl;break;
    case 2:cout<<"选择了中等"<<endl;break;
    case 3:cout<<"选择了困难"<<endl;break;}} return 0;
}

int 跳转break语句1()
{
    for(int 甲=0;甲<10;甲++){if(甲==5){break;}cout<<甲<<endl;} return 0;
}

int 跳转break语句2()
{
    for(int 甲=0; 甲<9; 甲++)
    {for(int 乙=0; 乙<9; 乙++)
    {if(甲==5){break;}cout<<"*"<<" ";}cout<<endl;} return 0;
}

int 跳转continue语句()
{
    for(int 甲=0;甲<100;甲++)
    {if(甲%2==0){continue;}cout<<甲<<endl;}return 0;
}

int 跳转goto语句()
{
    cout<<"1"<<endl;goto 旗子; cout<<"2"<<endl;cout<<"3"<<endl;cout<<"4"<<endl;
    旗子: cout<<"5"<<endl; return 0;
}
int main()
{
    //sizeof关键字();
    //实数浮点型();
    //字符型();
    //转义字符();
    //字符串型();
    //布尔型();
    //数据的输入();
    //算术运算符();
    //取模余数();
    //递增();
    //赋值运算符();
    //对比运算符();
    //逻辑运算符();
    /*选择结构单行if语句();*/ //选择结构多行if语句();
    //选择结构多条件if语句();
    //选择结构嵌套if语句();
    //三目运算符();
    //选择结构switch语句();
    //循环结构while语句();
    //循环结构do_while语句();
    //嵌套循环();
    //跳转break语句();
    //跳转break语句1();
    //跳转break语句2();
    //跳转continue语句();
    跳转goto语句();

}

