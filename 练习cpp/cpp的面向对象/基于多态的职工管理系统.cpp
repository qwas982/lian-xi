//在文件内分成两部分 工人管理.h  工人管理.cpp
//在工人管理.h内设计管理类, 在工人管理.cpp内将类函数实现,


#include <iostream>
using namespace std;

/*-------------工人.h-------------*/
class 工人{
    public:
        int 成员职工编号, 成员职工部门编号; string 成员职工名;
        virtual void 秀信息()=0;
        virtual string 取部门名()=0;
};

/*----------工人管理.h-------------*/
#include <fstream>
#define 文件名 "员工文件.txt"


class 工人管理{
    public:
        工人管理(); //构造函数
        void 秀菜单(); //功能描述: 人机交互
        void 退出系统(); //退出功能
        void 添加员工();
        void 保存();
        int 取员工个数();
        void 初始化员工();
        void  秀员工();
        void 删除员工();
        int 是否存在(int 身份);
        void 修改员工();
        void 找员工();
        void  排序员工();
        void 清除文件();
        ~工人管理(); //析构函数

    public:
        int  成员员工个数;
        工人** 成员员工数组;
        bool 成员文件是否空;
};

/*--------------员工.h------------*/
class 员工:public 工人{
    public: 员工(int 身份,string 名字,int 部门身份);
    virtual void 秀信息();
    virtual string 取部门名();
};

/*----------------经理.h-------------*/
class 经理:public 工人{
    public: 经理(int 身份,string 名字,int 部门身份);
    virtual void 秀信息();
    virtual string 取部门名();
};

/*------------------经理.cpp------------*/
经理::经理(int 身份,string 名字,int 部门身份){
    this->成员职工编号 =身份;
    this->成员职工名 = 名字;
    this->成员职工部门编号 = 部门身份;
}

void 经理::秀信息(){
    cout<<"职工编号: "<<this->成员职工编号
        <<" 职工姓名: "<<this->成员职工名
        <<" 岗位: "<<this->成员职工部门编号
        <<" 岗位职责:完成老板交给的任务,并下发任务给员工"<<endl;
}

string 经理::取部门名(){
    return string("经理");
}

/*---------------老板.h----------------*/
class 老板:public 工人{
    public: 老板(int 身份,string 名字,int 部门身份);
    virtual void 秀信息();
    virtual string 取部门名();
};

/*---------------老板.cpp-----------------*/
老板::老板(int 身份,string 名字,int 部门身份){
    this->成员职工编号 =身份;
    this->成员职工名 = 名字;
    this->成员职工部门编号 = 部门身份;
}

void 老板::秀信息(){
    cout<<"职工编号: "<<this->成员职工编号
        <<" 职工姓名: "<<this->成员职工名
        <<" 岗位: "<<this->成员职工部门编号
        <<" 岗位职责:管理公司所有事物"<<endl;
}

string 老板::取部门名(){
    return string("老板");
}

/*----------------员工.cpp--------------*/
员工::员工(int 身份,string 名字,int 部门身份){
    this->成员职工编号 = 身份;
    this->成员职工名 = 名字;
    this->成员职工部门编号 = 部门身份;
}

void 员工::秀信息(){
    cout<<"职工编号: "<<this->成员职工编号
        <<" 职工姓名: "<<this->成员职工名
        <<" 岗位: "<<this->成员职工部门编号
        <<" 岗位职责:完成经理交给的任务"<<endl;
}

string 员工::取部门名(){
    return string("员工");
}

/*----------工人管理.cpp-------------*/
//#include "工人管理.h"
工人管理::工人管理(){
    ifstream 入文件流; 入文件流.open(文件名,ios::in);
    if(!入文件流.is_open()){
        cout<<"文件不存在"<<endl;
        this->成员员工个数=0;
        this->成员文件是否空=true;
        this->成员员工数组=NULL;
        入文件流.close();  return; }
    char 字符; 入文件流>>字符;
    if(入文件流.eof()){
        cout<<"文件为空!"<<endl;
        this->成员员工个数=0;
        this->成员文件是否空=true;
        this->成员员工数组=NULL;
        入文件流.close();  return; }
    int 个数=this->取员工个数(); 
    cout<<"员工个数为: "<<个数<<endl; this->成员员工个数 = 个数;

    this->成员员工数组= new 工人 *[this->成员员工个数]; 初始化员工();
    for(int 甲=0;甲<成员员工个数;甲++){
        cout<<"职工号: "<<this->成员员工数组[甲]->成员职工编号
            <<"职工姓名: "<<this->成员员工数组[甲]->成员职工名
            <<"部门编号: "<<this->成员员工数组[甲]->成员职工部门编号<<endl; }
    
}

工人管理::~工人管理(){
    if(this->成员员工数组 != NULL){ delete[] this->成员员工数组; }
}

void 工人管理::秀菜单(){
    cout<<"***************************************"<<endl;
    cout<<"*********欢迎使用职工管理系统************"<<endl;
    cout<<"***********0-退出管理程序***************"<<endl;
    cout<<"***********1-增加职工信息***************"<<endl;
    cout<<"***********2-显示职工信息***************"<<endl;
    cout<<"***********3-删除离职职工***************"<<endl;
    cout<<"***********4-修改职工信息***************"<<endl;
    cout<<"***********5-查找职工信息***************"<<endl;
    cout<<"***********6-按照编号排序***************"<<endl;
    cout<<"***********7-清空所有文档***************"<<endl;
    cout<<"***************************************"<<endl;
    cout<<endl;
}

void 工人管理::退出系统(){
    cout<<"欢迎下次使用"<<endl; system("pause"); exit(0);
}

void 工人管理::添加员工(){
    cout<<"请输入增加员工的数量: "<<endl;
    int 增加数目=0; cin>>增加数目;
    if(增加数目>0){
        int 新大小=this->成员员工个数+增加数目;
        工人** 新空间=new 工人*[新大小];
        if(this->成员员工数组 != NULL){
            for(int 甲=0;甲<this->成员员工个数;甲++){
                新空间[甲]=this->成员员工数组[甲];}}
        for(int 甲=0;甲<增加数目;甲++){
            int 身份; string 名字; int 部门选择;
            cout<<"请输入第 "<<甲+1<<" 个新员工编号: "<<endl; cin>>身份;
            cout<<"请输入第 "<<甲+1<<" 个新员工姓名: "<<endl; cin>>名字;
            cout<<"请输入该员工的岗位: "<<endl;
            cout<<"1普工: "<<endl; cout<<"2经理: "<<endl;
            cout<<"3老板: "<<endl; cin>>部门选择;
            工人* 小工人=NULL; switch(部门选择){
                case 1: 小工人=new 员工(身份,名字,1); break;
                case 2: 小工人=new 经理(身份,名字,2); break;
                case 3: 小工人=new 老板(身份,名字,3); break;
                default: break; }
            新空间[this->成员员工个数+甲]=小工人; }
        delete[] this->成员员工数组;
        this->成员员工数组=新空间;
        this->成员员工个数=新大小;
        this->成员文件是否空=false;
        cout<<"成功添加"<<增加数目<<"名新员工"<<endl; 
        this->保存(); }
    else{ cout<<"输入有误"<<endl; }  system("pause"); system("cls");
}

void 工人管理::保存(){
    ofstream 出文件流; 出文件流.open(文件名, ios::out);
    for(int 甲=0;甲<this->成员员工个数;甲++){
        出文件流<<this->成员员工数组[甲]->成员职工编号<<" "
                <<this->成员员工数组[甲]->成员职工名<<" "
                <<this->成员员工数组[甲]->成员职工部门编号<<endl; }
    出文件流.close();
}

int 工人管理::取员工个数(){
    ifstream 入文件流; 入文件流.open(文件名,ios::in);
    int 身份; string 名字; int 部门身份; int 个数=0;
    while(入文件流>>身份 && 入文件流>>名字 && 入文件流>>部门身份){ 个数++; }
    入文件流.close();  return 个数;
}

void 工人管理::初始化员工(){
    ifstream 入文件流; 入文件流.open(文件名,ios::in);
    int 身份; string 名字; int 部门身份; int 索引=0;
    while(入文件流>>身份 && 入文件流>>名字 && 入文件流>>部门身份){
        工人* 小工人=NULL; if(部门身份==1){
            小工人=new 员工(身份,名字,部门身份); }
        else if(部门身份==2){
            小工人=new 经理(身份,名字,部门身份); }
        else{ 小工人=new 老板(身份,名字,部门身份); }
    this->成员员工数组[索引]=小工人; 索引++; }
}

void 工人管理::秀员工(){
    if(this->成员文件是否空){
        cout<<"文件不存在或记录为空"<<endl; }
    else{ for(int 甲=0;甲<成员员工个数;甲++){ 
        this->成员员工数组[甲]->秀信息(); }}
    system("pause"); system("cls");
}

int 工人管理::是否存在(int 身份){
    int 索引=-1;  for(int 甲=0;甲<this->成员员工个数;甲++){ 
        if(this->成员员工数组[甲]->成员职工编号=身份){ 索引=甲; break; }}
    return 索引;
}

void 工人管理::删除员工(){
    if(this->成员文件是否空){ cout<<"文件不存在或记录为空"<<endl; }
    else{ cout<<"请输入想要删除的员工号; "<<endl;
        int 身份=0; cin>>身份; int 索引=this->是否存在(身份); 
        if(索引 != -1){ for(int 甲=索引;甲<this->成员员工个数-1;甲++){
            this->成员员工数组[甲]=this->成员员工数组[甲+1]; }
        this->成员员工个数--;
        this->保存(); cout<<"删除成功"<<endl; }
        else{ cout<<"删除失败,未找到此员工"<<endl; }} system("pause");
}

void 工人管理::修改员工(){
    if(this->成员文件是否空){ cout<<"文件不存在或记录为空"<<endl; }
    else{ cout<<"请输入想修改的职工编号: "<<endl;
        int 身份; cin>>身份; int 返来=this->是否存在(身份);
        if(返来 != -1){ delete this->成员员工数组[返来];
            int 新身份=0; string 新名字=""; int 部门选择=0;
            cout<<"查到:"<<身份<<"号职工,请输入新职工号: "<<endl;
            cin>>新身份; cout<<"请输入新姓名: "<<endl; cin>>新名字;
            cout<<"请输入岗位: "<<endl; cout<<"1普工"<<endl;
            cout<<"2经理"<<endl; cout<<"3老板"<<endl; cin>>部门选择;
            
            工人* 小工=NULL; switch(部门选择){
                case 1: 小工=new 员工(新身份,新名字,部门选择); break;
                case 2: 小工=new 经理(新身份,新名字,部门选择); break;
                case 3: 小工=new 老板(新身份,新名字,部门选择); break;
                default: break; }
            this->成员员工数组[返来]=小工; cout<<"修改成功"<<endl; this->保存();}
        else{ cout<<"修改失败,查无此人"<<endl; }} system("pause");
}

void 工人管理::找员工(){
    if(this->成员文件是否空){ cout<<"文件不存在或记录为空"<<endl;}
    else{ cout<<"请输入查找的方式: "<<endl;
        cout<<"1按职工编号找"<<endl; cout<<"2按姓名找"<<endl; 
        int 选择=0; cin>>选择;
        if(选择==1){ int 身份; cout<<"你想找的职工编号: "<<endl; 
            cin>>身份; int 返来=是否存在(身份);
            if(返来 != -1){ cout<<"查找成功,此职工信息如下:"<<endl;
                this->成员员工数组[返来]->秀信息();}
            else{ cout<<"查找失败,未有"<<endl;}}
        else if(选择==2){ 
            string 名字; 
            cout<<"请输入查找的姓名:"<<endl;
            cin>>名字; 
            bool 旗子=false;
            for(int 甲=0;甲<成员员工个数;甲++){ 
                if(成员员工数组[甲]->成员职工名==名字){
                cout<<"查找成功,编号为:"
                    <<成员员工数组[甲]->成员职工编号
                    <<" 号的信息如下: "<<endl;
                旗子 = true; 
                this->成员员工数组[甲]->秀信息();}}
        if(旗子==false){ cout<<"查无此人,失败"<<endl;}}
        else{ cout<<"输入选项有误"<<endl;}} system("pause"); 
}

void 工人管理::排序员工(){
    if(this->成员文件是否空){ cout<<"文件不存在或记录为空"<<endl; system("pause"); }
    else{ cout<<"请选择排序方式: "<<endl; 
        cout<<"1按职工号升序"<<endl; cout<<"2按职工号降序"<<endl; 
        int 选择 = 0;  cin>>选择;  
        for(int 甲=0;甲<成员员工个数;甲++){ int 最小或最大=甲;
            for(int 乙= 甲+1;乙<成员员工个数;乙++){ if(选择==1){
                    if(成员员工数组[最小或最大]->成员职工编号 > 成员员工数组[乙]->成员职工编号){
                        最小或最大=乙; }}
                else{ if(成员员工数组[最小或最大]->成员职工编号 < 成员员工数组[乙]->成员职工编号){
                    最小或最大=乙; }}}
            if(甲 != 最小或最大){
                工人* 临时 = 成员员工数组[甲];
                成员员工数组[甲]=成员员工数组[最小或最大];
                成员员工数组[最小或最大]=临时; }}
        cout<<"排序成功, 排序结果为: "<<endl; 
        this->保存(); this->秀员工(); }
}

void 工人管理::清除文件(){
    cout<<"确认要情况?"<<endl; cout<<"1确认"<<"2返回"<<endl;
    int 选择=0; cin>>选择;
    if(选择==1){ ofstream 出文件流(文件名,ios::trunc); 出文件流.close();
        if(this->成员员工数组 != NULL){
            for(int 甲=0;甲<this->成员员工个数;甲++){
                if(this->成员员工数组[甲] != NULL){
                    delete this->成员员工数组[甲]; }}
            this->成员员工个数=0; delete[] this->成员员工数组;
            this->成员员工数组=NULL; this->成员文件是否空=true; }
        cout<<"清除成功"<<endl; } system("pause"); 
}

void 测试函数(){
        工人* 普工=NULL;
        普工=new 员工(1,"张三",1); 普工->秀信息(); delete 普工;
        普工=new 经理(2,"李四",2); 普工->秀信息(); delete 普工;
        普工=new 老板(3,"王五",3); 普工->秀信息(); delete 普工;
    }

/*---------------------主驱动.cpp--------------*/

int 职工管理系统(){
    工人管理 工管; 
    int 选择=0;
    while(true){
        工管.秀菜单(); cout<<"请输入你的选择: "<<endl; cin>>选择;
        switch(选择){
            case 0: 工管.退出系统(); break;
            case 1: 工管.添加员工(); break;
            case 2: 工管.秀员工(); break;
            case 3: 工管.删除员工(); break;
            case 4: 工管.修改员工(); break;
            case 5: 工管.找员工(); break;
            case 6: 工管.排序员工(); break;
            case 7: 工管.清除文件(); break;
            default: system("cls"); break;
        }
    } 

    system("pause"); return 0;
}

int main(){
    职工管理系统();
    //测试函数();
}