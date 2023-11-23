#include<FunctionalUnit.h>
class Tomasulo
{
private:
    std::vector<Instruction> Instructions;
    std::vector<FunctionalUnit> addUnits;
    std::vector<FunctionalUnit> mulUnits;
    std::vector<FunctionalUnit> swUnits;
    int cycle;
public:
    Tomasulo(/* args */);
    ~Tomasulo();
};

Tomasulo::Tomasulo(/* args */)
{
}

Tomasulo::~Tomasulo()
{
}
