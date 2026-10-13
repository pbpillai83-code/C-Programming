#include <iostream>
#include <conio.h>
int n;

int main ()
{
    std::cout<<"Enter the number =";
    std::cin>>n;
    if(n%2==0)
        std::cout<<"The number is even";
    else
        std::cout<<"The number is odd";
    return 0;
}
