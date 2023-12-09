#include "../includes/convertor.h"
#include <sstream>
#include <iomanip>

void Convertor::printBits(uint32_t b)
{
    for(int i = 31; i >= 0; --i)
    {
        std::cout << ((b >> i) & 1);
    }
    std::cout << '\n';
}

std::string Convertor::uintToHex(uint32_t val)
{
    std::stringstream ss;
    ss << std::hex << val;
    return ss.str();
}

std::string Convertor::uintToChars(uint32_t val)
{
    std::string chars;
    chars += static_cast<char>((val >> 24) & 0xFF);
    chars += static_cast<char>((val >> 16) & 0xFF);
    chars += static_cast<char>((val >> 8) & 0xFF);
    chars += static_cast<char>(val & 0xFF);
    return chars;
}

uint32_t Convertor::hexToUint(const std::string& str)
{
    uint32_t val = 0;
    std::stringstream ss(str);
    ss >> std::hex >> val;
    return val;
}

uint32_t Convertor::charsToUint(const std::string& str)
{
    uint32_t val = 0;
    for(int i = 0; i < 4 && i < str.length(); ++i)
            val |= (static_cast<uint32_t>(str[i]) << ((3-i) * 8));
    return val;
}

void Convertor::incrHex(/*std::string& str,*/ int times)
{
    for(int i = 0; i < times; i++)
    {
        uint32_t val;
        std::stringstream ss(Spim::inputAddress);
        ss >> std::hex >> val;
        
        val += 4;
        
        std::stringstream out;
        out << "0x"
            << std::setfill('0') << std::setw(8)
            << std::hex << val;
        Spim::inputAddress = out.str();
    }
}

void Convertor::mcode(const Registers R)
{
    uint32_t opcode, rd, rs, rt, shamt, funct, mcode;
    std::cout << "\n\n< MACHINE CODE FOR CURRENT HISTORY >\n\n";
    std::vector < std::string > cmdSep;
    std::string str;
    for(auto& line : Spim::history)
    {
        std::cout << line << ": \n";
        cmdSep = Spim::spim_string_separator(line);

        if(cmdSep[0] == "add")
        {
            opcode = 0;
            shamt = 0;
            funct = 32;
            rs = Spim::getIndex(cmdSep[2]);
            rt = Spim::getIndex(cmdSep[3]);
            rd = Spim::getIndex(cmdSep[1]);
        }
        
        
        mcode = opcode << 26
            | rs << 21
            | rt << 16
            | rd << 11
            | shamt << 6
            | funct;
        std::cout << "M. code: " << mcode << '\n';
        std::cout << "Binary: ";
        printBits(mcode);
        std::cout << '\n';
    }
    
        
}
