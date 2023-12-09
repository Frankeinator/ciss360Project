#include "../includes/instructions.h"
#include "../includes/spim.h"

std::fstream Instructions::file;
std::string Instructions::path;
bool Instructions::processingCase14 = false;

void Instructions::lb(){} //idk what to do yet
void Instructions::sb(){} //Still
void Instructions::lw(){} 
void Instructions::sw(){}
void Instructions::la(){}

//Fix jumping for 
void Instructions::j(const std::string input, Registers& R)
{
    int stopIndex;
    for(int i = 0; i < Spim::history.size(); ++i)
    {
        if(Spim::history[i] == input)
        {
            stopIndex = i;
            break;
        }
    }
    if(Spim::addressHistory.empty() || Spim::cmdSep.size() < 2)
        return;
    
    std::string label2find = Spim::cmdSep[1];
    //std::cout << Spim::cmdSep[1] << '\n';
    int index = 0;
    if(Spim::labelMap.find(label2find) != Spim::labelMap.end())
    {
        std::string address2find = Spim::labelMap[label2find];
        //std::cout << address2find << '\n';

        bool found = false;
        for(index; index < Spim::addressHistory.size(); ++index)
        {
            
            if(Spim::addressHistory[index] == address2find)
            {
                //std::cout << "Address " << address2find << " found in address"
                //<< " history at index " << index << '\n';
                
                found = true;
                break;
            }
        }
        //std::cout << index << '\n';
        //std::cout << "TEXT: < " << Spim::addressHistory[index] << " >" << Spim::history[index] << '\n';
        //std::cout << input << '\n';
        //std::cout << index << ' ' << stopIndex << std::endl;
        if(found)
        {
            //std::cout << "start\n\n\n";
            
            //START HERE close to working?????
            for(int i = index; i < stopIndex - 1; ++i)
            {
                //std::cout << Spim::history[i] << '\n';
                Spim::cmdSep = Spim::spim_string_separator(Spim::history[i]);
                
                std::cout << "Text: < " << Spim::addressHistory[i]
                          << '>' << Spim::history[i] << '\n';
                if(Spim::history[i] == "syscall")
                    Instructions::syscall(R, Spim::history[i], 2);
                Spim::runProgram(R, Spim::history[i], 2);
                
            }
            
            //std::cout << "end\n\n\n";
        }
        if(!found)
            std::cout << "Address not found in addressHistory.\n";
    } else
        std::cout << "Label not found in labelMap.\n";
}

//Still need to fully implement
void Instructions::syscall(Registers& R, std::string input, int mode)
{
    std::string line;
    switch(R.reg[2])
    {
        case 1: //print int
        {
            
            std::string temp = "< CONSOLE INT OUTPUT > ";
            std::cout << temp << R.reg[4] << std::endl;
            if(mode == 1 || mode == 0)
            {
                Spim::history.push_back(std::to_string(R.reg[4]));
                Spim::addressHistory.push_back(temp);
            }
            break;
        }
        case 2: //print float
            break;
        case 3: //print double
            break;
        case 4: //print string
            break;
        case 5: //read int
        {
            std::string temp = "< CONSOLE INT INPUT > ";
            std::cout << temp;
            std::cin >> R.reg[2];
            
            Spim::history.push_back(std::to_string(R.reg[2]));
            Spim::addressHistory.push_back(temp);
            
            //std::cin.ignore bc std::cin>> interferes with std::getline??????????
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
            break;
        case 6: //read float
            break;
        case 7: //read double
            break;
        case 8: //read string
            std::cout << "< CONSOLE STRING INPUT >";
            //TODO
            break;
        case 9: //skip
            std::cout << "< NOT IMPLEMENTED >\n";
            break;
        case 10: //Exit
            Spim::setRunning(false);//Change spim::running
            break;
        case 11: //print char
            break;
        case 12: //read char
            break;
        case 13: //open file
        {
            // /home/student/school/ciss360/semesterProject/test.txt
            std::cout << "< FILE NAME > ";
            std::getline(std::cin, path);

            //Loads the contents of the file into vector loadedFile
            file.open(path, std::ios::in | std::ios::out);
            if(!file)
            {
                std::cerr << "< UNABLE TO OPEN FILE: " << path << " > " << std::endl;
                return;
            }
            
            while(std::getline(file, line))
            {
                Spim::loadedFile.push_back(line);
            }                   
            break;
        }
        
        case 14: //read from file
        {
            if(processingCase14)
                break;
            processingCase14 = true;
            if(file.is_open())
            {
                for(const auto& fileLine : Spim::loadedFile)
                {
                    std::cout << "TEXT: < " << Spim::inputAddress << " >";
                    std::cout << fileLine << std::endl;
                    if(fileLine == "syscall")
                        Instructions::syscall(R, input, mode);
                    Spim::history.push_back(fileLine);
                    Spim::cmdSep = Spim::spim_string_separator(fileLine);
                    //IDK which mode
                    Spim::runProgram(R, input, 1);
                    //Spim::decide_instruction(R, input);
                }
            } else
                std::cout << "< NO FILE OPEN >\n";
            processingCase14 = false;
            break;
        }
        case 15: //write to file -- WORKS mostly
        {
            //Clear contents of file first, then rewrite to file.
            if(file.is_open())
            {
                //Go to beginning of file and remove everything inside.
                file.clear();
                file.seekp(0, std::ios::beg);
                file.close();
                file.open(path, std::ios::out | std::ios::trunc);

                if(!file.is_open())
                    std::cout << "< FILE NOT FOUND >\n";
                for(const auto& fileLine : Spim::history)
                {
                    file << fileLine << '\n';
                }
                std::cout << "< FILE WRITE COMPLETE >\n";
                file.flush();
            }
            else
            {
                std::cout << "< NO FILE OPEN >\n";
                break;
            }
            break;
        }
        case 16: //close file
        {
            Spim::loadedFile.clear();
            if(file.is_open())
            {
                file.close();
                std::cout << "< FILE CLOSED >\n";
            } else
                std::cout << "< NO FILE OPEN >\n";
            
            break;
        }
        default:
            std::cout << "< INVALID $v0 VALUE >\n";
            break;
    }
}



void Instructions::li(Registers& R, int regIndex, int immValue)
{
    R.reg[regIndex] = immValue;
}

void Instructions::add(Registers& R, int sumR, int op1R, int op2R)
{
    R.reg[sumR] = R.reg[op1R] + R.reg[op2R];
}

void Instructions::addi(Registers& R, int sumR, int op1R, int immValue)
{
    R.reg[sumR] = R.reg[op1R] + immValue;
}

void Instructions::addu(Registers& R, int sumR, int op1R, int op2R)
{
    uint32_t sum = static_cast<uint32_t>(R.reg[op1R]) + static_cast<uint32_t>(R.reg[op2R]);
    R.reg[sumR] = sum;
}

void Instructions::addiu(Registers& R, int sumR, int op1R, uint32_t immValue)
{
    uint32_t sum = static_cast<uint32_t>(R.reg[op1R]) + immValue;
    R.reg[sumR] = sum;
}

void Instructions::sub(Registers& R, int diffR, int op1R, int op2R)
{
    R.reg[diffR] = R.reg[op1R] - R.reg[op2R];
}

void Instructions::subu(Registers& R, int diffR, int op1R, int op2R)
{
    uint32_t diff = static_cast<uint32_t>(R.reg[op1R]) - static_cast<uint32_t>(R.reg[op2R]);
    R.reg[diffR] = diff;
}

void Instructions::move(Registers& R, int toR, int fromR)
{
    R.reg[toR] = R.reg[fromR];
}

void Instructions::mult(Registers& R, int prodR, int op1R)
{
    int64_t product = static_cast<int64_t>(R.reg[prodR]) * static_cast<int64_t>(R.reg[op1R]);

    //store lower bits into LO
    uint32_t lowerBits = static_cast<uint32_t>(product & 0xFFFFFFFF);
    R.LO = lowerBits;

    //store upper bits into HI
    R.HI = static_cast<uint32_t>(product >> 32);
}

void Instructions::mul(Registers& R, int prodR, int op1R, int op2R)
{
    int64_t product = static_cast<int64_t>(R.reg[op1R]) * static_cast<int64_t>(R.reg[op2R]);

    //store lower bits into LO and product into R.reg[prodR]
    uint32_t lowerBits = static_cast<uint32_t>(product & 0xFFFFFFFF);
    R.LO = lowerBits;
    R.reg[prodR] = product;

    //Store higher bits into HI
    R.HI = static_cast<uint32_t>(product >> 32);
}

void Instructions::multu(Registers& R, int prodR, int op1R)
{
    uint64_t product = static_cast<uint64_t>(R.reg[op1R]) * static_cast<uint64_t>(R.reg[prodR]);

    //store lower bits into LO and into R.reg[prodR]
    uint32_t lowerBits = static_cast<uint32_t>(product & 0xFFFFFFFF);
    R.LO = lowerBits;

    //store upper bits into HI
    R.HI = static_cast<uint32_t>(product >> 32);
}

void Instructions::div(Registers& R, int quotR, int op1R)
{
    if(R.reg[op1R] == 0){
        throw std::runtime_error("< DIVISION BY ZERO >\n");
    }
    else
    {
        int32_t quotient = static_cast<int32_t>(R.reg[quotR]) / static_cast<int32_t>(R.reg[op1R]);

        int32_t remainder = static_cast<int32_t>(R.reg[quotR]) % static_cast<int32_t>(R.reg[op1R]);

        //Store quotient into LO and into register
        R.LO = static_cast<uint32_t>(quotient);
        

        //Store remainder into HI
        R.HI = static_cast<uint32_t>(remainder);
    }
}

void Instructions::divu(Registers& R, int quotR, int op1R)
{
    if(R.reg[op1R] == 0){
        throw std::runtime_error("< DIVISION BY ZERO >\n");
    }
    
    else
    {
        uint32_t quotient = static_cast<uint32_t>(R.reg[quotR]) / static_cast<uint32_t>(R.reg[op1R]);

        uint32_t remainder = static_cast<uint32_t>(R.reg[quotR]) % static_cast<uint32_t>(R.reg[op1R]);

        //Store quotient into LO and into register
        R.LO = static_cast<uint32_t>(quotient);
        

        //Store remainder into HI
        R.HI = static_cast<uint32_t>(remainder);
    }
}

void Instructions::sll(Registers& R, int destR, int otherR, int shamt)
{
    R.reg[destR] = (R.reg[otherR] << shamt);
}

void Instructions::srl(Registers& R, int destR, int otherR, int shamt)
{
    R.reg[destR] = (R.reg[otherR] >> shamt);
}

void Instructions::mflo(Registers& R, int toR)
{
    R.reg[toR] = R.LO;
}

void Instructions::mfhi(Registers& R, int toR)
{
    R.reg[toR] = R.HI;
}

//Need to figure out labels -- NO BRANCHING IS CHECKED IN DECIDE_INSTRUCTION OR isValidInstruction!!!!!!!!!!!!!! 
bool Instructions::beq(Registers& R, int op1R, int op2R)
{
    return (R.reg[op1R] == R.reg[op2R]);
}

bool Instructions::bne(Registers& R, int op1R, int op2R)
{
    return (R.reg[op1R] != R.reg[op2R]);
}

bool Instructions::bgez(Registers& R, int op1R)
{
    return (R.reg[op1R] >= 0);
}

bool Instructions::bgtz(Registers& R, int op1R)
{
    return (R.reg[op1R] > 0);
}

bool Instructions::blez(Registers& R, int op1R)
{
    return (R.reg[op1R] <= 0);
}

bool Instructions::bltz(Registers& R, int op1R) 
{
    return (R.reg[op1R] < 0);
}



void Instructions::slt(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] < R.reg[op2R] ? 1 : 0); 
}

void Instructions::sgt(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] > R.reg[op2R] ? 1 : 0); 
}

void Instructions::slti(Registers& R, int destR, int op1R, int immValue)//
{
    R.reg[destR] = (R.reg[op1R] < immValue ? 1 : 0); 
}

void Instructions::sltu(Registers& R, int destR, int op1R, int op2R)//
{
    //static_cast< uint32_t >(R.reg[destR]) = (static_cast<uint32_t>(R.reg[op1R]) < static_cast<uint32_t>(R.reg[op2R]) ? 1 : 0);
    R.reg[destR] = static_cast<uint32_t>(R.reg[op1R] < R.reg[op2R] ? 1 : 0);
}

void Instructions::sltui(Registers& R, int destR, int op1R, uint32_t immValue)//
{
    R.reg[destR] = (static_cast<uint32_t>(R.reg[op1R]) < immValue ? 1 : 0); 
}

void Instructions::seq(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] == R.reg[op2R] ? 1 : 0); 
}

void Instructions::sne(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] != R.reg[op2R] ? 1 : 0); 
}

void Instructions::sle(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] <= R.reg[op2R] ? 1 : 0); 
}

void Instructions::sge(Registers& R, int destR, int op1R, int op2R)//
{
    R.reg[destR] = (R.reg[op1R] >= R.reg[op2R] ? 1 : 0); 
}

void Instructions::sleu(Registers& R, int destR, int op1R, int op2R)//
{
    //static_cast< uint32_t >(R.reg[destR]) = (static_cast<uint32_t>(R.reg[op1R]) <= static_cast<uint32_t>(R.reg[op2R]) ? 1 : 0);
    R.reg[destR] = static_cast<uint32_t>(R.reg[op1R] <= R.reg[op2R] ? 1 : 0);
}

void Instructions::sgeu(Registers& R, int destR, int op1R, int op2R)
{
    //static_cast< uint32_t >(R.reg[destR]) = (static_cast<uint32_t>(R.reg[op1R]) >= static_cast<uint32_t>(R.reg[op2R]) ? 1 : 0);
    R.reg[destR] = static_cast<uint32_t>(R.reg[op1R] >= R.reg[op2R] ? 1 : 0);
}

void Instructions::sgtu(Registers& R, int destR, int op1R, int op2R)
{
    //static_cast< uint32_t >(R.reg[destR]) = (static_cast<uint32_t>(R.reg[op1R]) > static_cast<uint32_t>(R.reg[op2R]) ? 1 : 0);
    R.reg[destR] = static_cast<uint32_t>(R.reg[op1R] > R.reg[op2R] ? 1 : 0);
}


