#include <iostream>
#include <conio.h>
int marks;

int main ()
{
 std::cout<<" Enter the marks =";
 std::cin>>marks;
 if (marks>=85)
        std::cout<<"The Grade is A+";
 else if (marks>=80)
    std::cout<<"The Grade is A";
 else if (marks>=75)
    std::cout<<"The Grade is B+";
 else if (marks>=70)
    std::cout<<"The Grade is B";
 else if (marks>=65)
    std::cout<<"The Grade is C+";
 else if (marks>=60)
    std::cout<<"The Grade is C";
 else if (marks>=55)
    std::cout<<"The Grade is D+";
 else if (marks>=50)
    std::cout<<"The Grade is D";
 else
    std::cout<<"The Grade is F";
    return 0;
}
