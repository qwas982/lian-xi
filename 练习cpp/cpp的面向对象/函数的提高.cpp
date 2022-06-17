#include <iostream>
using namespace std;

int 函克(int 甲,int 乙=9,int 丙=9){
    return 甲+乙+丙;
}

int 函克1(int 甲=9,int 乙=9);
int 函克1(int 甲,int 乙){
    return 甲+乙;
}

int 函数的默认参数(){
    cout<<"返来="<<函克(99,91)<<endl; cout<<"返来:"<<函克(19)<<endl;
    system("pause"); return 0;
}

void 函克2(int 甲,int){
    cout<<"这是函数"<<endl;
}

int 函数的占位参数(){
    函克2(9,9); //占位参数 需补齐,
    system("pause"); return 0;
}

//函数重载需要在同一个作用域下.
void 函克3(){
    cout<<"函克3的调用"<<endl;
}

void 函克3(int 甲){
    cout<<"函克3(int 甲)的调用"<<endl;
}

void 函克3(double 甲){
    cout<<"函克3(double 甲)的调用"<<endl;
}

void 函克3(int 甲,double 乙){
    cout<<"函克3(int 甲,double 乙)的调用"<<endl;
}

void 函克3(double 甲,int 乙){
    cout<<"函克3(double 甲,int 乙)的调用"<<endl;
}


int 函数重载(){
    函克3(); 函克3(9); 函克3(3.14); 函克3(9,3.14); 函克3(3.14,9);
    system("pause"); return 0;
}

void 函克4(int &甲){
    cout<<" 函克4(int &甲)的调用"<<endl;
}


void 函克4(const int &甲){
    cout<<" 函克4(const int &甲)的调用"<<endl;
}

void 函克5(int 甲,int 乙=9){
    cout<<" 函克5(int 甲,int 乙=9)的调用"<<endl;
}

void 函克5(int 甲){
    cout<<" 函克5(int 甲)的调用"<<endl;
}

int 重载注意事项(){
    int 甲=9; 函克4(甲); 函克4(9); 
    //函克5(9);
    system("pause"); return 0;
}

int main(){

    //函数的默认参数();
    //函数的占位参数();
    //函数重载();
    重载注意事项();

    return 0;
}