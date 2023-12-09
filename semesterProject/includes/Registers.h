#ifndef REGISTERS_H
#define REGISTERS_H
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <array>
#include <iomanip>
class Registers
{
public:
    int32_t reg[32] = {0};
    int32_t HI = 0;
    int32_t LO = 0;
    
    static const std::array<std::string, 32> registerNames;


    inline int32_t& getRegister(int index)
    {
        if(index < 0 || index > 31)
            throw std::out_of_range("Index is out of range.");
        return reg[index];
    }

    friend std::ostream& operator<<(std::ostream& cout, const Registers& r)
    {
        cout << std::setfill('0');
        cout << "Registers:\n";
        for(int i = 0; i < 32; i++)
        {
            cout << std::left << std::setw(1)
                 << "R" << i << " (" << std::setw(2) << std::right << Registers::registerNames[i] << ") = "
                 << std::setw(8) << r.reg[i]
                 << '\n';
       
        }
        cout << "HI = " << std::setw(8) << std::right << r.HI
             << "\nLO = " << std::setw(8) << std::right << r.LO << '\n';
        return cout;
    }

    inline void resetRegisters()
    {
        std::string ans;
        std::cout << "< SYSTEM MESSAGE >\n";
        std::cout << "Are you sure you want to reset registers? It will "
                  << "reset all registers to their original values. (Y / n): ";
        std::getline(std::cin, ans);
        if(ans == "Y")
        {
            for(int32_t i = 0; i < 32; i++)
            {
                reg[i] = 0;
            }
            std::cout << "< REGISTERS RESET >\n";
            
            return;
        }
        else
        {
            std::cout << "< REGISTERS NOT RESET, CONTINUING >\n";
        }
            
    }
};

#endif
