#include <iostream>
using namespace std;

class 成员变量和函数分开存储{
    public:
        int 成员甲;
        static int 成员乙;
        成员变量和函数分开存储(){
            成员甲=0;
        }
        void 函克(){
            cout<<"成员甲:"<<this->成员甲<<endl;
        }
        static void 静函克(){}
};

int 对象模型和this指针(){
    cout<<sizeof(成员变量和函数分开存储)<<endl; system("pause"); return 0;
}

class this指针{
    public:
        int 年龄;

        this指针(int 年龄){
            this->年龄=年龄;
        }
        this指针 &针与针(this指针 针){
            this->年龄 += 针.年龄; return *this;
        }
};

void this指针概念测试01(){
    this指针 针1(9); cout<<"针1.年龄:"<<针1.年龄<<endl;
    this指针 针2(9); 针2.针与针(针1).针与针(针1).针与针(针1);
    cout<<"针2.年龄"<<针2.年龄<<endl;
}

int this指针概念(){
    this指针概念测试01(); system("pause"); return 0;
}

class 空指针访问{
    public:
        int 成员年龄;
        void 秀类名(){
            cout<<"我是 空指针访问 类"<<endl;
        }
        void 秀空指针访问(){
            if(this==NULL){ return; } cout<<成员年龄<<endl;
        }
};

void 空指针访问测试01(){
    空指针访问* 针=NULL; 针->秀类名(); 针->秀空指针访问();

}

int 空指针访问成员函数(){
    空指针访问测试01(); system("pause"); return 0;
}

class 常函数常对象{
    public:
        int 成员_甲; mutable int 成员_乙;
        常函数常对象(){ 成员_甲=0; 成员_乙=0;}
        void 秀常函数常对象() const{
            /*const type* const 指针;
            this = NULL;
            this->成员甲 = 99;*/
            this->成员_乙=99;
        }
        void 我的函克() const{
            //成员甲=9999;
        }
};

int 常函数常对象测试01(){
    const  常函数常对象 常量对象;
    cout<<常量对象.成员_甲<<endl;
    //常量对象.成员甲=99;
    常量对象.成员_乙=99;
    常量对象.我的函克(); return 0;
}

int const修饰成员函数(){
    常函数常对象测试01();
    system("pause"); return 0;
}

class 建造{
    friend void 好家伙(建造* 小建造);
    private:
        string 成员_卧室;
    public:
        string 成员_客厅;
        建造(){
            this->成员_卧室="卧室";
            this->成员_客厅="客厅";
        }
};

void 好家伙(建造* 小建造){
    cout<<"好友正在访问:"<<小建造->成员_卧室<<endl;
    cout<<"好友正在访问:"<<小建造->成员_客厅<<endl;
}

class 建造1;

class 好家伙1{
    private:
        建造1 *小建造1;
    public:
        好家伙1();
        void 访问();
};

class 建造1{
    friend class 好家伙1;
    private: string 成员_卧室;
    public:
        string 成员_客厅;
        建造1();
};

建造1::建造1(){
    this->成员_客厅="客厅"; this->成员_卧室="卧室";
}

好家伙1::好家伙1(){
    小建造1=new 建造1;
}

void 好家伙1::访问(){
    cout<<"好友正在访问:"<<小建造1->成员_卧室<<endl;
    cout<<"好友正在访问:"<<小建造1->成员_客厅<<endl;
}

int 全局函数做友元(){
    建造 建; 好家伙(&建);  system("pause"); return 0;
}

int 类做友元(){
    好家伙1 家伙; 家伙.访问(); system("pause"); return 0;
}

class 建造2;
class 好家伙2{
    private: 建造2* 小建造2;
    public:
        好家伙2();
        void 访问(); void 访问1();
};

class 建造2{
    private: string 成员卧室;
    public:
        string 成员客厅;
        建造2();
    friend void 好家伙2::访问();
};

建造2::建造2(){
    this->成员卧室="卧室"; this->成员客厅="客厅";
}

好家伙2::好家伙2(){
    小建造2 = new 建造2;
}

void 好家伙2::访问(){
    cout<<"好友正在访问:"<<小建造2->成员卧室<<endl;
    cout<<"好友正在访问:"<<小建造2->成员客厅<<endl;
}

void 好家伙2::访问1(){
    //cout<<"好友正在访问:"<<小建造2->成员卧室<<endl;
    cout<<"好友正在访问:"<<小建造2->成员客厅<<endl;
}

int 成员函数做友元(){
    好家伙2 家伙; 家伙.访问(); 家伙.访问1(); system("pause"); return 0;
}

int 友元(){
    全局函数做友元();
    类做友元();
    成员函数做友元();

    return 0;
}

class 加重载{
    public:
        int 成员甲, 成员乙;
        加重载(){};
        加重载(int 甲,int 乙){
            this->成员甲 = 甲; this->成员乙 = 乙;
        }
        加重载 operator+(const 加重载 &针){
            加重载 临时;
            临时.成员甲 = this->成员甲 + 针.成员甲;
            临时.成员乙 = this->成员乙 + 针.成员乙; return 临时;
        }
};

加重载 operator+(const 加重载 &针1, int 值){
    加重载 临时; 临时.成员甲 = 针1.成员甲 + 值;
    临时.成员乙 = 针1.成员乙 + 值; return 临时;
}

//全局函数实现+号运算符重载,
加重载 operator+(const 加重载 &针2,const 加重载 &针3){
    加重载 临时(0,0);
    临时.成员甲=针2.成员甲 + 针3.成员甲;
    临时.成员乙 = 针2.成员乙 + 针3.成员乙;  return 临时;
}

int 加号运算符重载(){
    加重载 针1(9,9); 加重载 针2(99,99);
    加重载 针3=针1+针2; cout<<针3.成员甲<<针3.成员乙<<endl;
    加重载 针4=针3+9; cout<<针4.成员甲<<针4.成员乙<<endl; 
    return 0;
}

class 左移重载{
    private: int 成员甲; int 成员乙;
    public:
        左移重载(int 甲,int 乙){
            this->成员甲=甲; this->成员乙=乙;
        }
    friend ostream &operator<<(ostream &出,左移重载 &针);
};

ostream &operator<<(ostream &出,左移重载 &针){
    出<<"甲:"<<针.成员甲<<"乙:"<<针.成员乙; return 出;
}

int 左移运算符重载(){
    左移重载 针1(9,99);
    cout<<针1<<"你好世界"<<endl;  system("pause"); return 0;
}

class 我的整数{
    private: int 成员数号;
    public:
        我的整数(){ 成员数号=0; }
        我的整数 &operator++(){ 成员数号++; return* this;}
        我的整数 operator++(int){
            我的整数 临时=*this; 成员数号++; return 临时; }
    friend ostream &operator<<(ostream &出,我的整数 我整型);
};

ostream &operator<<(ostream &出,我的整数 我整型){
    出<<我整型.成员数号; return 出;
}

int 递增运算符重载(){
    我的整数 我整型; cout<<++我整型<<endl;  cout<<我整型<<endl; 
    cout<<我整型++<<endl;  cout<<我整型<<endl;
    system("pause"); return 0;
}

class 赋值重载{
    public:
        int* 成员年龄;
        赋值重载(int 年龄){
            成员年龄 = new int(年龄);
        }
        赋值重载 &operator=(赋值重载 &针){
            if(成员年龄 != NULL){
                delete 成员年龄; 成员年龄 = NULL;
            }
            成员年龄 = new int(*针.成员年龄); return* this;
        }
        ~赋值重载(){
            if(成员年龄 != NULL){
                delete 成员年龄; 成员年龄 = NULL;
            }
        }
};

int 赋值运算符重载(){
    赋值重载 针1(9), 针2(99), 针3(999); 针3 = 针2 = 针1;
    cout<<"针1的年龄:"<<*针1.成员年龄<<endl;
    cout<<"针2的年龄:"<<*针2.成员年龄<<endl;
    cout<<"针3的年龄:"<<*针3.成员年龄<<endl; system("pause"); return 0;
}

class 关系重载{
    public:
        string 成员名; int 成员年龄;
        关系重载(string 名字,int 年龄){
            this->成员名=名字; this->成员年龄=年龄;
        }
        bool operator==(关系重载& 针){
            if(this->成员名==针.成员名 && this->成员年龄==针.成员年龄){
                return true; }
            else{ return false; }
        }
        bool operator!=(关系重载& 针){
            if(this->成员名==针.成员名 && this->成员年龄==针.成员年龄){
                return false; }
            else{ return true; }
        }
};

int 关系运算符重载(){
    关系重载 甲("关晓彤",99); 关系重载 乙("关晓彤",99);
    if(甲==乙){ cout<<"甲与乙相等"<<endl; }
    else{ cout<<"甲与乙不想等"<<endl; }
    if(甲 != 乙){ cout<<"甲与乙不想等"<<endl; }
    else{ cout<<"甲与乙相等"<<endl; } return 0;
}

class 我的打印{
    public: void operator()(string 文本){
        cout<<文本<<endl; }
};

void 重载括号操作符(){
    我的打印 我的函克; 我的函克("你好世界");
}

class 我的加{
    public: int operator()(int 值1,int 值2){ return 值1+值2; }
};

void 重载括号操作符1(){
    我的加 加法; int 返来=加法(9,9); cout<<"返来:"<<返来<<endl;
    cout<<"我的加()(99,99):"<<我的加()(99,99)<<endl;
}

int 函数调用运算符重载(){
    重载括号操作符(); 重载括号操作符1(); return 0;
}

int 运算符重载(){
    //对已有运算符重新定义,
    //加号运算符重载();
    //左移运算符重载();
    //递增运算符重载();
    //赋值运算符重载();
    //关系运算符重载();
    函数调用运算符重载();

    system("pause"); return 0;
}

class Java{
    public: void 头部(){ cout<<"首页,登录,注册,(公有头部)"<<endl; }
    void 脚部(){ cout<<"帮助,交流,站内(公有底部)"<<endl; }
    void 左边(){ cout<<"Java,Python,C++(公有分类列表)"<<endl; }
    void 上下文(){ cout<<"Java课程视频"<<endl; }
};

class Python{
    public: void 头部(){ cout<<"首页,登录,注册,(公有头部)"<<endl; }
    void 脚部(){ cout<<"帮助,交流,站内(公有底部)"<<endl; }
    void 左边(){ cout<<"Java,Python,C++(公有分类列表)"<<endl; }
    void 上下文(){ cout<<"Python课程视频"<<endl; }
};

class cpp{
    public: void 头部(){ cout<<"首页,登录,注册,(公有头部)"<<endl; }
    void 脚部(){ cout<<"帮助,交流,站内(公有底部)"<<endl; }
    void 左边(){ cout<<"Java,Python,C++(公有分类列表)"<<endl; }
    void 上下文(){ cout<<"C++课程视频"<<endl; }
};

int 普通实现(){
    cout<<"Java下载视频页面:"<<endl;
    Java 加瓦; 加瓦.头部(); 加瓦.脚部(); 加瓦.左边(); 加瓦.上下文();
    cout<<"___________________"<<endl;

    cout<<"Python下载视频页面:"<<endl;
    Python 蟒蛇; 蟒蛇.头部(); 蟒蛇.脚部(); 蟒蛇.左边(); 蟒蛇.上下文();
    cout<<"____________________"<<endl;

    cout<<"c++下载视频页面:"<<endl;
    cpp 西加加; 西加加.上下文(); 西加加.脚部(); 西加加.左边(); 西加加.头部();
    cout<<"____________________"<<endl; return 0;
}

class 基本页面{
    public: void 头部(){ cout<<"登录,注册,首页,公有头部"<<endl; }
    void 脚部(){cout<<"中心,合作,地图,公有底部"<<endl; }
    void 左部(){ cout<<"C++,Java,Python,分类列"<<endl; }
};

class 加瓦:public 基本页面{
    public: void 上下文(){ cout<<"加瓦视频"<<endl; }
};

class 派森:public 基本页面{
    public: void 上下文(){ cout<<"派森视频"<<endl; }
};

class 西加加:public 基本页面{
    public: void 上下文(){ cout<<"西加加视频"<<endl;}
};

int 继承实现(){
    cout<<"加瓦下载页面如下:"<<endl;
    加瓦 加瓦; 加瓦.上下文(); 加瓦.脚部(); 加瓦.左部(); 加瓦.头部();
    cout<<"XXXXXXXXXXXXXXXXXXX"<<endl;

    cout<<"派森下载页面如下:"<<endl;
    派森 派森; 派森.上下文(); 派森.脚部(); 派森.左部(); 派森.头部();
    cout<<"XXXXXXXXXXXXXXXXXXXXXX"<<endl;

    cout<<"西加加下载页面如下:"<<endl;
    西加加 西加; 西加.上下文(); 西加.脚部(); 西加.左部(); 西加.头部();
    cout<<"XXXXXXXXXXXXXXXXXXXXXX"<<endl; return 0;
}

int 继承的基本语法(){
    普通实现(); 继承实现(); return 0;
}

class 一个基类或父类{
    public: int 成员甲;  protected: int 成员乙;  private: int 成员丙;
};

class 一个派生或子类:public 一个基类或父类{
    public: void 函克(){ 成员甲; 成员乙; /*成员丙;*/ }
};

void 我的类(){ 一个派生或子类 派生子; 派生子.成员甲;  }

class 一个基类或父类1{ 
    public:int 成员甲; protected:int 成员乙; private: int 成员丙;
};

class 一个派生或子类1:protected 一个基类或父类1{
    public: void 函克(){ 成员甲; 成员乙; /*成员丙;*/ }
};

void 我的类2(){ 一个派生或子类1 派生子; /*派生子.成员甲;*/ }

class 一个基类或父类2{
    public: int 成员甲; protected: int 成员乙; private: int 成员丙;
};

class 一个派生或子类2:private 一个基类或父类2{
    public: void 函克(){ 成员甲; 成员乙; /*成员丙;*/ }
};

class 曾孙:public 一个派生或子类2{
    public: void 函克(){ /*成员甲; 成员乙; 成员丙;*/ }
};

int 继承的方式(){
    //继承的方式有三种; 公有 保有 私有,
    /*一个基类或父类 公有1; 公有1.成员甲; 公有1.成员乙; 公有1.成员丙;
    一个基类或父类1 保有1; 保有1.成员甲; 保有1.成员乙; 保有1.成员丙;
    一个基类或父类2 私有1; 私有1.成员甲; 私有1.成员乙; 私有1.成员丙;

    一个派生或子类 公有2; 公有2.函克; 一个派生或子类1 保有2; 保有2.函克;
    一个派生或子类2 私有2; 私有2.函克; 曾孙 曾孙; 曾孙.函克;*/

    我的类(); 我的类2();  return 0;
    
}

class 对象模型的基类{
    public: int 成员甲; protected: int 成员乙; private: int 成员丙;
};

class 对象模型的子类 : public 对象模型的基类{
    public: int 成员丁;
};

int 继承中的对象模型(){
    cout<<"求大小对象模型的子类:"<<sizeof(对象模型的子类)<<endl; return 0;
}

class 构造析构顺序基类{
    public: 构造析构顺序基类(){ cout<<"构造析构顺序基类构造函数"<<endl; }
    ~构造析构顺序基类(){ cout<<"构造析构顺序基类的析构函数"<<endl; }
};

class 构造析构顺序子类:public 构造析构顺序基类{
    public: 构造析构顺序子类(){ cout<<"构造析构顺序子类构造函数"<<endl; }
    ~构造析构顺序子类(){ cout<<"构造析构顺序子类析构函数"<<endl; }
};

int 继承中的构造和析构顺序(){
    构造析构顺序子类 子类; return 0;
}

class 同名成员基类{
    public: int 成员甲;
        同名成员基类(){ 成员甲=99; }
        void 函克(){ cout<<"同名成员基类-函克()调用"<<endl; }
        void 函克(int 甲){ cout<<"基类-函克(int 甲)调用"<<endl; }
};

class 同名成员子类:public 同名成员基类{
    public: int 成员甲;
        同名成员子类(){ 成员甲 = 999; }
        void 函克(){ cout<<"同名成员子类-函克()调用"<<endl; }
};

int 继承同名成员处理方式(){
    同名成员子类 子啊; cout<<"同名子类下的成员甲"<<子啊.成员甲<<endl;
    cout<<"同名基类下的成员甲"<<子啊.同名成员基类::成员甲<<endl;
    子啊.函克(); 子啊.同名成员基类::函克();
    子啊.同名成员基类::函克(9);  return 0;
}

class 同名静态基类{
    public: 
        static int 成员甲;
        static void 函克(){ cout<<"同名静态基类-static void 函克()"<<endl; }
        static void 函克(int 甲){ 
            cout<<"同名静态基类-static void 函克(int 甲)"<<endl; }
};
int 同名静态基类::成员甲=99;

class  同名静态子类:public 同名静态基类{
    public: static int 成员甲;
        static void 函克(){ cout<<"同名静态子类-static void 函克()"<<endl; }
};
int 同名静态子类::成员甲=999;

int 继承同名静态成员处理方式(){
    cout<<"通过对象访问:"<<endl; 同名静态子类 子啊;
    cout<<"子类下的成员甲"<<子啊.成员甲<<endl;
    cout<<"基类下的成员甲"<<子啊.同名静态基类::成员甲<<endl;

    cout<<"通过类名访问:"<<endl; 
    cout<<"子类下的成员甲"<<同名静态子类::成员甲<<endl;
    cout<<"基类下的成员甲"<<同名静态子类::同名静态基类::成员甲<<endl;

    //访问同名函数
    cout<<"通过对象访问:"<<endl; 同名静态子类 子啊1;
    子啊1.函克; 子啊1.同名静态基类::函克();
    cout<<"通过类名访问:"<<endl; 
    同名静态子类::同名静态基类::函克(); 同名静态子类::函克();
    同名静态子类::同名静态基类::函克(99); return 0;
}

class 多继承1{
    public: int 成员甲;
        多继承1(){ 成员甲=99; }
};

class 多继承2{
    public: int 成员甲;
        多继承2(){ 成员甲=11; }
};

class 多继承子:public 多继承2, public 多继承1{
    public: int 成员乙, 成员丙;
        多继承子(){ 成员乙=22; 成员丙=33; }
};

int 多继承语法(){
    多继承子 子啊; cout<<"求大小多继承子类:"<<sizeof(子啊)<<endl;
    cout<<子啊.多继承1::成员甲<<endl;
    cout<<子啊.多继承2::成员甲<<endl; return 0;
}

class 动物{
    public: int 成员年龄;
};

class 羊:virtual public 动物{};
class 骆驼:virtual public 动物{};
class 羊驼:public 羊, public 骆驼{};

int 菱形继承(){
    羊驼 草泥; 草泥.羊::成员年龄=99; 草泥.骆驼::成员年龄=11;
    cout<<"草泥.羊::成员年龄"<<草泥.羊::成员年龄<<endl;
    cout<<"草泥.骆驼::成员年龄"<<草泥.骆驼::成员年龄<<endl;
    cout<<"草泥.成员年龄"<<草泥.成员年龄<<endl; return 0;
}

int 继承(){
    //继承的基本语法();
    //继承的方式();
    //继承中的对象模型();
    //继承中的构造和析构顺序();
    //继承同名成员处理方式();
    //继承同名静态成员处理方式();
    //多继承语法();
    菱形继承();

    system("pause"); return 0;
}

class 动物1{
    public: virtual void 说话(){ cout<<"动物在说话"<<endl; }
};

class 猫:public 动物1{
    public: void 说话(){ cout<<"小猫在说话"<<endl; }
};

class 狗:public 动物1{
    public: void 说话(){ cout<<"狗仔说话"<<endl; }
};

void 做说话(动物1& 小动物){
    小动物.说话();
}

int 多态的概念(){
    猫 小猫; 做说话(小猫); 狗 小狗; 做说话(小狗);  return 0;
}

class 普通实现计算器类{
    public: int 成员数号1, 成员数号2;
        int 取结果(string 操作){
            if(操作=="+"){ return 成员数号1+成员数号2; }
            else if(操作=="-"){ return 成员数号1-成员数号2; }
            else if(操作=="*"){ return 成员数号1*成员数号2; } }
};

class 多态实现计算器类{
    public: int 成员数号1, 成员数号2;
        virtual int 拿结果(){ return 0; }
};

class 加法计算器:public 多态实现计算器类{
    public: int 拿结果(){ return 成员数号1 + 成员数号2; }
};

class 减法计算器:public 多态实现计算器类{
    public: int 拿结果(){ return 成员数号1 - 成员数号2; }
};

class 乘法计算器:public 多态实现计算器类{
    public: int 拿结果(){ return 成员数号1 * 成员数号2; }
};

int 多态案例计算器(){
    普通实现计算器类 计算器; 计算器.成员数号1=9; 计算器.成员数号2=9;
    cout<<"普通实现计算器类"<<endl;
    cout<<计算器.成员数号1<<"+"<<计算器.成员数号2<<"="
    <<计算器.取结果("+")<<endl;
    cout<<计算器.成员数号1<<"-"<<计算器.成员数号2<<"="
    <<计算器.取结果("-")<<endl;
    cout<<计算器.成员数号1<<"*"<<计算器.成员数号2<<"="
    <<计算器.取结果("*")<<endl;

    cout<<"多态实现计算器类"<<endl;
    多态实现计算器类* 甲乙丙=new 加法计算器;
    甲乙丙->成员数号1=9; 甲乙丙->成员数号2=9; 
    cout<<甲乙丙->成员数号1<<"+"<<甲乙丙->成员数号2<<"="<<甲乙丙->拿结果()<<endl;
    delete 甲乙丙;

    甲乙丙=new 减法计算器;
    甲乙丙->成员数号1=9; 甲乙丙->成员数号2=9; 
    cout<<甲乙丙->成员数号1<<"-"<<甲乙丙->成员数号2<<"="<<甲乙丙->拿结果()<<endl;
    delete 甲乙丙;

    甲乙丙=new 乘法计算器;
    甲乙丙->成员数号1=9; 甲乙丙->成员数号2=9; 
    cout<<甲乙丙->成员数号1<<"*"<<甲乙丙->成员数号2<<"="<<甲乙丙->拿结果()<<endl;
    delete 甲乙丙; return 0;
}

class 纯虚抽象基类{
    public: virtual void 函克()=0;
};

class 纯虚抽象子类:public 纯虚抽象基类{
    public: virtual void 函克(){ cout<<"函数调用"<<endl; }
};

int 纯虚函数和抽象类(){
    //存续函数语法; virtual 类型 函名 (形参) = 0;
    纯虚抽象基类* 基类=NULL; //基类=new 纯虚抽象基类;
    基类 = new 纯虚抽象子类; 基类->函克(); delete 基类; return 0;
}

class 制作饮品{
    public: virtual void 烧水()=0; virtual void 冲泡()=0;
    virtual void 倒水()=0; virtual void 调味()=0;
    void 造饮品(){
        烧水(); 冲泡(); 倒水(); 调味(); }
};

class 做咖啡:public 制作饮品{
    public: virtual void 烧水(){ cout<<"掺水烧煮开"<<endl; }
    virtual void 冲泡(){ cout<<"冲泡咖啡"<<endl; }
    virtual void 倒水(){ cout<<"冲好的倒入杯具"<<endl; }
    virtual void 调味(){ cout<<"加入需要的味品"<<endl; }
};

class 做茶:public 制作饮品{
    public: virtual void 烧水(){ cout<<"掺水烧煮开"<<endl; }
    virtual void 冲泡(){ cout<<"冲泡茶叶"<<endl; }
    virtual void 倒水(){ cout<<"冲好的倒入杯具"<<endl; }
    virtual void 调味(){ cout<<"加入需要的味品"<<endl; }
};

void 做事(制作饮品* 饮品){
    饮品->造饮品(); delete 饮品; }

int 多态案例制作饮品(){
    //制作饮品的流程为; 烧水 冲泡 倒水 调味
    做事(new 做咖啡); cout<<"----------"<<endl; 做事(new 做茶); return 0;
}

class 动物基类{
    public: 动物基类(){ cout<<"动物基类的构造函数"<<endl; }
    virtual void 说话()=0;
    //virtual ~动物基类(){ cout<<"动物基类虚析构函数调用"<<endl; }
    virtual ~动物基类()=0;
};

动物基类::~动物基类(){ cout<<"动物基类纯虚析构函数调用"<<endl; }
class 猫1:public 动物基类{
    public: string* 成员名;
        猫1(string 名字){ cout<<"猫1构造函数调用"<<endl; 
        成员名=new string(名字); }
        virtual void 说话(){ cout<<*成员名<<"猫1说话"<<endl; }
        ~猫1(){ cout<<"猫1 的析构函数"<<endl; 
        if(this->成员名 != NULL){ delete 成员名; 成员名=NULL; }}
};

int 虚析构和纯虚析构(){
    //虚析构语法; virtual ~类名(){}
    //纯虚析构语法; virtual ~类名()=0; 类名::~类名(){}
    动物基类* 动物=new 猫1("汤姆猫"); 动物->说话(); delete 动物; return 0;
}

class 处理器{public: virtual void 计算()=0; };
class 显卡{public: virtual void 显示()=0; };
class 内存{public: virtual void 存储()=0; };
class 电脑{
    private: 处理器* 成员处理器; 显卡* 成员显卡; 内存* 成员内存;
    public:
        电脑(处理器* 处理,显卡* 图形处理, 内存* 记忆体){
            成员处理器=处理; 成员显卡=图形处理; 成员内存=记忆体; }
        void 工作(){
            成员处理器->计算(); 成员显卡->显示(); 成员内存->存储(); }
        ~电脑(){
            if(成员处理器 != NULL){ delete 成员处理器; 成员处理器=NULL; }
            if(成员显卡 != NULL){ delete 成员显卡; 成员显卡=NULL; }
            if(成员内存 != NULL){ delete 成员内存; 成员内存=NULL; } }
};

class 英特尔处理器:public 处理器{
    public: virtual void 计算(){ cout<<"英特尔的处理器运行"<<endl; }
};

class 英特尔显卡:public 显卡{
    public: virtual void 显示(){ cout<<"英特尔显卡运行"<<endl; }
};

class 英特尔内存:public 内存{
    public: virtual void 存储(){cout<<"英特尔内存运行"<<endl; }
};

class 不同厂商处理器:public 处理器{
    public: virtual void 计算(){ cout<<"不同厂商处理器运行"<<endl; }
};

class 不同厂商显卡:public 显卡{
    public: virtual void 显示(){ cout<<"不同厂商显卡运行"<<endl; }
};

class 不同厂商内存:public 内存{
    public: virtual void 存储(){cout<<"不同厂商内存运行"<<endl; }
};

int 多态案例组装电脑(){
    处理器* 英特尔的U=new 英特尔处理器; 显卡* 英特尔的显卡=new 英特尔显卡;
    内存* 英特尔的内存=new 英特尔内存; cout<<"第一台电脑运行"<<endl;
    电脑* 计算机=new 电脑(英特尔的U,英特尔的显卡,英特尔的内存);
    计算机->工作();  delete 计算机; cout<<"----------------"<<endl;
    cout<<"第二台电脑运行"<<endl;
    电脑* 计算机1=new 电脑(new 不同厂商处理器,new 不同厂商显卡,
                            new 不同厂商内存);
        计算机1->工作(); delete 计算机1; cout<<"------------------"<<endl;
    cout<<"第三天电脑运行"<<endl;
    电脑* 计算机2=new 电脑(new 不同厂商处理器,new 不同厂商显卡,
                            new 不同厂商内存);
        计算机2->工作(); delete 计算机2;  return 0;
}

int 多态(){
    //多态的概念();
    //多态案例计算器();
    //纯虚函数和抽象类();
    //多态案例制作饮品();
    //虚析构和纯虚析构();
    多态案例组装电脑();

    system("pause"); return 0;
}

int main(){
    //对象模型和this指针();
    //this指针概念();
    //空指针访问成员函数();
    //const修饰成员函数();
    //友元();
    //运算符重载();
    //继承();
    多态();

    return 0;
}