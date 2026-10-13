#include <iostream>
#include <conio.h>
int a,b,c,max;

int main()
{
    std::cout<<"Enter the first number = "<<"\n";
    std::cin>>a;
    std::cout<<"Enter the second number = "<<"\n";
    std::cin>>b;
    std::cout<<"Enter the third number = "<<"\n";
    std::cin>>c;
    if (a>b)
        max=a;
    else
        max=b;
    if (c>max)
        max=c;
    std::cout<<"The greater number is = "<<max;
    return 0;
}
