#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "Registers.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <limits>

/*
  All methods are static because I didn't want to have an object
  be required to call them.
  Bad in hindsight but it works
 */


class Instructions
{
  public:
    static std::fstream file;
    static std::string path;
    static bool processingCase14;
    
    static void li(Registers& R, int regIndex, int immValue);//works

    static void lb(); //not done
    static void sb(); //not done

    static void add(Registers& R, int sumR, int op1R, int op2R);//works
    static void addi(Registers& R, int sumR, int op1R, int immValue);//works
    static void addu(Registers& R, int sumR, int op1R, int op2R);//works
    static void addiu(Registers& R, int sumR, int op1R, uint32_t immValue);//works
    
    static void sub(Registers& R, int diffR, int op1R, int op2R);
    static void subu(Registers& R, int diffR, int op1R, int op2R);

    static void move(Registers& R, int toR, int fromR);
    
    static void syscall(Registers& R, std::string input, int mode);

    static void lw(); //not done -- pseudo -> worth 2 instuctions
    static void sw(); //not done

    static void la(); //not done

    static void mult(Registers& R, int prodR, int op1R);//works
    static void mul(Registers& R, int prodR, int op1R, int op2R);//works
    static void multu(Registers& R, int prodR, int op1R);//works 
    static void div(Registers& R, int quotR, int op1R);//works
    static void divu(Registers& R, int quotR, int op1R);//works
    static void mflo(Registers& R, int toR);//works
    static void mfhi(Registers& R, int toR);//works

    static void sll(Registers& R, int destR, int otherR, int shamt);//works
    static void srl(Registers& R, int destR, int otherR, int shamt);//works

    //branching -- idk how
    static bool beq(Registers&, int op1R, int op2R);//works
    static bool bne(Registers&, int op1R, int op2R);//works
    static bool bgez(Registers& R, int op1R);//works
    static bool bgtz(Registers& R, int op1R);//works
    static bool blez(Registers& R, int op1R);//works
    static bool bltz(Registers& R, int op1R);//works

    //pseudo branch
    static bool blt(); //-- pseudo -> worth 2 instuctions
    static bool ble();// -- pseudo -> worth 2 instuctions
    static bool bgt();// -- pseudo -> worth 2 instuctions
    static bool bge();// -- pseudo -> worth 2 instuctions
    static bool bez();
    static bool bnez();

    //unsigned branch
    static bool bequ();
    static bool bgeu();
    static bool bgtu();
    static bool blu();
    static bool bltu();

    //Set -- all done
    static void slt(Registers& R, int destR, int op1R, int op2R); //-
    static void slti(Registers& R, int destR, int op1R, int immValue); //-
    static void sltu(Registers& R, int destR, int op1R, int op2R);//-
    static void sltui(Registers& R, int destR, int op1R, uint32_t immValue);//-

    static void seq(Registers& R, int destR, int op1R, int op2R);//-
    static void sne(Registers& R, int destR, int op1R, int op2R);//-

    static void sle(Registers& R, int destR, int op1R, int op2R);//-
    static void sge(Registers& R, int destR, int op1R, int op2R);//-
    static void sgt(Registers& R, int destR, int op1R, int op2R); //-

    static void sleu(Registers& R, int destR, int op1R, int op2R);//-
    static void sgeu(Registers& R, int destR, int op1R, int op2R);//-
    static void sgtu(Registers& R, int destR, int op1R, int op2R);//-

    //Jump -- Mostly done
    static void j(const std::string input, Registers& R);

    
};

#endif
