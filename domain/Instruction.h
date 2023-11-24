#include <string>
// Definindo a estrutura de uma instrução
class  Instruction
{
private:
    // std::string op;
    // std::string dest;
    // std::string src1;
    // std::string src2;
    // int execCycles;
    // bool isIssued;
    // bool isExecuting;
    // bool isCompleted;
public:
    std::string op;
    std::string dest;
    std::string src1;
    std::string src2;
    int execCycles;
    bool isIssued;
    bool isExecuting;
    bool isCompleted;
     Instruction();
     Instruction(std::string op, std::string dest, std::string src1, std::string src2);
    ~ Instruction();

    
};

//Construtor
 Instruction::Instruction(std::string op, std::string dest, std::string src1, std::string src2)
{
    this->op = op;
    this->dest = dest;
    this->src1 = src1;
    this->src2 = src2;
    this->execCycles = 0;
    this->isIssued = false;
    this->isExecuting = false;
    this->isCompleted = false;
}

//Destrutor
 Instruction::~ Instruction()
{
}
