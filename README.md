# Algoritmo de Tomasulo
## Descrição
Esse repositório é referente a um simulador do algoritmo de Tomasulo. O programa lê uma entrada de instruções de um arquivo txt e o processa, seguindo os princípios de superescalaridade do algoritmo de Tomasulo. O simulador todo é feito usando **C++** como linguagem principal.
# Funcionamento
## Estruturas de dados
O algoritmo é composto por quatro classes, sendo elas:
### Instruction Class:
Representa uma instrução no contexto do algoritmo de Tomasulo. Armazena informações sobre a operação, registradores de destino e operandos, além de rastrear seu estado durante a execução.
- Atributos:
  - **op:** Operação da instrução (add, sub, mul, div, etc.)
  - **dest:** Registrador de Destino
  - **src1 e src2:** Operandos da instrução
  - **ExecCyles:** Ciclos restantes para conclusão da tarefa
  - **isIssued, isExecuting, isCompleted:** Flags para rastrear o estado da instrução
---
### Classe FunctionalUnit:
Modela uma unidade funcional no hardware. Cada instância representa uma unidade específica, como adição, multiplicação ou armazenamento. Mantém informações sobre seu tipo, latência e se está atualmente ocupada.
- Atributos:
  - **type:** Tipo da unidade funcional (add, mul, sw, etc.)
  - **latency:** Ciclos de latência da unidade funcional
  - **busy:** Indica se a unidade funcional está ocupada
  - **instruction:** Instrução atualmente em execução na unidade
---
### Class Register 
Define um registrador, que é utilizado para armazenar valores temporários ou resultados intermediários. Registra seu nome, valor atual e status de ocupação durante leitura e escrita.
- Atributos:
  - **name:** Nome do registrador
  - **value:** Valor atual do registrador
  - **busyRead e busyWrite:** Flags para indicar se o registrador está sendo lido ou escrito
  - **instruction:** Instrução que está utilizando o registrador
---
## Passo a Passo (Tomasulo e seus métodos)
### Construtor da Classe Tomasulo:
Recebe instruções e valores de configuração (latência, quantidade de unidades, etc.). Cria unidades funcionais (add, mul, sw) e registradores.

### Fase de Emissão (Issue):
- Função issue:
  - Realiza o estágio de emissão para cada ciclo.
  - Verifica se há instruções a serem emitidas e se há unidades funcionais disponíveis.
  - Avalia dependências entre instruções e registra o início da execução.
- Função findAvailableUnit:
  - Encontra uma unidade funcional disponível para a instrução.
- Função renameRegister:
  - Renomeia registradores para evitar conflitos.

### Fase de Execução:

- Função execute:
  - Realiza o estágio de execução, decrementando os ciclos restantes.
### Fase de Escrita:
- Função write:
  - Realiza o estágio de escrita, marcando instruções como concluídas.
  - Atualiza valores de registradores e libera recursos utilizados.
- Função writeExecution:
  - Executa as funções de escrita para cada tipo de unidade funcional.
- Função returnRenamed:
  - Retorna os registradores renomeados às instruções.

### Outras Funções Auxiliares:

- Função getRegister:
  - Obtém o registro pelo nome.
- Função performOperation:
  - Executa operações matemáticas.

## Visualização e Finalização:

- Função showState:
  - Exibe o estado atual das instruções em cada ciclo.
- Função isExecutionComplete:
  - Verifica se a execução está completa.
- Função run:
  - Executa o algoritmo de Tomasulo até a conclusão.
## Como Executar?
Para executar o código, você pode utilizar o seguinte comando no terminal ou prompt de comando:
```console  
g++ new.cpp -o new && "c:\workspace\Tomasulo\"new
```
Este comando compila o programa usando o compilador GNU g++ (g++ new.cpp -o new) e, se a compilação for bem-sucedida, executa o programa resultante ("c:\workspace\Tomasulo\"new). Certifique-se de estar no diretório correto no terminal antes de executar esse comando.

Além disso, o código espera um arquivo de texto chamado "instrucoes.txt" no mesmo diretório do executável, contendo as instruções a serem processadas pelo algoritmo de Tomasulo. Este arquivo de texto deve seguir o formato esperado pelo programa, com cada linha representando uma instrução, por exemplo:
```text
add F1 F2 F3
mul F4 F1 F5
sw F2 100 R1
sub R2 R1 F6
```
Cada linha deve conter a operação seguida pelos registradores de destino e operandos, separados por espaços. O código irá ler esse arquivo para inicializar a simulação do algoritmo de Tomasulo com as instruções fornecidas.
