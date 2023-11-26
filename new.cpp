#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
// Definindo a estrutura de uma instrução
class Instruction {
public:
    std::string op; // Operação (add, sub, mul, div, etc.)
    std::string dest; // Registrador de destino
    std::string src1; // Operando 1
    std::string src2; // Operando 2
    int execCycles; // Ciclos de execução restantes
    bool isIssued;
    bool isExecuting; // Indica se está em execução
    bool isCompleted; // Indica se está concluída

    Instruction(std::string op, std::string dest, std::string src1, std::string src2)
        : op(op), dest(dest), src1(src1), src2(src2), execCycles(0), isIssued(false), isExecuting(false), isCompleted(false) {}
};

// Definindo a estrutura de uma unidade funcional
class FunctionalUnit {
public:
    std::string type; // Tipo da unidade funcional (ex: add, mul)
    int latency; // Ciclos de latência da unidade funcional OBS: Altera valores depois
    bool busy; // Indica se está ocupada
    Instruction* instruction; // Instrução atualmente em execução

    FunctionalUnit(std::string type, int latency)
        : type(type), latency(latency), busy(false), instruction(nullptr) {}
};

// Definindo a estrutura de um registrador
class Register {
public:
    std::string name; // Nome do registrador
    int value; // Valor atual do registrador
    bool busyRead; // Indica se o registrador está sendo lido
    bool busyWrite; // Indica se o registrador está sendo escrito
    Instruction* instruction; // Instrução que está utilizando o registrador

    Register(std::string name, int value)
        : name(name), value(value), busyRead(false), busyWrite(false), instruction(nullptr) {}
};

// Implementação do algoritmo de Tomasulo
class Tomasulo {
public:
    std::vector<Instruction*> instructions; // Lista de instruções
    std::vector<FunctionalUnit*> addUnits; // Unidades funcionais de soma
    std::vector<FunctionalUnit*> mulUnits; // Unidades funcionais de multiplicação
    std::vector<FunctionalUnit*> swUnits; // Unidades funcionais de store word
    std::vector<Register*> registers; // Registradores
    std::map<std::string, std::string> rename; //
    int cycle; // Ciclo atual
    std::vector<int> cacheMem;

    Tomasulo(std::vector<Instruction*> instructions, std::map<std::string, int> values)
        : instructions(instructions), cycle(1), cacheMem(32, 2) {
        addUnits = createFunctionalUnits(values["addUnitQnt"], "add", values["addUnitLatency"]);
        mulUnits = createFunctionalUnits(values["mulUnitQnt"], "mul", values["mulUnitLatency"]);
        swUnits = createFunctionalUnits(values["swUnitQnt"], "sw", values["swUnitLatency"]);
        registers = createRegisters(values["registerQnt"]);
    }

    // Cria as unidades funcionais
    std::vector<FunctionalUnit*> createFunctionalUnits(int numUnits, std::string type, int latency) {
        std::vector<FunctionalUnit*> units;
        for (int i = 0; i < numUnits; i++) {
            units.push_back(new FunctionalUnit(type, latency));
        }
        return units;
    }

    // Cria os registradores
    std::vector<Register*> createRegisters(int quant) {
        std::vector<Register*> reg;
        for (int i = 0; i < quant; i++) {
            reg.push_back(new Register("F" + std::to_string(i), 1));
        }
        for (int i = 0; i < quant; i++) {
            reg.push_back(new Register("R" + std::to_string(i), 1));
        }
        return reg;
    }

    // Executa o algoritmo de Tomasulo
    void run() {
        while (!isExecutionComplete()) {
            issue();
            execute();
            write();
            showState();
            cycle++;
        }
        std::cout << "\nExecution complete\n";
    }

    void showState() {
        std::cout << "\n------------\nCycle " << cycle;
        std::cout << "\n> Lidas: ";
        for (size_t i = 0; i < instructions.size(); i++) {
            if (instructions[i]->isIssued) {
                std::cout << "\nInstrucao " << instructions[i]->op << " " << instructions[i]->dest
                          << " " << instructions[i]->src1 << " " << instructions[i]->src2;
            }
        }
        std::cout << "\n> Em execucao:";
        for (size_t i = 0; i < instructions.size(); i++) {
            if (instructions[i]->isExecuting) {
                std::cout << "\nInstrucao " << instructions[i]->op << " " << instructions[i]->dest
                          << " " << instructions[i]->src1 << " " << instructions[i]->src2;
            }
        }
        std::cout << "\n> Concluidas:";
        for (size_t i = 0; i < instructions.size(); i++) {
            if (instructions[i]->isCompleted) {
                std::cout << "\nInstrucao " << instructions[i]->op << " " << instructions[i]->dest
                          << " " << instructions[i]->src1 << " " << instructions[i]->src2;
            }
        }
        std::cout << std::endl;
    }

    // Verifica se a execução está completa
    bool isExecutionComplete() {
        for (size_t i = 0; i < instructions.size(); i++) {
            if (!instructions[i]->isCompleted) {
                return false;
            }
        }
        return true;
    }

    void renameRegister(Register* target, size_t index) {
        // Declara o inicio dos registradores temporarios
        size_t posTempReg = registers.size() / 2;
        std::string newName = "R0";

        // Loop para encontrar registrador temporario disponivel
        while (registers[posTempReg]->busyRead || registers[posTempReg]->busyWrite) {
            posTempReg++;
            if (posTempReg >= registers.size()) {
                return;
            } else {
                newName = registers[posTempReg]->name;
            }
        }

        if (rename.find(target->name) != rename.end()) {
            rename[newName] = rename[target->name];
        } else {
            rename[newName] = target->name;
        }

        // Renomear próximas ocorrências do registrador alvo
        for (size_t i = index; i < instructions.size(); i++) {
            if (instructions[i]->dest == target->name) {
                instructions[i]->dest = newName;
            }

            if (instructions[i]->src1 == target->name) {
                instructions[i]->src1 = newName;
            }

            if (instructions[i]->src2 == target->name) {
                instructions[i]->src2 = newName;
            }
        }
    }

    // Faz o estágio de emissão (issue)
    void issue() {
        size_t constrain = instructions.size();
        if (cycle < instructions.size()) {
            constrain = cycle;
        }
        for (size_t i = 0; i < constrain; i++) {
            Instruction* instruction = instructions[i];
            FunctionalUnit* unit = nullptr;
            if (instruction->isExecuting || instruction->isCompleted || !instruction->isIssued) {
                // Verifica se há uma unidade funcional disponível para a instrução
                instruction->isIssued = true;
                unit = nullptr;
            } else if (instruction->op == "add" || instruction->op == "sub") {
                unit = findAvailableUnit(addUnits);
            } else if (instruction->op == "mul" || instruction->op == "div") {
                unit = findAvailableUnit(mulUnits);
            } else if (instruction->op == "sw" || instruction->op == "lw") {
                unit = findAvailableUnit(swUnits);
            }

            if (unit) {
                // Ajusta para caso de SW e LW
                Register* aux;
                if (instruction->op == "sw" || instruction->op == "lw") {
                    aux = new Register("aux", std::stoi(instruction->src1));
                } else {
                    aux = getRegister(instruction->src1);
                }

                // Verifica se os operandos estão disponíveis
                Register* destRegister = getRegister(instruction->dest);
                Register* src1Register = aux;
                Register* src2Register = getRegister(instruction->src2);

                // Dependencia falsa
                if (destRegister->busyRead || destRegister->busyWrite) {
                    renameRegister(destRegister, i);
                }

                // Dependencia verdadeira
                if (!src1Register->busyWrite && !src2Register->busyWrite) {
                    // Define a instrução como executando
                    instruction->isExecuting = true;
                    instruction->execCycles = unit->latency;

                    // Define a unidade funcional como ocupada
                    unit->busy = true;
                    unit->instruction = instruction;

                    // Atualiza os registradores utilizados pela instrução
                    destRegister->busyWrite = true;
                    destRegister->instruction = instruction;

                    src1Register->busyRead = true;
                    src1Register->instruction = instruction;

                    src2Register->busyRead = true;
                    src2Register->instruction = instruction;
                }
            }
        }
    }

    // Faz o estágio de execução
    void execute() {
        for (size_t i = 0; i < addUnits.size(); i++) {
            FunctionalUnit* unit = addUnits[i];
            if (unit->busy) {
                unit->instruction->execCycles--;
                if (unit->instruction->execCycles == 0) {
                    unit->instruction->isExecuting = false;
                }
            }
        }

        for (size_t i = 0; i < mulUnits.size(); i++) {
            FunctionalUnit* unit = mulUnits[i];
            if (unit->busy) {
                unit->instruction->execCycles--;
                if (unit->instruction->execCycles == 0) {
                    unit->instruction->isExecuting = false;
                }
            }
        }

        for (size_t i = 0; i < swUnits.size(); i++) {
            FunctionalUnit* unit = swUnits[i];
            if (unit->busy) {
                unit->instruction->execCycles--;
                if (unit->instruction->execCycles == 0) {
                    unit->instruction->isExecuting = false;
                }
            }
        }
    }

    // Faz o estágio de escrita
    void write() {
        writeExecution(addUnits);
        writeExecution(mulUnits);
        writeExecution(swUnits);
    }

   
    // Executa as funções da escrita
    void writeExecution(std::vector<FunctionalUnit*>& unitType) {
        for (size_t i = 0; i < unitType.size(); i++) {
            FunctionalUnit* unit = unitType[i];
            if (unit->busy && !unit->instruction->isExecuting && !unit->instruction->isCompleted) {
                unit->instruction->isCompleted = true;
                unit->busy = false;

                // Atualiza o valor do registrador de destino
                Register* destRegister = getRegister(unit->instruction->dest);
                Register* src1Register = getRegister(unit->instruction->src1);
                Register* src2Register = getRegister(unit->instruction->src2);

                if (unit->instruction->op == "sw" || unit->instruction->op == "lw") {
                    int offset = performOperation("add", std::stoi(unit->instruction->src1), src2Register->value);

                    if (unit->instruction->op == "sw") {
                        // Evitar out of bounds com %, resto de divisão (para testes)
                        cacheMem[offset % cacheMem.size()] = destRegister->value;
                    } else if (unit->instruction->op == "lw") {
                        destRegister->value = cacheMem[offset % cacheMem.size()];
                    }

                } else {
                    destRegister->value = performOperation(unit->instruction->op, src1Register->value, src2Register->value);

                    src1Register->busyRead = false;
                    src1Register->instruction = nullptr;
                }

                // Libera os registradores utilizados pela instrução
                destRegister->busyWrite = false;
                destRegister->instruction = nullptr;

                if (rename.find(destRegister->name) != rename.end()) {
                    returnRenamed(destRegister, i);
                }

                src2Register->busyRead = false;
                src2Register->instruction = nullptr;
            }
        }
    }
    void returnRenamed(Register* target, size_t index) {
        // Renomear os valores na lista de instrução
        for (size_t i = 0; i < instructions.size(); i++) {
            if (instructions[i]->dest == target->name) {
                instructions[i]->dest = rename[target->name];
            }

            if (instructions[i]->src1 == target->name) {
                instructions[i]->src1 = rename[target->name];
            }

            if (instructions[i]->src2 == target->name) {
                instructions[i]->src2 = rename[target->name];
            }
        }

        // Apaga da lista de renomeados
        rename.erase(target->name);
    }

    // Encontra uma unidade funcional disponível
    FunctionalUnit* findAvailableUnit(std::vector<FunctionalUnit*>& units) {
        for (size_t i = 0; i < units.size(); i++) {
            if (!units[i]->busy) {
                return units[i];
            }
        }
        return nullptr;
    }

    // Obtém o registro pelo nome
    Register* getRegister(const std::string& name) {
        for (size_t i = 0; i < registers.size(); i++) {
            if (registers[i]->name == name) {
                return registers[i];
            }
        }
        return nullptr;
    }

    // Executa a operação matemática
    int performOperation(const std::string& op, int src1, int src2) {
        if (op == "add") {
            return src1 + src2;
        } else if (op == "sub") {
            return src1 - src2;
        } else if (op == "mul") {
            return src1 * src2;
        } else if (op == "div") {
            return src1 / src2;
        }
        // Handle unsupported operation or return a default value
        return 0;
    }
};

// Função para liberar a memória alocada para as instruções
void freeInstructions(std::vector<Instruction*>& instructions) {
    for (const auto& instruction : instructions) {
        delete instruction;
    }
}

// Exemplo de uso
int main() {
    // Leitura das instruções a partir de um arquivo de texto
    std::vector<Instruction*> instructions;
    std::ifstream inputFile("instrucoes.txt");  // Nome do arquivo de texto

    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string op, dest, src1, src2;
        iss >> op >> dest >> src1 >> src2;
        instructions.push_back(new Instruction(op, dest, src1, src2));
    }

    inputFile.close();

    // Preset de latências por instrução, quantidade de registradores e quantidade de unidades lógicas
    std::map<std::string, int> values = {
        {"addUnitLatency", 3},
        {"mulUnitLatency", 10},
        {"swUnitLatency", 2},
        {"addUnitQnt", 3},
        {"mulUnitQnt", 2},
        {"swUnitQnt", 2},
        {"registerQnt", 16}
    };

    std::vector<int> cacheMem(32, 2); // Inicialização do cache com valores diferentes

    Tomasulo tomasulo(instructions, values);
    tomasulo.run();

    // Libera a memória alocada para as instruções
    freeInstructions(instructions);

    return 0;
}