#ifndef SPIM_H
#define SPIM_H
#include <string>
#include <vector>
#include <unordered_map>
#include "Registers.h"
#include "instructions.h"
#include "convertor.h"
/*
  The meat and taters of the program. This class calls the entire project
  and is responsible for it working.
 */

class Spim
{
  public:
    Spim()
    {}

    ~Spim()
    {}

    //--------Methods
    static void run(int mode);

    static bool getRunning();
    static void setRunning(bool c);

    static std::string toUpperCase(const std::string& s);

    static void loadFile(const std::string& path, std::vector< std::string >& fileContents);
    

    static std::vector< std::string > spim_string_separator(const std::string& s);

    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string addExtensionIfNeeded(const std::string& filename, const std::string& extension);
    
    static void help();

    static void decide_instruction(Registers& registers, std::string input);

    
    static int getIndex(std::string reg);
    
    static void priorityCheck(Registers& registers, std::string input, int mode);

    
    static void isValidInstruction(Registers& R, std::string input);

    static bool validReg();
    static bool constRegCheck();
    static bool isImmediate(const std::string& str);
    static bool isRegister(const std::string& str);

    static void runProgram(Registers& R, std::string input, int mode);


    //-----------Members
    static bool running;

    //Vectors that are used by pretty much all classes
    //So I made them static so that I don't have to pass
    //Them through every function that needs them.
    static std::vector < std::string > validRegs;
    static std::vector < std::string > constRegs;

    
    //Used for storing address history for jumping and branching
    static std::vector < std::string > addressHistory;
    
    
    //Used for the separation of inputs
    static std::vector < std::string > cmdSep;
    
    //To show valid history, and used for file i/o
    //will only save valid inputs
    static std::vector < std::string > history;
    

    //Holds an entire file, line by line
    static std::vector < std::string > loadedFile;
    
    //Holds labels and their address
    static std::unordered_map< std::string, std::string > labelMap;
    
    //Starting inputAddress
    static std::string inputAddress;

    //Data starting Address
    static std::string  dataAddress;
};

#endif
