#include <string>
#include <Instruction.h>

class Register
{
private:
    // std::string name;
    // int value;
    // bool busyRead;
    // bool busyWrite;
    // Instruction Instruction;
    
public:
    std::string name;
    int value;
    bool busyRead;
    bool busyWrite;
    Instruction instruction;
    Register();
    Register(std::string name, int value);
    ~Register();
};

Register::Register(std::string name, int value)
{
    this->name = name;
    this->value = value;
    this->busyRead = false;
    this->busyWrite = false;
}   // TODO: Esse valor (instruction) deveria ser nulo?

Register::~Register()
{
}
