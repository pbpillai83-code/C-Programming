#include <iostream>
#include <conio.h>
unsigned y,u,t,h,th,res;
int main()
{
    std::cout<<"Enter the year = ";
    std::cin>>y;
    th=y/1000;
    res=y%1000;
    h=res/100;
    res=res%100;
    t=res/10;
    res=res%10;
    u=res/1;
    std::cout<<"The reverse number is = "<<u<<t<<h<<th;
    return 0;
}
