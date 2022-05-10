# MOA-AlgoritmoGenetico

## Modelagem e Otimização Algorítmica - CC UEM 2022

Implementação de algoritmo baseado na heurística Algoritmo Genético com Busca Local para o Problema do Caixeiro Viajante.

Vinícius Kenzo Fukace - RA115672
Felipe Roveroni de Lima - RA112675

### Execução

Compilar com "g++ algoritmoGenetico.cpp -o algoritmoGenetico"

Comando de execução: "./algoritmoGenetico [-i \<param1\> | -o \<param2\> | -p \<param3\> | -m \<param4\> | -n \<param5\> | -c \<param6\> | -s \<param6\> | -t]"

Descrição dos Argumentos:
* -i \<arqEntrada\> = Define o nome do arquivo de entrada que contém o caso de teste. Caso não seja fornecido, o usuário deverá inserir o caso de teste no terminal.
* -o \<arqSaida\> = Define o nome do arquivo de saída que contém informações sobre a execução. Caso não seja fornecido, o nome padrão é "saida.txt".
* -p \<tamanhoPopulacao\> = Define o tamanho da população utilizada no algoritmo. O valor padrão é 30.
* -m \<taxaMutacao\> = Define o valor da taxa de mutação utilizada no algoritmo. Deve possuir valor entre 0 e 1. O valor padrão é 0.15.
* -c \<operadorCruzamento\> = Define o operador de cruzamento utilizado pelo algoritmo. O valor padrão é CX.
  - [-c OX1] = Operador Order Crossover (OX1);
  - [-c CX] = Operador Cycle Crossover (CX).
* -n \<maxIteracoes\> = Define o valor máximo de iterações do algoritmo. O valor padrão é 1000.
* -s \<seed\> = Define a seed utilizada para as operações que utilizam aleatoriedade. Utilizado para depuração.
* -t = Inicia o modo de testes automáticos. Usa valores pré-definidos para tamanho de população e taxa de mutação e executa todas as combinações destes valores.

Presume-se que os casos de teste seguem o formato descrito em http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf, com a última linha contendo "EOF".

O tempo de execução dependende do tamanho do caso de teste.
