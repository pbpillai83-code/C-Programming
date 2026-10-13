#include <iostream>
#include <conio.h>
int hours,pay,opay,total,rate,overtime;

int main ()
{
    std::cout<<"Enter the hours worked =";
    std::cin>>hours;
    std::cout<<"Enter the rate per hour =";
    std::cin>>rate;


    if (hours>40)
    {
        overtime=hours-40;
    opay=overtime*2*rate;
    pay=40*rate;
    total=pay+opay;
    }
    else
    total=hours*rate;
    std::cout<<"The total salary of the servant is ="<<total<<" Rupees";
    return 0;
}
