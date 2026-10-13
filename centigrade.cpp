//Temperature conversion from fahrenheit to centigrade
#include <iostream>
#include <conio.h>
float ftemp;
float ctemp;

int main()
{
    std::cout<<"The temperature in Fahrenheit is "<<"\n";
    std::cin>>ftemp;
    ctemp =((ftemp-32.0)*5.0/9.0);
    std::cout<<"The temperature in Centigrade is "<<ctemp<<"\n";
    return 0;
}
