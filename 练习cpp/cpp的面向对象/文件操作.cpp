#include <iostream>
#include <fstream>
using namespace std;
//using namespace ios;

void 文本文件写入(){
    ofstream 出文件流;
    出文件流.open("测试.txt",ios::out);
    出文件流<<"姓名:张三"<<" 性别:男"<<" 年龄:11"<<endl;
    出文件流.close();
}

void 文本文件读取(){
    ifstream 入文件流;
    入文件流.open("测试.txt",ios::in);
    if(!入文件流.is_open()){
        cout<<"文件打开失败"<<endl; return;
    }

    //第一种打开法
    /*char 缓冲[1024]={0};  while(入文件流>>缓冲){
        cout<<缓冲<<endl;
    }*/

    //第二种打开
    /*char 缓冲1[1024]={0}; while(入文件流.getline(缓冲1,sizeof(缓冲1))){
        cout<<缓冲1<<endl;
    }*/

    //第三种打开
    /*string 缓冲3; while(getline(入文件流,缓冲3)){
        cout<<缓冲3<<endl;
    }*/

    //第四种打开法
    char 字; while((字=入文件流.get()) != EOF){
        cout<<字;
    } 入文件流.close();
}

//二进制文件的读写
class 个人{
    public: char 成员_名[64]; int 成员_年龄;
};

int 二进制文件写入(){
    ofstream 出文件流("个人.txt",ios::out | ios::binary); //创建
    //出文件流.open("个人.txt",ios::out | ios::binary); //打开
    个人 单个={"张三",11};
    出文件流.write((const char*)&单个, sizeof(单个)); //写
    出文件流.close(); //关闭 
    return 0;
}

int 二进制文件读取(){
    ifstream 入文件流("函数的提高.exe",ios::in | ios::binary);
    if(!入文件流.is_open()){
        cout<<"打开文件失败"<<endl;
    }  //个人 单个; 
    char 二进制流[5];
    入文件流.read((char*)&二进制流,sizeof(二进制流));
    //cout<<"姓名:"<<单个.成员_名<<" 年龄:"<<单个.成员_年龄<<endl; 
    cout<<二进制流<<endl;
    return 0;
}

int main()
{
    //文本文件写入();
    //文本文件读取();
    //二进制文件写入();
    二进制文件读取();

    system("pause");    return 0;
}