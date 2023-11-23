#include <string>
#include <Instruction.h>

class Register
{
private:
    std::string name;
    int value;
    bool busyRead;
    bool busyWrite;
    Instruction Instruction;
    
public:
    Register(string name, int value);
    ~Register();
};

Register::Register(string name, int value)
{
    this->name = name;
    this->value = value;
    this->busyRead = false;
    this->busyWrite = false;
}   // TODO: Esse valor (instruction) deveria ser nulo?

Register::~Register()
{
}
