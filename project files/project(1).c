// Atandra Mahalder, Emin Mammadzada, Thomas Barone
// CDA project Spring 2021
// 04/14/2021

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // get value of ALUresult according to the control signal
    switch ((int)ALUControl)
    {
        // addition
        case 0: *ALUresult = A + B;
                break;

        // subtraction
        case 1: *ALUresult = A - B;
                break;

        // set less than
        case 2: *ALUresult = ((signed)A < (signed)B);
                break;

        // set less than unsigned
        case 3: *ALUresult = (A < B);
                break;

        // and
        case 4: *ALUresult = A & B;
                break;

        // or
        case 5: *ALUresult = A | B;
                break;

        // shift left by 16 bits
        case 6: *ALUresult = B << 16;
                break;

        // negation
        case 7: *ALUresult = ~A;
                break;
    }

    // set Zero according to ALUresult
    *Zero = (*ALUresult == 0);
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // if PC is not word aligned or not within memory bounds, halt
    if (PC & 3 || PC < 0 || PC > 65532)
        return 1;

    // fetch instruction from memory
    *instruction = Mem[PC >> 2];

    // return 0 if no halt conditions have been encountered
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // fetching the op code
    *op = (instruction >> 26);

    // if the instruction is J type
    if (*op == 2)
    {
        // store jump target calculator
        *jsec = instruction & ((1 << 26) - 1);

        return;
    }

    // fetching the first two registers
    *r1 = (instruction >> 21) & 31;
    *r2 = (instruction >> 16) & 31;

    // if the instruction is R type
    if (*op == 0)
    {
        // fetch the destination register
        *r3 = (instruction >> 11) & 31;

        // function code
        *funct = instruction & 63;

        return;
    }

    // if the instruction is I type fetch the last 16 bits to store the constant
    *offset = instruction & ((1 << 16) - 1);
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // set controls according to op - opcode
    switch (op)
    {
        // R type
        case 0: controls->RegDst = 1;
                controls->Jump = 0;
                controls->Branch = 0;
                controls->MemRead = 0;
                controls->MemtoReg = 0;
                controls->ALUOp = 7;
                controls->MemWrite = 0;
                controls->ALUSrc = 0;
                controls->RegWrite = 1;
                return 0;

        // jump
        case 2: controls->RegDst = 0;
                controls->Jump = 1;
                controls->Branch = 0;
                controls->MemRead = 0;
                controls->MemtoReg = 0;
                controls->ALUOp = 0;
                controls->MemWrite = 0;
                controls->ALUSrc = 0;
                controls->RegWrite = 0;
                return 0;

        // addi
        case 8: controls->RegDst = 0;
                controls->Jump = 0;
                controls->Branch = 0;
                controls->MemRead = 0;
                controls->MemtoReg = 0;
                controls->ALUOp = 0;
                controls->MemWrite = 0;
                controls->ALUSrc = 1;
                controls->RegWrite = 1;
                return 0;

        // load word
        case 35: controls->RegDst = 0;
                 controls->Jump = 0;
                 controls->Branch = 0;
                 controls->MemRead = 1;
                 controls->MemtoReg = 1;
                 controls->ALUOp = 0;
                 controls->MemWrite = 0;
                 controls->ALUSrc = 1;
                 controls->RegWrite = 1;
                 return 0;

        // store word
        case 43: controls->RegDst = 2;
                 controls->Jump = 0;
                 controls->Branch = 0;
                 controls->MemRead = 0;
                 controls->MemtoReg = 2;
                 controls->ALUOp = 0;
                 controls->MemWrite = 1;
                 controls->ALUSrc = 1;
                 controls->RegWrite = 0;
                 return 0;

        // load upper immediate
        case 15: controls->RegDst = 0;
                 controls->Jump = 0;
                 controls->Branch = 0;
                 controls->MemRead = 0;
                 controls->MemtoReg = 0;
                 controls->ALUOp = 6;
                 controls->MemWrite = 0;
                 controls->ALUSrc = 1;
                 controls->RegWrite = 1;
                 return 0;

        // branch if equal
        case 4:  controls->RegDst = 2;q
                 controls->Jump = 0;
                 controls->Branch = 1;
                 controls->MemRead = 0;
                 controls->MemtoReg = 2;
                 controls->ALUOp = 1;
                 controls->MemWrite = 0;
                 controls->ALUSrc = 0;
                 controls->RegWrite = 0;
                 return 0;

        // set less than immediate
        case 10: controls->RegDst = 0;
                 controls->Jump = 0;
                 controls->Branch = 0;
                 controls->MemRead = 0;
                 controls->MemtoReg = 0;
                 controls->ALUOp = 2;
                 controls->MemWrite = 0;
                 controls->ALUSrc = 1;
                 controls->RegWrite = 1;
                 return 0;

        // set less than immediate unsigned
        case 11: controls->RegDst = 0;
                 controls->Jump = 0;
                 controls->Branch = 0;
                 controls->MemRead = 0;
                 controls->MemtoReg = 0;
                 controls->ALUOp = 3;
                 controls->MemWrite = 0;
                 controls->ALUSrc = 1;
                 controls->RegWrite = 1;
                 return 0;
    }

    // if the instruction doesn't match any of the supported instructions halt
    return 1;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // fetch contents of register 1 and register 2
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // if offset is +ve, set extended_value equal to offset
    *extended_value = offset;

    // if offset is -ve, set the first 16 bits of extended_value to 1
    if (offset & (1 << 15))
        *extended_value = (((1 << 16) - 1) << 16) | offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // if ALUSrc is 0, we use data2 with data1 to make calculations
    if (ALUSrc == 0)
    {
        // if ALUOp is 7, the instruction is R type. So, check the function code
        if (ALUOp == 7)
        {
            switch(funct)
            {
                // addition
                case 32: ALU(data1, data2, 0, ALUresult, Zero);
                         return 0;

                // subtract
                case 34: ALU(data1, data2, 1, ALUresult, Zero);
                         return 0;

                // and
                case 36: ALU(data1, data2, 4, ALUresult, Zero);
                         return 0;

                // or
                case 37: ALU(data1, data2, 5, ALUresult, Zero);
                         return 0;

                // set less than
                case 42: ALU(data1, data2, 2, ALUresult, Zero);
                         return 0;

                // set less than unsigned
                case 43: ALU(data1, data2, 3, ALUresult, Zero);
                         return 0;

                // if we have an unknown function code, halt
                default: return 1;
            }
        }

        // if instruction is not R type, use ALUOp to make calculations
        ALU(data1, data2, ALUOp, ALUresult, Zero);
        return 0;
    }

    // if ALUSrc is 1, use extended_value along with data1 to make calculations
    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // if we are allowed access to memory
    if (MemWrite == 1 || MemRead == 1)
    {
        // if ALUresult is not word aligned or ALUresult is not within memory bounds, halt
        if (ALUresult & 3 || ALUresult < 0 || ALUresult > 65532)
            return 1;

        // if MemWrite is activated, write to memory
        if (MemWrite)
            Mem[ALUresult >> 2] = data2;

        // if MemRead is activated, read from memory
        if (MemRead)
            *memdata = Mem[ALUresult >> 2];
    }

    // if no halt conditions are encountered, return 0
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // if RegWrite is activated
    if (RegWrite == 1)
    {
        // if RegDst is 1, use r3 as destination register
        if (RegDst == 1)
        {
            // set register r3 equal to ALUresult
            Reg[r3] = ALUresult;

            // if MemtoReg is activated, set register r3 to memdata
            if (MemtoReg == 1)
                Reg[r3] = memdata;
        }

        // if RegDst is 0, use r2 as destination register
        if (RegDst == 0)
        {
            // set register r2 equal to ALUresult
            Reg[r2] = ALUresult;

            // if MemtoReg is activated, set register r3 to memdata
            if (MemtoReg == 1)
                Reg[r2] = memdata;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    // update PC address by adding 4
    *PC += 4;

    // if Jump is activated, update PC according to jump algorithm
    if (Jump == 1)
        *PC = (*PC & (15 << 28)) | (jsec << 2);

    // if Branch is activated, update PC according to branch algorithm
    if (Branch == 1)
        *PC += (extended_value << 2);
}
