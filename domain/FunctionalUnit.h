#include <string>
#include <Instruction.h>
//Definindo a estrutura de uma unidade funcional
class FunctionalUnit
{
private:
    std::string type;
    int latency;
    bool busy;
    Instruction instruction;
public:
    FunctionalUnit(string type, int latency);
    ~FunctionalUnit();
};

FunctionalUnit::FunctionalUnit(string type, int latency)
{
    this->type = type;// Tipo da unidade funcional (ex: add, mul)
    this->latency = latency;// Ciclos de latência da unidade funcional OBS: Altera valores depois
    this->busy = false;// Indica se está ocupada
}   // TODO: Esse valor (instruction) deveria ser nulo?

FunctionalUnit::~FunctionalUnit()
{
}
