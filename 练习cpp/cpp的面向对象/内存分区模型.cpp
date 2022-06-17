#include <iostream>
using namespace std;

//全局变量
int 全_甲=9, 全_乙=9; 
//全局常量
const int 常_全_甲=9, 常_全_乙=9;

int 程序运行前()
{
    int 甲=9,乙=9; cout<<"本地变量甲地址:"<<&甲<<endl;
    cout<<"本地变量乙地址:"<<&乙<<endl;
    cout<<"全局变量全_甲地址:"<<&全_甲<<endl;
    cout<<"全局变量全_乙地址:"<<&全_乙<<endl;
    //静态变量
    static int 静_甲=9, 静_乙=9;
    cout<<"静态变量静_甲地址:"<<&静_甲<<endl;cout<<"静态变量静_乙地址:"<<&静_乙<<endl;
    cout<<"字符串常量地址:"<<&"你好世界"<<endl;
    cout<<"字符串常量地址:"<<&"你好世界1"<<endl;

    cout<<"全局常量常_全_甲地址:"<<&常_全_甲<<endl;
    cout<<"全局常量常_全_乙地址:"<<&常_全_乙<<endl;

    const int 常_本地_甲=9, 常_本地_乙=9;
    cout<<"本地常量常_本地_甲地址:"<<&常_本地_甲<<endl;
    cout<<"本地常量常_本地_乙地址:"<<&常_本地_乙<<endl;
    system("pause"); return 0;
}

int* 函克()
{
    //int 甲=9; return &甲;
    int* 甲=new int(9); return 甲;
}

int 程序运行后()
{
    int* 针=函克(); cout<<*针<<endl; cout<<*针<<endl; 
    delete 针; cout<<*针<<endl;

    //开辟数组
    int* 数组=new int[9]; 
    for(int 甲;甲<9;甲++){
        数组[甲]=甲+100;
    } 
    for(int 乙=0;乙<10;乙++){
        cout<<数组[乙]<<endl;
    } 
    delete[] 数组; 
    system("pause"); return 0;
}

int main()
{
    //程序运行前();
    程序运行后();
    return 0;
}