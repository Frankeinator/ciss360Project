#ifndef CONVERTOR_H
#define CONVERTOR_H
#include "Registers.h"
#include "spim.h"
#include <string>
#include <cstdint>
#include <vector>
class Convertor
{
  public:
    static std::string uintToHex(uint32_t val);
    static std::string uintToChars(uint32_t val);
    static uint32_t hexToUint(const std::string& str);
    static uint32_t charsToUint(const std::string& str);

    //To increment address
    static void incrHex(/*std::string& str,*/ int times=1);

    //convert instructions to machine code and print it
    static void mcode(const Registers R);

    static void printBits(uint32_t b);
    
    
};
#endif
