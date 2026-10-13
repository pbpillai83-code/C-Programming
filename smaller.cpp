#include <iostream>
#include <conio.h>
int a,b,c,min;

int main ()
{
    std::cout<<"Enter the first number =";
    std::cin>>a;
    std::cout<<"Enter the second number =";
    std::cin>>b;
    std::cout<<"Enter the third number =";
    std::cin>>c;

    if (a<b)
        if(a<c)
        min=a;
    if (b<a)
        if (b<c)
        min=b;
    if (c<a)
        if (c<b)
        min=c;
        std::cout<<"The smallest number is ="<<min;
        return 0;
}
