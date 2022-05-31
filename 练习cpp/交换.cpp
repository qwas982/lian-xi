#include <iostream>
using namespace std;

void 交换(int &甲, int &乙);

int main(int 实参计数, char *实参值[])
{
    int 甲{1};
    int 乙{3};
    cout<<"实现连个数字交换"<<endl;
    cout<<"交换前甲="<<甲<<",乙="<<乙<<endl;
    交换(甲,乙);
    cout<<"交换后甲="<<甲<<",乙="<<乙<<endl;
    return 0;
}

void 交换(int &甲, int &乙)
{
    int 临时;
    临时 = 甲;
    甲 = 乙;
    乙 = 临时;
}