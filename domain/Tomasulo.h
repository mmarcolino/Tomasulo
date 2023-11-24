#include<FunctionalUnit.h>
#include<Register.h>
#include<map>
#include<vector>
class Tomasulo
{
private:
    std::vector<Instruction> instructions;
    std::vector<FunctionalUnit*> addUnits;
    std::vector<FunctionalUnit*> mulUnits;
    std::vector<FunctionalUnit*> swUnits;
    std::vector<Register*> registers;
    std::map<int, std::string> rename;
    int cycle;
    std::vector<FunctionalUnit*> createFunctionalUnits(int numUnits, std::string type, int latency);
    std::vector<Register*> createRegisters(int quant);
    bool isExecutionComplete();
    void issue();
    void execute();
    void write();
    void showState();
    void renameRegister(Register destRegister, int index);
    std::map<std::string, std::string> rename; 
    FunctionalUnit* findAvailableUnit(std::vector<FunctionalUnit*> units);
    Register* getRegister(std::string name);
    void writeExecution(std::vector<FunctionalUnit*> units);
    void returnRenamed(Register dest, int index);
    int performOperation(std::string op, int src1, int src2);
    bool hasKey(const std::string& key) const;

public:
    Tomasulo();
    Tomasulo(std::vector<Instruction> instructions, std::map<std::string, int> values);
    ~Tomasulo();
    void Run();
};

Tomasulo::Tomasulo(std::vector<Instruction> instructions, std::map<std::string, int> values)
{
    this->instructions = instructions;
    this->addUnits = createFunctionalUnits(values["addUnitQnt"], "add", values["addUnitLatency"]);
    this->mulUnits = createFunctionalUnits(values["mulUnitQnt"], "mul", values["mulUnitLatency"]);
    this->swUnits = createFunctionalUnits(values["swUnitQnt"], "sw", values["swUnitLatency"]);
    this->registers = createRegisters(values["registerQnt"]);
    this->cycle = 1;
}

Tomasulo::~Tomasulo()
{
}

    // Cria as unidades funcionais
std::vector<FunctionalUnit*> Tomasulo::createFunctionalUnits(int numUnits, std::string type, int latency){
    std::vector<FunctionalUnit*> units;
    for (int i = 0; i < numUnits; i++)
    {
        units.push_back(new FunctionalUnit(type, latency));
    }
    return units;
}

    // Cria os registradores
std::vector<Register*> Tomasulo::createRegisters(int quant){
    std::vector<Register*> registers;
    for (int i = 0; i < quant; i++){
        registers.push_back(new Register("F%d", i));
    }
    for (int i = 0; i < quant; i++){
        registers.push_back(new Register("R%d", i));
    }
    return registers;
}

//Executa o algoritmo de Tomasulo
void Tomasulo::Run(){
    while (!this->isExecutionComplete())
    {
        /* code */
    }
    
}

//Printa o status atual das instruções
void Tomasulo::showState(){
    printf("\n------------\nCycle %d", this->cycle);
    printf("\n> Lidas: ", this->cycle);
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if(this->instructions[i].isIssued){
            printf("Instrucao %d %d %d %d", i, this->instructions[i].op, this->instructions[i].dest, this->instructions[i].src1), this->instructions[i].src2;
        }
    }
    printf("\n> Em execucao:");
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if(this->instructions[i].isExecuting){
            printf("Instrucao %d %d %d %d", i, this->instructions[i].op, this->instructions[i].dest, this->instructions[i].src1), this->instructions[i].src2;
        }
    }
    printf("\n>Concluidas:");
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if(this->instructions[i].isExecuting){
            printf("Instrucao %d %d %d %d", i, this->instructions[i].op, this->instructions[i].dest, this->instructions[i].src1), this->instructions[i].src2;
        }
    }
}

// Verifica se a execução está completa
bool Tomasulo::isExecutionComplete(){
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if(!this->instructions[i].isCompleted){
            return false;
        }
    }
    return true;
}

// Renomeia o Registrador
void Tomasulo::renameRegister(Register destRegister, int index){
    // Declara o inicio dos registradores temporarios
    int posTempReg = this->registers.size() / 2;
    std::string newName = "R0";

    //Loop para encontrar registrador temporario disponível
    while (this->registers[posTempReg]->busyRead || this->registers[posTempReg]->busyWrite)
    {
        posTempReg++;
        if(posTempReg > this->registers.size()){
            return;
        }else{
            newName = this->registers[posTempReg]->name;
        }
    }
    // Verifica se o registrador alvo já está no mapa de renomeação
    if (this->rename.find(destRegister.name) == this->rename.end()){
        // Se sim, atualiza o mapa de renomeação com o novo nome
        this->rename[newName] = this->rename[destRegister.name];
    }
    // Se não, adiciona o registrador alvo ao mapa de renomeação
    else{
        this->rename[newName] = destRegister.name;
    }

    //Renomear próximas ocorrências do registrador alvo
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if (this->instructions[i].dest == destRegister.name){
            this->instructions[i].dest = newName;
        }
        if (this->instructions[i].src1 == destRegister.name){
            this->instructions[i].src1 = newName;
        }
        if (this->instructions[i].src2 == destRegister.name){
            this->instructions[i].src2 = newName;
        }
    }
    
}

void Tomasulo::issue() {
    int constrain = this->instructions.size();
    if (this->cycle < this->instructions.size()) {
        constrain = this->cycle;
    }

    for (int i = 0; i < constrain; i++) {
        Instruction& instruction = this->instructions[i];
        FunctionalUnit* unit = new FunctionalUnit();

        if (instruction.isExecuting || instruction.isCompleted || !instruction.isIssued) {
            // Verifica se há uma unidade funcional disponível para a instrução
            instruction.isIssued = true;
            unit = nullptr;
        }else if(instruction.op == "add" || instruction.op == "sub"){
            unit = this->findAvailableUnit(this->addUnits);
        }else if(instruction.op == "mul" || instruction.op == "div"){
            unit = this->findAvailableUnit(this->addUnits);
        }else if(instruction.op == "sw" || instruction.op == "lw"){
            unit = this->findAvailableUnit(this->addUnits);
        }

        if(unit){
            // Ajusta para caso de SW e LW
            Register* aux;
            if (instruction.op == "sw" || instruction.op == "lw"){
                //aux = new Register("aux", instruction.src1); 
                //TODO: arrumar essa coisa feia do krl de js
            }else{
                aux = this->getRegister(instruction.dest);
            }
            // Verifica se os operandos estão disponíveis
            Register* destRegister = this->getRegister(instruction.dest);
            Register* src1Register = aux;
            Register* src2Register = this->getRegister(instruction.src2);

            
            //Dependencia Falsa
            if(!destRegister->busyRead || destRegister->busyWrite){
                this->renameRegister(*destRegister, i);
            }

            //dependencia verdadeira
            if(!src1Register->busyWrite && !src2Register->busyWrite){
                // Define a instrução como executando
                instruction.isExecuting = true;
                instruction.execCycles = unit->latency;
                // Define a unidade funcional como ocupada
                unit->busy = true;
                unit->instruction = instruction;

                // Atualiza os registradores utilizados pela instrução
                destRegister->busyWrite = true;
                destRegister->instruction = instruction;

                src1Register->busyRead = true;
                src1Register->instruction = instruction;

                src2Register->busyWrite = true;
                src2Register->instruction = instruction;

            }
        }

    }
}

void Tomasulo::execute(){
    for (int i = 0; i < this->addUnits.size(); i++)
    {
        FunctionalUnit* unit = this->addUnits[i];
        if (unit->busy){
            unit->instruction.execCycles --;
            if (unit->instruction.execCycles == 0){
                unit->instruction.isExecuting == false;
            }
        }
    }

    for (int i = 0; i < this->mulUnits.size(); i++) {
        FunctionalUnit* unit = this->mulUnits[i];
        if (unit->busy) {
            unit->instruction.execCycles--;
            if (unit->instruction.execCycles == 0) {
                unit->instruction.isExecuting = false;
            }
        }
    }

    for (int i = 0; i < this->swUnits.size(); i++) {
        FunctionalUnit* unit = this->swUnits[i];
            if (unit->busy) {
                unit->instruction.execCycles--;
                if (unit->instruction.execCycles == 0) {
                    unit->instruction.isExecuting = false;
                }
            }
    }

    
}

void Tomasulo::write(){
    this->writeExecution(this->addUnits);
    this->writeExecution(this->mulUnits);
    this->writeExecution(this->swUnits);
}

std::vector<int> cacheMem(32, 2);
void Tomasulo::writeExecution(std::vector<FunctionalUnit*> units){
    for (int i = 0; i < units.size(); i++)
    {
        FunctionalUnit* unit = units[i];
        if (unit->busy && !unit->instruction.isExecuting && !unit->instruction.isCompleted)
        {
            unit->instruction.isCompleted = true;
            unit->busy = false;

            // Atualiza o valor do registrador de destino
            Register* destRegister = this->getRegister(unit->instruction.dest);
            Register* src1Register = this->getRegister(unit->instruction.src1);
            Register* src2Register = this->getRegister(unit->instruction.src2);

            if (unit->instruction.op == "sw"|| unit->instruction.op == "lw")
            {

                int ofset = this->performOperation("add", unit->instruction.src1, src2Register->value);
                if (unit->instruction.op == "sw")
                {
                    cacheMem[ofset % cacheMem.size()] = destRegister->value;
                } else if (unit->instruction.op == "lw"){
                    destRegister->value = cacheMem[ofset % cacheMem.size()];
                }
                
            }
            else{
                destRegister->value = this->performOperation(unit->instruction.op, src1Register->value, src2Register->value);
                src1Register->busyRead = false;
                src1Register->instruction = nullptr;

            }

            destRegister->busyWrite = fale;
            destRegister->instruction = nullptr;

            if(this->hasKey(destRegister->name)){
                this->returnRenamed(destRegister, i);
            }

            src2Register->busyRead = false;
            src2Register->instruction = nullptr;
            
        }
        
    }
  
}

void Tomasulo::returnRenamed(Register dest, int index){
    for (int i = 0; i < this->instructions.size(); i++)
    {
        if (this->instructions[i].dest == dest.name)
        {
            this->instructions[i].dest = this->rename.find(dest.name);
        }
        if (this->instructions[i].src1 == dest.name)
        {
            this->instructions[i].src1 = this->rename.find(dest.name);
        }
        if (this->instructions[i].src2 == dest.name)
        {
            this->instructions[i].src2 = this->rename.find(dest.name);
        }
        
    }
    
}

FunctionalUnit* Tomasulo::findAvailableUnit(std::vector<FunctionalUnit*> units) {
    for (int i = 0; i < units.size(); i++) {
        if (!units[i]->busy) {
            return units[i];
        }
    }
    return new FunctionalUnit();  // Use 'new FunctionalUnit()' to return a pointer to a dynamically allocated object
}

Register* Tomasulo::getRegister(std::string name) {
    for (int i = 0; i < this->registers.size(); i++)
    {   
        if(this->registers[i]->name == name){
            return this->registers[i];
        }
    }
    return new Register(); // Use 'new Register()' to return a pointer to a empty object
}

bool Tomasulo::hasKey(const std::string& key) const {
    return (this->rename.find(key) != this->rename.end());
}

int Tomasulo::performOperation(std::string op, int src1, int src2){
    if (op == 'add') {
        return src1 + src2;
    } else if (op == 'sub') {
        return src1 - src2;
    } else if (op == 'mul') {
        return src1 * src2;
    } else if (op == 'div') {
        return src1 / src2;
    } 
}