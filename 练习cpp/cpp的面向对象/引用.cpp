#include <iostream>
using namespace std;

int 引用的基本使用()
{
    int 甲=9, &乙=甲; cout<<"甲="<<甲<<endl;
    cout<<"乙="<<乙<<endl;  乙=100;
    cout<<"甲="<<甲<<endl; cout<<"乙="<<乙<<endl;
    system("pause"); return 0;
}

int 引用注意事项(){
    int 甲=9,乙=99; //int &丙;
    int &丙=甲; 丙=乙; cout<<"甲="<<甲<<endl;cout<<"乙="<<乙<<endl;
    cout<<"丙="<<丙<<endl; system("pause"); return 0;
}

int 值传递(int 甲,int 乙){
    int 临时=甲; 甲=乙; 乙=临时;
}

int 地址传递(int* 甲, int* 乙){
    int 临时=*甲; *甲=*乙; *乙=临时;
}

int 引用传递(int &甲, int &乙){
    int 临时=甲; 甲=乙; 乙=临时;
}


int 引用做函数参考()
{
    int 甲=9, 乙=99;
    值传递(甲,乙); cout<<"甲="<<甲<<" 乙="<<乙<<endl;
    地址传递(&甲,&乙); cout<<"甲="<<甲<<" 乙="<<乙<<endl;
    引用传递(甲,乙); cout<<"甲="<<甲<<" 乙="<<乙<<endl;
    system("pause"); return 0;
}

int &返回本地变量引用(){
    int 甲=9; return 甲;
}

int &返回静态变量引用(){
    static int 甲 =99; return 甲;
}

int 引用做函数返回值(){
    //int &引起= 返回本地变量引用();
    //cout<<"引起:"<<引起<<endl; cout<<"引起="<<引起<<endl;
    int &引起1=返回静态变量引用();
    cout<<"引起1:"<<引起1<<endl; cout<<"引起1:"<<引起1<<endl;
    返回静态变量引用()=999;
    cout<<"引起1:"<<引起1<<endl; cout<<"引起1:"<<引起1<<endl;
    system("pause"); return 0; 
}

void 函克(int &引子){
    引子 = 99;
}

int 引用的本质(){
    //本质是指针常量
    int 甲=9; int &引起=甲; 引起=99;
    cout<<"甲:"<<甲<<endl; cout<<"引起:"<<引起<<endl;
    函克(甲); return 0;
}

int 秀值(const int &值){
    //值 += 9;
    cout<<值<<endl;
}

int 常量引用(){
    //int &引起=9; 
    const int &引起 = 9;
    //引起 = 99;
    cout<<引起<<endl;
    int 甲 = 9; 秀值(甲); system("pause"); return 0;
}

int main()
{
    //引用的基本使用();
    //引用注意事项();
    //引用做函数参考();
    //引用做函数返回值();
    //引用的本质(); //要不就使用指针 要不就使用引用, 混用必然误操作,
    常量引用();

    return 0;
}