#include "函数声明.h"

void 交换(int 甲, int 乙)
{
    int 临时 = 甲; 甲 = 乙; 乙=临时;
    cout<<"乙="<<乙<<endl;cout<<"甲="<<甲<<endl;
}