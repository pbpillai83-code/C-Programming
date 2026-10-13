#include <iostream>
#include <conio.h>
int sum,counter,n;
int main()
{
    std::cout<<"Enter the number of the table =";
    std::cin>>n;
    counter=1;
    in:
        sum=n*counter;
        std::cout<<n<<"*"<<counter<<"="<<sum<<"\n";
        counter=counter+1;
        if (counter<=10)
            goto in;
        return 0;
}
