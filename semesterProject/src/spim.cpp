#include "../includes/spim.h"
#include <iostream>
#include <cctype>
#include <vector>
#include <fstream>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <cstdlib>

/*
  THE MAIN FILE!!
  TODO:
  DATA SEGMENT!! --Important, need to do next
  Idea:
  > have them type --data to goto data input section
  > They can then type data segment...
  > Data segment HAS to have:
  > cmdSep[0] be a label so 'Alex:'
  > cmdSep[1] be datatype ex. .word, .ascii, .asciiz, .byte
  >>>
  > cmdSep[everythingElse] everything after that will be put into
  > array/vector that is dynamically made
  > --spim will go back to spim mode.
  > --viewData will show the data segment in a viewing mode
  
  Machine code   --Know how to do, just need to implement
  {
    Add is done
  }
  
  Finish Jumps, once done do branching
  Branching will be: checking if the statement is true or false,
  if true perform jump, ELSE return;

  FIX Spim::isImmediate(), check if int is negative or not, because
  for now it ALWAYS returns a +number even if it is a negative number
 */

//All info about these static variables is in spim.h at the bottom

//----------static variables------------------\\
bool Spim::running = true;

std::vector < std::string > Spim::validRegs =
    {
        "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
        "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"
    };

//Obviously some of these aren't constant, but for this project they
//are because they aren't being implemented.
std::vector < std::string > Spim::constRegs =
    {
        "$zero", "$at", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
    };


std::vector < std::string > Spim::addressHistory;
std::vector < std::string > Spim::history;
std::vector < std::string > Spim::loadedFile;
std::vector < std::string > Spim::cmdSep;
std::unordered_map< std::string, std::string > Spim::labelMap;
std::string Spim::inputAddress = "0x00400000";
std::string Spim::dataAddress  = "0x10010000";


//---------Methods---------------\\
void Spim::run(int mode)
{
    std::cout << "Alex Franke SPIM simulator\nEnter ? for help\n" <<
        "type quit to exit program\n\n";
    
    //For registers.
    Registers registers;

    //Input string
    std::string input;
    
    while(Spim::getRunning())
    {
        if(mode == 1)//interactive mode
        {
            //output current line and Address of current instruction
            std::cout << "TEXT: < " << Spim::inputAddress << " >";
            
            //Get input
            std::getline(std::cin, input);
            
            /*
              Separates string into a vector of strings
              cmdSep[0] holds the instruction
              and everything after is either a register or immediate
            */
            Spim::cmdSep = Spim::spim_string_separator(input);
            //std::cout << Spim::validReg(cmdSep);
            if(Spim::cmdSep.size() == 1)
            {
                if(!input.empty() && input.back() == ':')
                {
                    Spim::history.push_back(input);
                    input.pop_back();
                    Spim::labelMap[input] = Spim::inputAddress;
                    Spim::addressHistory.push_back(Spim::inputAddress);
                    Convertor::incrHex();
                }
            }

            if(input == "reset")
                registers.resetRegisters();
            if(input == "clear")
            {
                std::system("clear");
                for(std::size_t index = 0; index < Spim::history.size(); ++index)
                {
                    if(Spim::addressHistory[index][0] != '<')
                        std::cout << "TEXT: < " << Spim::addressHistory[index] << " >" << Spim::history[index] << '\n';
                    else
                        std::cout << Spim::addressHistory[index] << ' ' << Spim::history[index] << '\n'; 
                }
            }

            //Checks for important inputs, for things such as printing registers, help, quit... etc..
            Spim::priorityCheck(registers, input, mode);

            /*
              This method, depending on mode, will either:
              Run an instruction
              Save history/addressHistory
              OR
              both
             */
            Spim::runProgram(registers, input, mode);
            //Spim::decide_instruction(registers, input);
        }
        else
        {
            std::cout << "TEXT: < " << Spim::inputAddress << " >";
            //Get input
            std::getline(std::cin, input);
            
            /*
              Separates string into a vector of strings
              cmdSep[0] holds the instruction
              and everything after is either a register or immediate
            */
            Spim::cmdSep = Spim::spim_string_separator(input);

            //Gets label input
            if(Spim::cmdSep.size() == 1)
            {
                if(!input.empty() && input.back() == ':')
                {
                    Spim::history.push_back(input);
                    input.pop_back();
                    Spim::labelMap[input] = Spim::inputAddress;
                    Spim::addressHistory.push_back(Spim::inputAddress);
                    Convertor::incrHex();
                }
            }

            //Checks for important inputs, for things such as printing registers, help, quit... etc..
            Spim::priorityCheck(registers, input, mode);
            
            /*
              This method, depending on mode, will either:
              Run an instruction
              Save history/addressHistory
              OR
              both
             */
            Spim::runProgram(registers, input, mode);
            //Spim::isValidInstruction(registers, input);
        }        
    } 
}

//Returns false if OKAY, returns true if WRONG, i.e. trying to change
//const reg...
bool Spim::constRegCheck()
{
    for(int i = 0; i < constRegs.size(); ++i)
    {
        if(Spim::cmdSep[1] == Spim::constRegs[i])
            return true;
    }
    return false;
}

//Reset the program... in order to switch modes this must be called.
//void Spim::resetProgram() {}

bool Spim::isImmediate(const std::string& str)
{
    if(str.empty())
        return false;

    for(char ch : str)
    {
        if(!std::isdigit(ch) && ch != '-' && ch != '+')
            return false;
    }
    return true;
}

bool Spim::isRegister(const std::string& str)
{
    for(const auto& reg : Spim::validRegs)
    {
        if(str == reg)
            return true;
    }
    return false;
}

bool Spim::validReg()
{
    int size = Spim::cmdSep.size() - 1;
    int counter = 0;
    for(const auto& str : Spim::cmdSep)
    {
        if(Spim::isRegister(str) || Spim::isImmediate(str))
            counter++;
    }

    return counter == size;
}
/* DELETE
//Used for interpretive mode(?) (not interactive mode)
void Spim::isValidInstruction(Registers& R, std::string input)
{
    int size = Spim::cmdSep.size();
    if(size == 2)
    {
        if(Spim::cmdSep[0] == "mflo" && Spim::cmdSep[1][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();

        }
        else if(Spim::cmdSep[0] == "mfhi" && Spim::cmdSep[1][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();

        }
        // commenting out of uninteractive until it works
       // else if(cmdSep[0] == "j" && cmdSep[1][0] != '$' && labelMap.find(cmdSep[1]) != labelMap.end())
        //{
         //   Convertor::incrHex(inputAddress);
         //   history.push_back(input);
         //   addressHistory.push_back(inputAddress);
       // }
        
    }
    else if(size == 3)
    {
        if(Spim::cmdSep[0] == "li" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex(2);
            
        }
        else if(Spim::cmdSep[0] == "move" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
            
        }
        else if(Spim::cmdSep[0] == "mult" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
            
        }
        else if(Spim::cmdSep[0] == "multu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "div" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "divu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
    }
    else if(size == 4)
    {
        //Spim::cmdSep[0] == "add" && cmdSep[1][0] == '$' && cmdSep[2][0] == '$' && cmdSep[3][0] == '$' && Spim::validReg(cmdSep)
        if(Spim::cmdSep[0] == "add" && Spim::validReg())
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sub" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "mul" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex(2);
        }        
        else if(Spim::cmdSep[0] == "addi" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sll" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "srl" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "slt" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgt" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "slti" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sltu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sltui" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] != '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "seq" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sne" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sle" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sge" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sleu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgeu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgtu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$')
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        
    }
}
*/

void Spim::priorityCheck(Registers& registers, std::string input, int mode)
{
    //interactive
    if(mode == 1)
    {
        //Initial priority checks
        if(toUpperCase(input) == "QUIT")
        {
            Spim::setRunning(false);
        }
        else if(input == "syscall") // Still need to get the rest of the syscalls done
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Instructions::syscall(registers, input, mode);
        }
        else if(input == "?")
        {
            Spim::help();
        }
        else if(input == "--history")
        {
            std::cout << "\n< HISTORY BEGIN >\n\n";
            if(Spim::history.size() == Spim::addressHistory.size())
            {
                for(std::size_t index = 0; index < Spim::history.size(); ++index)
                {
                    if(Spim::addressHistory[index][0] != '<')
                        std::cout << "TEXT: < " << Spim::addressHistory[index] << " >" << Spim::history[index] << '\n';
                    else
                        std::cout << Spim::addressHistory[index] << ' ' << Spim::history[index] << '\n'; 
                }
            }
            std::cout << "\n< HISTORY END >\n\n";
        }
        else if(input == "--reg")
        {
            std::cout << registers << std::endl;
        }
        else if(input == "--labels")
        {
            std::cout << "\n< LABELS >\n\n";
            for(const auto& pair : Spim::labelMap)
                std::cout << pair.first << '\n';
            std::cout << "\n\n";
        }
        else if(input == "--allLabels")
        {
            std::cout << "\n< LABELS  -- ADDRESS >\n\n";
            for(const auto& pair : Spim::labelMap)
                std::cout << pair.first << "   " << pair.second << '\n';
            std::cout << "\n\n";
        }
        else if(input == "--mcode")
            Convertor::mcode(registers);
    }
    else if(mode == 0)//not interactive
    {
        if(toUpperCase(input) == "QUIT")
        {
            std::cout << "< SYSTEM MESSAGE >\nWould you like to save your work before quitting? (Y/n) ";
            std::string a;
            std::getline(std::cin, a);
            if(a == "y" || a == "Y")
            {
                std::cout << "Please enter filename: ";
                std::getline(std::cin, a);

                a = Spim::addExtensionIfNeeded(a, ".s");
                
                std::ofstream file(a);
                if(!file)
                {
                    std::cerr << "< ERROR CREATING FILE " << a << " >\n";
                    return;
                }
                for(const auto& fileLine : Spim::history)
                    file << fileLine << '\n';

                if(file.fail())
                {
                    std::cerr << "< ERROR WRITING TO FILE " << a << " >\n";
                    return;
                }

                std::cout << "< FILE SAVED SUCCESSFULLY >\n";
            }
            Spim::setRunning(false);
        }
        else if(input == "?")
        {
            Spim::help();
        }
        else if(input == "--reg")
        {
            std::cout << registers << std::endl;
        }
        else if(input == "--history")
        {
            std::cout << "\n< HISTORY BEGIN >\n\n";
            if(Spim::history.size() == Spim::addressHistory.size())
            {
                for(std::size_t index = 0; index < Spim::history.size(); ++index)
                {
                    if(Spim::addressHistory[index][0] != '<')
                        std::cout << "TEXT: < " << Spim::addressHistory[index] << " >" << Spim::history[index] << '\n';
                    else
                        std::cout << Spim::addressHistory[index] << ' ' << Spim::history[index] << '\n'; 
                }
            }
            std::cout << "\n< HISTORY END >\n\n";
        }
        else if(input == "syscall")
        {
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
        }
        else if(input == "--labels")
        {
            std::cout << "\n< LABELS >\n\n";
            for(const auto& pair : Spim::labelMap)
                std::cout << pair.first << '\n';
            std::cout << "\n\n";
        }
        else if(input == "--allLabels")
        {
            std::cout << "\n< LABELS  -- ADDRESS>\n\n";
            for(const auto& pair : Spim::labelMap)
                std::cout << pair.first << "   " << pair.second << '\n';
            std::cout << "\n\n";
        }
        else if(input == "--mcode")
            Convertor::mcode(registers);
    }
    else if(mode == 2) //Jumping/branching/running
    {
        /*
          MODE FOR JUMPING
          PUT JUST
          SYSCALL AND DON'T SAVE TO HISTORY!!!!
          Change Spim::decideInstruction to have
          another mode JUST for JUMPING so that no history is saved
          will fix jumping :)
         */
        if(input == "syscall")
        {
            std::cout << "$v0: " <<  registers.reg[2] << '\n';
            Instructions::syscall(registers, input, mode);
        }
    }
}

//get index of register or if an immediate, converts it to int and returns.
int Spim::getIndex(std::string reg)
{
    bool isDigit = true;
    for(char ch : reg)
    {
        if(!std::isdigit(ch))
        {
            isDigit = false;
            break;
        }
    }
    if(isDigit)
    {
        return static_cast<int32_t>(std::stoi(reg));
    }
    
    if(reg == "$zero")
        return 0;
    else if(reg == "$gp")
        return 28;
    else if(reg == "$sp")
        return 29;
    else if(reg == "$fp")
        return 30;
    else if(reg == "$ra")
        return 31;
       

    if(reg[1] == 'a')
    {
        if(reg == "$at")
            return 1;
        else if(reg == "$a0")
            return 4;
        else if(reg == "$a1")
            return 5;
        else if(reg == "$a2")
            return 6;
        else if(reg == "$a3")
            return 7;
    }
    else if(reg[1] == 'v')
    {
        if(reg == "$v0")
            return 2;
        else if(reg == "$v1")
            return 3;
    }
    else if(reg[1] == 't')
    {
        if(reg == "$t0")
            return 8;
        else if(reg == "$t1")
            return 9;
        else if(reg == "$t2")
            return 10;
        else if(reg == "$t3")
            return 11;
        else if(reg == "$t4")
            return 12;
        else if(reg == "$t5")
            return 13;
        else if(reg == "$t6")
            return 14;
        else if(reg == "$t7")
            return 15;
        else if(reg == "$t8")
            return 24;
        else if(reg == "$t9")
            return 25;
        
    }
    else if(reg[1] == 's')
    {
        if(reg == "$s0")
            return 16;
        else if(reg == "$s1")
            return 17;
        else if(reg == "$s2")
            return 18;
        else if(reg == "$s3")
            return 19;
        else if(reg == "$s4")
            return 20;
        else if(reg == "$s5")
            return 21;
        else if(reg == "$s6")
            return 22;
        else if(reg == "$s7")
            return 23;
    }
    else if(reg[1] == 'k')
    {
        if(reg == "$k0")
            return 26;
        else
            return 27;
    }
    return 0;
}

//See if filename already has an extension
bool Spim::endsWith(const std::string& str, const std::string& suffix)
{
    if(str.length() < suffix.length())
        return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

//Adds extension if user hasnt given one
std::string Spim::addExtensionIfNeeded(const std::string& filename, const std::string& extension)
{
    if(!Spim::endsWith(filename, extension))
        return filename + extension;
    return filename;
}
/*
//Do the instruction based on cmdSep[0] example: li, add, sub... etc (for interactive mode)
void Spim::decide_instruction(Registers& R, std::string input)
{
    int size = Spim::cmdSep.size();
    int* arr = new int[size];
    
    for(int i = 1; i < Spim::cmdSep.size(); i++)
    {
        arr[i] = Spim::getIndex(Spim::cmdSep[i]);
    }
    if(size == 2)
    {
        if(Spim::cmdSep[0] == "mflo" && Spim::cmdSep[1][0] == '$'  && !Spim::constRegCheck())
        {
            Instructions::mflo(R, arr[1]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
            
        }
        else if(Spim::cmdSep[0] == "mfhi" && Spim::cmdSep[1][0] == '$' && !Spim::constRegCheck())
        {
            Instructions::mfhi(R, arr[1]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "j" && Spim::cmdSep[1][0] != '$' && Spim::labelMap.find(Spim::cmdSep[1]) != Spim::labelMap.end())
        {
            Spim::history.push_back(input);
            Instructions::j(input, R);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        
        
    }
    else if(size == 3)
    {
        if(Spim::cmdSep[0] == "li" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] != '$')
        {
            Instructions::li(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex(2);
        }
        else if(Spim::cmdSep[0] == "move" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            Instructions::move(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "mult" && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::mult(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "multu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::multu(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "div"  && Spim::validReg() && !Spim::constRegCheck())
        {
            try
            {
            Instructions::div(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }
        else if(Spim::cmdSep[0] == "divu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            try
            {
            Instructions::divu(R, arr[1], arr[2]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }
    }
    else if(size == 4)
    {
        
         //Spim::cmdSep[0] == "add" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$' && Spim::cmdSep[3][0] == '$' && Spim::validReg() && !Spim::constRegCheck()
        if(Spim::cmdSep[0] == "add" && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::add(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sub" && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sub(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "mul" && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::mul(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex(2);
        }        
        else if(Spim::cmdSep[0] == "addi" && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            Instructions::addi(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sll"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            Instructions::sll(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "srl"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            Instructions::srl(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "slt"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::slt(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgt"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sgt(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();

        }
        else if(Spim::cmdSep[0] == "slti"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            Instructions::slti(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sltu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sltu(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sltui"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            Instructions::sltui(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "seq"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::seq(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sne"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sne(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sle"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sle(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sge"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sge(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sleu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sleu(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgeu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sgeu(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
        }
        else if(Spim::cmdSep[0] == "sgtu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            Instructions::sgtu(R, arr[1], arr[2], arr[3]);
            Spim::history.push_back(input);
            Spim::addressHistory.push_back(Spim::inputAddress);
            Convertor::incrHex();
                
        }
    } 
}
*/

//Separates input string into instuction, registers, and immediates.
std::vector< std::string > Spim::spim_string_separator(const std::string& s)
{
    std::vector< std::string > ret;
    std::string currWord;

    for(char ch : s)
    {
        if(ch == '\t' || ch == ' ' || ch == ',')
        {
            if(!currWord.empty())
            {
                ret.push_back(currWord);
                currWord.clear();
            }
            
        } else {
            currWord += ch;
        }
    }

    if(!currWord.empty())
        ret.push_back(currWord);
    
    return ret;
}

//Condition for while loop
bool Spim::getRunning()
{
    return running;
}
//Change state of while loop condition
void Spim::setRunning(bool c)
{
    running = c;
}

//Convert to uppercase, idk if this will be used that much
//mainly for convencience so that all inputs that aren't registers,
//dont have to be case sensitive, like quit
std::string Spim::toUpperCase(const std::string& str)
{
    std::string ret;
    for(char ch : str)
        ret += std::toupper(ch);

    return ret;
}


void Spim::help()
{
    std::cout << "\n\n\n\n===============HELP===============\n"
              << "To view data segment type --data\n"
              << "To view labels type --labels\n"
              << "To view labels and the address they are associated with type --allLabels\n"
              << "To view register values type --reg\n"
              << "To view machine code for current history type --mcode\n"
              << "Labels HAVE to be on their own line AND end with : "
              << "Or else the label will not be saved.\n"
              << "To view valid history type '--history'\n\n\n"
              << "Currently implemented:\n"
              << "'add', 'sub', 'mult', 'div', 'addi',\n"
              << "'li', 'move', 'mflo', 'mfhi'\n\n";
}


//This will do various instructions and methods
//based on which mode is active
//Mode 0: Uninteractive
//Mode 1: Interactive
//Mode 2: Jumping/Branching
void Spim::runProgram(Registers& R, std::string input, int mode)
{
    int size = Spim::cmdSep.size();
    int* arr = new int[size];
    
    for(int i = 1; i < Spim::cmdSep.size(); i++)
    {
        arr[i] = Spim::getIndex(Spim::cmdSep[i]);
    }
    if(size == 2)
    {
        if(Spim::cmdSep[0] == "mflo" && Spim::cmdSep[1][0] == '$'  && !Spim::constRegCheck())
        {
            if(mode == 1 || mode == 2)
                Instructions::mflo(R, arr[1]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "mfhi" && Spim::cmdSep[1][0] == '$' && !Spim::constRegCheck())
        {
            if(mode == 1 || mode == 2)
                Instructions::mfhi(R, arr[1]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "j" && Spim::cmdSep[1][0] != '$' && Spim::labelMap.find(Spim::cmdSep[1]) != Spim::labelMap.end())
        {
            if(mode == 0 || mode == 1)
                Spim::history.push_back(input);
            if(mode == 2 || mode == 1)
                Instructions::j(input, R);
            if(mode == 0 || mode == 1)
            {
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        
        
    }
    else if(size == 3)
    {
        if(Spim::cmdSep[0] == "li" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::li(R, arr[1], arr[2]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex(2);
            }
        }
        else if(Spim::cmdSep[0] == "move" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::move(R, arr[1], arr[2]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "mult" && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
            Instructions::mult(R, arr[1], arr[2]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "multu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::multu(R, arr[1], arr[2]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "div"  && Spim::validReg() && !Spim::constRegCheck())
        {
            try
            {
                if(mode == 2 || mode == 1)
                    Instructions::div(R, arr[1], arr[2]);
                if(mode == 0 || mode == 1)
                {
                    Spim::history.push_back(input);
                    Spim::addressHistory.push_back(Spim::inputAddress);
                    Convertor::incrHex();
                }
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }
        else if(Spim::cmdSep[0] == "divu" && Spim::cmdSep[1][0] == '$' && Spim::cmdSep[2][0] == '$')
        {
            try
            {
                if(mode == 2 || mode == 1)
                    Instructions::divu(R, arr[1], arr[2]);
                if(mode == 0 || mode == 1)
                {
                    Spim::history.push_back(input);
                    Spim::addressHistory.push_back(Spim::inputAddress);
                    Convertor::incrHex();
                }
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }
    }
    else if(size == 4)
    {
        if(Spim::cmdSep[0] == "add" && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::add(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sub" && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sub(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "mul" && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::mul(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex(2);
            }
        }        
        else if(Spim::cmdSep[0] == "addi" && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::addi(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sll"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::sll(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "srl"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::srl(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "slt"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::slt(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sgt"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sgt(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }

        }
        else if(Spim::cmdSep[0] == "slti"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::slti(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sltu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sltu(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sltui"  && Spim::validReg() && !Spim::constRegCheck() && Spim::cmdSep[3][0] != '$')
        {
            if(mode == 2 || mode == 1)
                Instructions::sltui(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "seq"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::seq(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sne"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sne(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sle"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sle(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sge"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sge(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sleu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sleu(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sgeu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sgeu(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }
        }
        else if(Spim::cmdSep[0] == "sgtu"  && Spim::validReg() && !Spim::constRegCheck())
        {
            if(mode == 2 || mode == 1)
                Instructions::sgtu(R, arr[1], arr[2], arr[3]);
            if(mode == 0 || mode == 1)
            {
                Spim::history.push_back(input);
                Spim::addressHistory.push_back(Spim::inputAddress);
                Convertor::incrHex();
            }   
        }
    } 
}
