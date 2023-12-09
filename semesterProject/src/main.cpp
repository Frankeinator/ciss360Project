#include "../includes/spim.h"

/*
  I wanted main to be as simple and clean as possible so I made Spim::run()
  do absolutely everything.
 */

int main()
{
    std::cout << "Would you like to launch in interactive mode? (Y/N) ";
    std::string ans;
    std::getline(std::cin, ans);
    if(ans == "y" || ans == "Y")
        Spim::run(1);
    else
        Spim::run(0);
    return 0;
}
