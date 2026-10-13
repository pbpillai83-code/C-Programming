//Program to find out symbol from ASCII character set
#include <iostream>
#include <conio.h>

int n,n1;
char ch,ch1;

int main()
{
    std::cout<<"Enter any number = ";
    std::cin>>n;
    ch=n;
    std::cout<<"The character of the given number is = "<<ch<<"\n";
    std::cout<<"Press any character on the keyboard ="<<"\n";
    std::cin>>ch1;
    n1=ch1;
    std::cout<<"The numeric value of the given character is ="<<n1<<"\n";
    return 0;
}
