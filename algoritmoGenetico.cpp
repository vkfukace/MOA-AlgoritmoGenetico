#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <limits>
#include <unistd.h>
#include <fstream>

using namespace std;

#define INF_POS std::numeric_limits<float>::max()
#define POSITION_BASED_CROSSOVER 1
#define ORDER_CROSSOVER 2

class Vertice
{
public:
    float x, y; // coordenadas do vertice

    Vertice() {}

    Vertice(float i, float j)
    {
        x = i;
        y = j;
    }
};

// Remove o elemento de índice i de lista em tempo constante.
// Não preserva a ordem original dos elementos
template <typename T>
void remover(int i, vector<T> &lista)
{
    int ultimoIdx = lista.size() - 1;
    swap(lista[i], lista[ultimoIdx]);
    lista.pop_back();
}

class PCVSolver
{
private:
    vector<Vertice> vertices;
    int numVertices;
    vector<int> melhorCaminho;
    float melhorResultado;
    unsigned int seed; // Seed utilizado para operações com rand()

public:
    // Inicializa a classe com a lista e a quantidade de vértices
    PCVSolver(vector<Vertice> listaVertices, int n)
    {
        vertices = listaVertices;
        numVertices = n;
    }

    // Retorna a distância entre os vértices v1 e v2.
    float distancia(Vertice v1, Vertice v2)
    {
        float deltaX = v2.x - v1.x;
        float deltaY = v2.y - v1.y;

        return sqrt((deltaX * deltaX) + (deltaY * deltaY));
    }

    // Retorna a distância percorrida pelo caminho inteiro.
    float distanciaCaminho(vector<int> &caminho)
    {
        int i;
        float distanciaTotal = 0;
        for (i = 0; i < (int)caminho.size() - 1; i++)
        {
            distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[i + 1]]);
        }
        distanciaTotal += distancia(vertices[caminho[i]], vertices[caminho[0]]);
        return distanciaTotal;
    }

    // Imprime todos os vértices do caminho
    void printCaminho(vector<int> &caminho)
    {
        for (int i = 0; i < (int)caminho.size(); i++)
        {
            cout << caminho[i] << " => ";
        }
        cout << caminho[0] << endl;
    }

    // Imprime as coordenadas de todos os vértices na tela
    void printVertices()
    {
        for (int i = 1; i < numVertices + 1; i++)
        {
            cout << i << " => x: " << vertices[i].x << " y: " << vertices[i].y << "\n";
        }
        cout << endl;
    }

    // Imprime os fitness de todos os indivíduos da população
    void printFitness(vector<float> &fitness)
    {
        for (int i = 0; i < fitness.size(); i++)
        {
            cout << "Fitness "<< i << ": " << fitness[i] << endl;
        }
        cout << endl;
    }

    void caminhoValido(vector<int> &caminho){
        vector<bool> visitado(numVertices+1, false);
        int verticeAtual;
        for(int i = 0; i < caminho.size(); i++){
            verticeAtual = caminho[i];
            if(!visitado[verticeAtual])
                visitado[verticeAtual] = true;
            else
                cout << "Vertice " << verticeAtual << "duplicado, indice:" << i << endl;
        }
        for(int i = 1; i < numVertices + 1; i++){
            if(!visitado[i])
                cout << "Vertice " << i << "nao foi adicionado" << endl;
        }
    }

    void populacaoValida(vector<vector<int>> &populacao){
        for(int i = 0; i< populacao.size(); i++){
            cout << "Validando " << i << endl;
            caminhoValido(populacao[i]);
        }
    }

    // Define a seed utilizada em operações aleatórias
    void setSeed(unsigned int seed) {
        PCVSolver::seed = seed;
        srand(seed);
    }

    // Retorna o índice do vizinho mais próximo de verticeAtual
    // Retorna também o índice deste vizinho na lista de vértices disponíveis;
    int vizinhoMaisProximo(int verticeAtual, vector<int> &idxVerticesDisponiveis, int &idxListaDisponiveis)
    {
        float distanciaCalc, tamanhoMenorAresta;
        int idxMaisProximo;

        // tamanhoMenorAresta inicialmente possui o maior valor de float possível
        tamanhoMenorAresta = INF_POS;
        for (int i = 0; i < (int)idxVerticesDisponiveis.size(); i++)
        {
            distanciaCalc = distancia(vertices[verticeAtual], vertices[idxVerticesDisponiveis[i]]);
            if (distanciaCalc < tamanhoMenorAresta)
            {
                tamanhoMenorAresta = distanciaCalc;
                idxMaisProximo = idxVerticesDisponiveis[i];
                idxListaDisponiveis = i;
            }
        }
        return idxMaisProximo;
    }

    // Aplica o algoritmo do vizinho mais próximo para o PCV.
    // Retorna o caminho gerado em caminhoRetorno, e a distância do caminho em distanciaRetorno.
    void solveVizinhoMaisProximo(int verticeInicial, vector<int> &caminhoRetorno, float &distanciaRetorno)
    {
        // Inicializa a lista de vértices disponíveis
        vector<int> idxVerticesDisponiveis;
        for (int i = 1; i <= numVertices; i++)
        {
            idxVerticesDisponiveis.push_back(i);
        }
        int idxMaisProximo, idxRemover;
        caminhoRetorno = vector<int>(0);
        distanciaRetorno = 0;

        int idxVerticeAtual = verticeInicial;
        remover(idxVerticeAtual - 1, idxVerticesDisponiveis);
        while (idxVerticesDisponiveis.size() > 0)
        {
            caminhoRetorno.push_back(idxVerticeAtual);
            // Seleciona o vizinho mais próximo
            idxMaisProximo = vizinhoMaisProximo(idxVerticeAtual, idxVerticesDisponiveis, idxRemover);
            idxVerticeAtual = idxMaisProximo;
            remover<int>(idxRemover, idxVerticesDisponiveis);
        }
        caminhoRetorno.push_back(idxVerticeAtual);
        distanciaRetorno = distanciaCaminho(caminhoRetorno);
    }

    float calculoCustoInsercao(vector<int> &ciclo, int i, int j, int k)
    {
        float d_ik = distancia(vertices[i], vertices[k]);
        float d_kj = distancia(vertices[k], vertices[j]);
        float d_ij = distancia(vertices[i], vertices[j]);
        return d_ik + d_kj - d_ij;
    }

    // Aplica o algoritmo random insertion para o PCV.
    // Retorna o caminho gerado em caminhoRetorno, e a distância do caminho em distanciaRetorno.
    void solveRandomInsertion(vector<int> &caminhoRetorno, float &distanciaRetorno)
    {
        // Inicializa a lista de vértices disponíveis.
        vector<int> idxVerticesDisponiveis;
        for (int i = 1; i <= numVertices; i++)
            idxVerticesDisponiveis.push_back(i);
        int numVerticesDisponiveis = (int)idxVerticesDisponiveis.size();

        // Inicializa o ciclo vazio.
        // proxCiclo funciona de forma diferente de melhorCaminho.
        // No melhorCaminho, o elemento após melhorCaminho[i] é melhorCaminho[i+1],
        // no proxCiclo, o elemento após proxCiclo[i] é proxCiclo[proxCiclo[i]].
        vector<int> proxCiclo(numVertices + 1, -1);

        // Escolhe vértice inicial aleatório.
        int idxRandom = rand() % numVerticesDisponiveis;
        int verticeInicial = idxVerticesDisponiveis[idxRandom];
        remover<int>(idxRandom, idxVerticesDisponiveis);
        numVerticesDisponiveis--;

        // Define um ciclo inicial com um único vértice.
        proxCiclo[verticeInicial] = verticeInicial;
        int tamanhoCiclo = 1;

        while (numVerticesDisponiveis > 0)
        {
            // Escolhe outro vértice aleatório, chamado k.
            idxRandom = rand() % numVerticesDisponiveis;
            int k = idxVerticesDisponiveis[idxRandom];
            numVerticesDisponiveis--;
            remover<int>(idxRandom, idxVerticesDisponiveis);

            int i = verticeInicial, j, iInsercao, jInsercao;
            float melhorCustoInsercao = INF_POS;
            // Busca todas as arestas (i, j) do ciclo, e escolhe aquela com o menor custo de inserção de k.
            for (int cont = 0; cont < tamanhoCiclo; cont++)
            {
                j = proxCiclo[i];
                float custoInsercao = calculoCustoInsercao(proxCiclo, i, j, k);
                if (custoInsercao < melhorCustoInsercao)
                {
                    iInsercao = i;
                    jInsercao = j;
                    melhorCustoInsercao = custoInsercao;
                }
                i = j;
            }
            proxCiclo[k] = jInsercao;
            proxCiclo[iInsercao] = k;
            tamanhoCiclo++;
        }

        // Transfere o ciclo para o caminhoRetorno, e calcula a distância
        int i = verticeInicial;
        caminhoRetorno = vector<int>(0);
        for (int cont = 0; cont < tamanhoCiclo; cont++)
        {
            caminhoRetorno.push_back(i);
            i = proxCiclo[i];
        }
        distanciaRetorno = distanciaCaminho(caminhoRetorno);
    }

    // Calcula a diferença entre as distâncias do caminho antes
    // e depois da operação de troca 2-opt entre os vértices
    // (caminho[i-1], caminho[i]) e (caminho[j], caminho[j+1]).
    // Caso a diferença seja negativa, o caminho em que a troca foi
    // realizada possui distância menor.
    // Presume que i < j.
    float diferencaTroca2Opt(vector<int> &caminho, int i, int j)
    {
        int iMenos = (numVertices + ((i - 1) % numVertices)) % numVertices;
        int jMais = (j + 1) % numVertices;
        float antes1 = distancia(vertices[caminho[i]], vertices[caminho[iMenos]]);
        float antes2 = distancia(vertices[caminho[j]], vertices[caminho[jMais]]);
        float depois1 = distancia(vertices[caminho[iMenos]], vertices[caminho[j]]);
        float depois2 = distancia(vertices[caminho[i]], vertices[caminho[jMais]]);

        return (depois1 + depois2) - (antes1 + antes2);
    }

    // Realiza a troca 2-opt no caminho dado.
    // Faz a troca dos vértices (caminho[i-1], caminho[i]) e
    // (caminho[j], caminho[j+1]), através da inversão dos
    // elementos de caminho[i] até caminho[j].
    void trocar2Opt(vector<int> &caminho, int i, int j)
    {
        while (i < j)
        {
            swap(caminho[i], caminho[j]);
            i++;
            j--;
        }
    }

    // Aplica o algoritmo 2-opt para o PCV.
    // Busca todas as soluções vizinhas ao caminhoInicial e acha a melhor.
    // Após a operação, atualiza caminhoInicial, junto de sua distância
    void busca2OptBestImprovement(vector<int> &caminhoInicial, float &distanciaCaminhoInicial)
    {
        float diferencaDistancia, melhorDiferencaDistancia= 0;
        int i, j, melhor_i, melhor_j;

        // Seleciona a melhor entre as soluções vizinhas.
        for (i = 1; i < numVertices; i++)
        {
            for (j = i + 1; j < numVertices; j++)
            {
                diferencaDistancia = diferencaTroca2Opt(caminhoInicial, i, j);
                // Se a troca resulta em uma redução da distância
                if (diferencaDistancia < melhorDiferencaDistancia)
                {
                    melhor_i = i;
                    melhor_j = j;
                    melhorDiferencaDistancia = diferencaDistancia;
                }
            }
        }
        // Se existe uma solução vizinha que resulta em redução da distância
        if (melhorDiferencaDistancia < 0)
        {
            trocar2Opt(caminhoInicial, melhor_i, melhor_j);
            distanciaCaminhoInicial += melhorDiferencaDistancia;
        }
    }

    // #######################################################
    // TODO: Gera a população inicial do algoritmo genético utilizando o ???????????.
    // #######################################################
    void gerarPopulacaoInicial(vector<vector<int>> &populacao, vector<float> &fitness){
        vector<int> caminhoRetorno;
        float distanciaRetorno;

        for(int i = 0; i < populacao.size(); i++){
            // solveRandomInsertion(caminhoRetorno, distanciaRetorno);

            int verticeInicial = (rand() % numVertices) + 1;
            solveVizinhoMaisProximo(verticeInicial, caminhoRetorno, distanciaRetorno);

            populacao[i] = caminhoRetorno;
            fitness[i] = distanciaRetorno;
            // cout << "distancia " << i << ": " << fitness[i] << endl;
        }
    }

    // Busca a população por resultados melhores do que o armazenado em melhorResultado.
    // Retorna true caso ache um resultado melhor, false caso contrário
    bool atualizaMelhorResultado(vector<vector<int>> &populacao, vector<float> &fitness){
        bool melhorou = false;
        int idxMelhor;
        for(int i = 0; i < fitness.size(); i++){
            if(fitness[i] < melhorResultado){
                idxMelhor = i;
                melhorResultado = fitness[i];
                melhorou = true;
            }
        }
        if (melhorou) melhorCaminho = populacao[idxMelhor];
        return melhorou;
    }

    // Retorna a soma das distâncias da população
    float somaDistanciasPopulacao(vector<float> &fitness){
        float total = 0;
        for(int i = 0; i < fitness.size(); i++){
            total += fitness[i];
        }
        return total;
    }

    // Retorna o índice do indivíduo que foi sorteado na roleta
    int resultadoRoleta(float somaDistancias, vector<float> &fitness){
        float random = ((double) rand() / (RAND_MAX)) * (fitness.size() - 1);
        int i;
        double somaAtual = 0;
        double porcentagemAtual;
        // cout << "#####################" << endl;
        // cout << "size: " << fitness.size() << endl;
        // cout << "random: " << random << endl;
        for(i = 0; i < fitness.size(); i++){
            // 1 - (fitness[i] / somaDistancias) é inversamente proporcional à distância do caminho i. 
            // Ou seja, os menores caminhos têm mais chances de serem escolhidos
            somaAtual += 1 - (fitness[i] / somaDistancias);
            // cout << "i " << i << ": " << somaAtual << endl;
            if (random < somaAtual) break;
        }
        // cout << "escolhido: " << i << endl;
        return i;
    }

    // Atualiza o estado da população a ser sorteada, removendo o indivíduo sorteado.
    void atualizaRoleta(int idxSorteado, float &somaDistancias, vector<vector<int>> &populacao, vector<float> &fitness){
        somaDistancias -= fitness[idxSorteado];
        remover<float>(idxSorteado, fitness);
        remover<vector<int>>(idxSorteado, populacao);
    }

    // Retorna os índices dos k indivíduos da população que participarão na criação 
    // da próxima geração.
    vector<int> selecaoRoleta(vector<vector<int>> populacao, int k, vector<float> fitness){
        vector<int> idxsSelecionados(0);
        float somaDistancias = somaDistanciasPopulacao(fitness);
        int idxSorteado;

        for(int i = 0; i < k; i++){
            idxSorteado = resultadoRoleta(somaDistancias, fitness);
            idxsSelecionados.push_back(idxSorteado);
            atualizaRoleta(idxSorteado, somaDistancias, populacao, fitness);
        }
        return idxsSelecionados;
    }

    // Guarda os índices dos elementos de caminho em indices.
    // Ex:  caminho = {1, 3, 5, 4, 2}
    //      indices[5] contém o índice do elemento 5 => 2
    // Assume que indices já tem espaço suficiente alocado.
    void inicializaListaIndices(vector<int> &indices, vector<int> &caminho){
        for(int i = 0; i < caminho.size(); i++){
            indices[caminho[i]] = i;
        }
    }

    // Aplica a operação de Order Crossover (OX1) em dois indivíduos pais
    // para gerar dois filhos.
    vector<vector<int>> orderCrossover(vector<int> &pai1, vector<int> &pai2){
        vector<vector<int>> pais{pai1, pai2};
        vector<vector<int>> filhos(2, vector<int>(numVertices, -1));
        // indicesPais[0][3] contém o índice do vértice 3 no pai 0
        vector<vector<int>> indicesPais(2, vector<int>(numVertices + 1, -1));
        vector<vector<bool>> filhoContem(2, vector<bool>(numVertices + 1, false));
        int i = rand() % (numVertices - 1);
        int j = i + (1 + rand() % (numVertices - i - 1));

        // cout << "i: " << i << " j: " << j << endl;

        // Copia pai[i:j] para filho[i:j] e atualiza a lista de vértices contidos em filho
        for(int k = i; k <= j; k++){
            filhos[0][k] = pais[0][k];
            filhoContem[0][pais[0][k]] = true;
            filhos[1][k] = pais[1][k];
            filhoContem[1][pais[1][k]] = true;
        }
        
        for(int v = 0; v < 2; v++){
            int index = (j+1) % numVertices;
            int outro = (v+1) % 2;
            inicializaListaIndices(indicesPais[outro], pais[outro]);
            while (index != i) {
                int cidade = pais[outro][index];
                while(filhoContem[v][cidade]){
                    int aux = indicesPais[outro][cidade];
                    cidade = pais[outro][(aux+1) % numVertices];
                }
                filhos[v][index] = cidade;
                filhoContem[v][cidade] = true;
                index = (index + 1) % numVertices;
            }
        }

        return filhos;
    }
    
    // Realiza a operação de cruzamento, utilizando o operador de cruzamento Position Based Crossover (POS)
    // Guarda a população de indivíduos filhos na variável filhos.
    void cruzamentoOX1(vector<vector<int>> &populacao, vector<int> &idxsReproducao, vector<vector<int>> &filhos){
        filhos = vector<vector<int>>(0);
        vector<vector<int>> resultado;
        int x1, x2;
        for(int i = 0; i < (int)(populacao.size() / 2); i++){
            x1 = rand() % idxsReproducao.size();
            x2 = rand() % idxsReproducao.size();
            // cout << "x1: " << x1 << " x2: " << x2 << endl;
            resultado = orderCrossover(populacao[idxsReproducao[x1]], populacao[idxsReproducao[x2]]);
            filhos.push_back(resultado[0]);
            filhos.push_back(resultado[1]);
        }
    }

    // Inicializa a lista de posições aleatórias para serem utilizadas no Position Based Crossover.
    // O tamanho da lista é aleatório, mas contém no máximo (numVertices/2) elementos.
    void inicializaListaPosicoes(vector<int> &listaPosicoes){
        listaPosicoes = vector<int>(0);
        vector<int> posicoesDiponiveis;
        for(int i = 0; i < numVertices; i++){
            posicoesDiponiveis.push_back(i);
        }
        int n = rand() % (int)(numVertices/2);

        for(int i = 0; i < n; i++){
            int idxRandom = rand() % posicoesDiponiveis.size();
            listaPosicoes.push_back(posicoesDiponiveis[idxRandom]);
            remover<int>(idxRandom, posicoesDiponiveis);
        }
    }

    // Aplica a operação de Position Based Crossover (POS) em dois indivíduos pais
    // para gerar dois filhos.
    vector<vector<int>> positionBasedCrossover(vector<int> &pai1, vector<int> &pai2){
        vector<vector<int>> pais{pai1, pai2};
        vector<vector<int>> filhos(2, vector<int>(numVertices, -1));
        vector<vector<int>> indicesPais(2, vector<int>(numVertices + 1, -1));
        vector<int> listaPosicoes;

        inicializaListaPosicoes(listaPosicoes);
        
        for(int i = 0; i < 2; i++){
            inicializaListaIndices(indicesPais[i], pais[i]);
            filhos[i] = pais[i];
        }

        // cout << endl << "Lista Pos: ";
        for(int pos: listaPosicoes){
            // cout << pos << " ";
            filhos[0][pos] = pais[1][pos];
            filhos[0][indicesPais[0][filhos[0][pos]]] = pais[0][pos];
            filhos[1][pos] = pais[0][pos];
            filhos[1][indicesPais[1][filhos[1][pos]]] = pais[1][pos];
        }

        return filhos;
    }

    // Realiza a operação de cruzamento, utilizando o operador de cruzamento Position Based Crossover (POS)
    // Guarda a população de indivíduos filhos na variável filhos.
    void cruzamentoPOS(vector<vector<int>> &populacao, vector<int> &idxsReproducao, vector<vector<int>> &filhos){
        filhos = vector<vector<int>>(0);
        vector<vector<int>> resultado;
        int x1, x2;
        for(int i = 0; i < (int)(populacao.size() / 2); i++){
            x1 = rand() % idxsReproducao.size();
            do {
                x2 = rand() % idxsReproducao.size();
            } while (x2 == x1);
            // cout << "x1: " << x1 << " x2: " << x2 << endl;
            resultado = positionBasedCrossover(populacao[idxsReproducao[x1]], populacao[idxsReproducao[x2]]);
            filhos.push_back(resultado[0]);
            filhos.push_back(resultado[1]);
        }
    }

    // Realiza a operação de cruzamento, utilizando os indivíduos cujos índices pertencem a idxsReproducao.
    // Guarda a população de indivíduos filhos na variável filhos.
    // operacao pode receber um de dois valores:
    // - POSITION_BASED_CROSSOVER
    // - ORDER_CROSSOVER
    // Caso não possua um destes valores, recebe POSITION_BASED_CROSSOVER por padrão;
    void cruzamento(vector<vector<int>> &populacao, vector<int> &idxsReproducao, vector<vector<int>> &filhos, int operacao){
        switch(operacao){
            case POSITION_BASED_CROSSOVER:
                cruzamentoPOS(populacao, idxsReproducao, filhos);
                break;
            case ORDER_CROSSOVER:
                cruzamentoOX1(populacao, idxsReproducao, filhos);
                break;
            default:
                cruzamentoPOS(populacao, idxsReproducao, filhos);
        }
    }

    // Retorna o índice do pior fitness
    int selecionaPiorFitness(vector<float> fitness){
        float piorFitness = fitness[0];
        int piorIdx = 0;
        for(int i = 1; i<fitness.size(); i++){
            if(fitness[i] > piorFitness){
                piorFitness = fitness[i];
                piorIdx = i;
            }
        }
        return piorIdx;
    }

    // Realiza a operação de cruzamento, utilizando a técnica Steady Stated.
    // Para cada indivíduo i em filhos, i entra na população se tiver uma aptidão melhor que o pior indivíduo.
    void manutencaoSteadyStated(vector<vector<int>> &populacao, vector<float> &fitness, vector<vector<int>> &filhos){
        int idxPiorFitness = selecionaPiorFitness(fitness);
        float fitnessFilho;
        for(int i = 0; i < filhos.size(); i++){
            fitnessFilho = distanciaCaminho(filhos[i]);
            if(fitnessFilho < fitness[idxPiorFitness]){
                fitness[idxPiorFitness] = fitnessFilho;
                populacao[idxPiorFitness] = filhos[i];
                idxPiorFitness = selecionaPiorFitness(fitness);
            }
        }
    }

    // Realiza mutações na população, baseado em taxaMutacao, utilizando trocas 2-opt.
    void mutacao(vector<vector<int>> &populacao, vector<float> &fitness, float taxaMutacao){
        for(int x = 0; x < populacao.size(); x++){
            float random =  ((float) rand() / (RAND_MAX));
            if(random <= taxaMutacao){
                // cout << "Mutacao em " << x << endl;
                int i = rand() % (numVertices - 1);
                int j = i + (1 + rand() % (numVertices - i - 1));
                fitness[x] += diferencaTroca2Opt(populacao[x], i, j);
                trocar2Opt(populacao[x], i, j);
            }
        }
    }

    // Realiza a busca local para todos os indivíduos da população.
    // A busca local é feita através da operação 2-opt Best Improvement
    void buscaLocal(vector<vector<int>> &populacao, vector<float> &fitness){
        for(int i = 0; i < populacao.size(); i++){
            // cout << "\tBusca local em " << i << endl;
            busca2OptBestImprovement(populacao[i], fitness[i]);
        }
    }

    // Aplica o algoritmo genético para o PCV.
    // #######################################################
    // TODO: falar sobre os parametros
    // #######################################################
    float solveAlgoritmoGenetico(int tamPopulacao, float taxaMutacao, int operacaoCrossover, int maxIteracoes, string nomeArqSaida){
        vector<vector<int>> populacao(tamPopulacao, vector<int>(numVertices)), filhos;
        vector<float> fitness(tamPopulacao);
        melhorResultado = INF_POS;
        int tamPopulacaoReproducao = ceil(tamPopulacao*0.3);
        vector<int> idxsReproducao;
        bool resultadoMelhorou = true;
        int iterSemMelhora = 0, maxIterSemMelhora = 5;

        clock_t tempoInicial = clock();
        ofstream arq;
        arq.open(nomeArqSaida, ios::out);
        arq << "Utilizando os seguintes parâmetros:" << endl;
        arq << "Tamanho da população: " << tamPopulacao << endl;
        arq << "Taxa de Mutação: " << taxaMutacao << endl;
        arq << "Operação de Crossover: " << (operacaoCrossover == 1 ? "POS" : "OX1") << endl;
        arq << "Máximo de Iterações: " << maxIteracoes << endl << endl;

        gerarPopulacaoInicial(populacao, fitness);
        atualizaMelhorResultado(populacao, fitness);

        for (int geracao = 0; geracao < maxIteracoes and iterSemMelhora < maxIterSemMelhora; geracao++) {
            idxsReproducao = selecaoRoleta(populacao, tamPopulacaoReproducao, fitness);
            cruzamento(populacao, idxsReproducao, filhos, operacaoCrossover);
            manutencaoSteadyStated(populacao, fitness, filhos);
            mutacao(populacao, fitness, taxaMutacao);
            buscaLocal(populacao, fitness);
            resultadoMelhorou = atualizaMelhorResultado(populacao, fitness);
            resultadoMelhorou ? iterSemMelhora = 0 : iterSemMelhora++;

            arq << "Geração " << geracao << ": " << melhorResultado << endl;
            cout << "Geração " << geracao << ": " << melhorResultado << endl;
        }

        clock_t tempoFinal = clock();
        float tempoTotal = (tempoFinal - tempoInicial) / (float)CLOCKS_PER_SEC;
        arq << "Resultado Obtido: " << melhorResultado << endl;
        arq << "Tempo: " << tempoTotal << "s" << endl;

        return melhorResultado;
    }
};

// Separa a string em uma lista de tokens
// Tokens são trechos da string original separados por um espaço
vector<string> tokenizar(string str)
{
    vector<string> listaTokens;
    int i;
    string token = "";
    for (i = 0; i < (int)str.length(); i++)
    {
        if (str[i] == ' ')
        {
            if (!token.empty())
            {
                listaTokens.push_back(token);
                token = "";
            }
        }
        else
        {
            token = token + str[i];
        }
    }
    if (!token.empty())
    {
        listaTokens.push_back(token);
    }

    return listaTokens;
}

// Inicializa a lista de vértices utilizando linhas de entrada
// da linha de comando.
// Assume que a entrada está formatada de acordo com os exemplos.
// Inicializa a posição 0 da linha de vértices com um vértice vazio.
void inicializarPorTerminal(vector<Vertice> &listaVertices, int &tamanhoLista)
{
    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(cin, linhaEntrada);
    }

    // Declaração dos vértices
    // Primeiro vértice é vazio
    listaVertices.push_back(Vertice());
    tamanhoLista = 0;
    getline(cin, linhaEntrada);
    // Enquanto não achar um "EOF"
    while (linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        tamanhoLista++;
        getline(cin, linhaEntrada);
    }
}

// Inicializa a lista de vértices utilizando um arquivo
// Assume que a entrada está formatada de acordo com os exemplos.
// Inicializa a posição 0 da linha de vértices com um vértice vazio.
// Retorna true caso o arquivo exista, false caso contrário
bool inicializarPorArquivo(string nomeArquivo, vector<Vertice> &listaVertices, int &tamanhoLista)
{
    ifstream arq(nomeArquivo);

    if (!arq.good())
        return false;

    string linhaEntrada;
    vector<string> listaTokens;

    for (int i = 0; i < 6; i++)
    {
        getline(arq, linhaEntrada);
    }

    // Declaração dos vértices
    // Primeiro vértice é vazio
    listaVertices.push_back(Vertice());
    tamanhoLista = 0;
    // Enquanto não for o fim do arquivo e não achar um "EOF"
    while (getline(arq, linhaEntrada) && linhaEntrada.find("EOF") == string::npos)
    {
        listaTokens = tokenizar(linhaEntrada);
        listaVertices.push_back(Vertice(stof(listaTokens[1]), stof(listaTokens[2])));
        tamanhoLista++;
    }

    return true;
}

int main(int argc, char **argv)
{
    vector<Vertice> listaVertices;
    int tamanhoLista;
    
    // Valores Padrão
    float resultado, taxaMutacao = 0.15;
    int tamPopulacao = 30, maxIteracoes = 1000, operacaoCrossover = POSITION_BASED_CROSSOVER;
    unsigned int seed = time(NULL);
    string nomeArqEntrada = "", nomeArqSaida = "saida.txt";
    int opt;

    // Pegar argumentos da linha de comando
    while((opt = getopt(argc, argv, "i:o:p:m:n:c:s:")) != -1) 
    { 
        switch(opt) 
        {
            case 'i': 
                nomeArqEntrada = optarg;
                break;
            case 'o': 
                nomeArqSaida = optarg;
                break;
            case 'p': 
                tamPopulacao = stoi(optarg);
                break;
            case 'm': 
                taxaMutacao = stof(optarg);
                break;
            case 'n': 
                maxIteracoes = stoi(optarg);
                break;
            case 'c': 
                if(strcmp(optarg, "ox1") == 0 or strcmp(optarg, "OX1") == 0)
                    operacaoCrossover = ORDER_CROSSOVER;
                else if(strcmp(optarg, "pos") == 0 or strcmp(optarg, "POS") == 0)
                    operacaoCrossover = POSITION_BASED_CROSSOVER;
                else
                    cout << "Operacao " << optarg << " invalida, utilizando crossover POS";
                break;
            case 's': 
                seed = stoi(optarg);
                break;
            case '?': 
                printf("unknown option: %c\n", optopt);
                break; 
        } 
    }

    // Inicialização da lista de vértices
    if (nomeArqEntrada != "")
    {
        if (inicializarPorArquivo(nomeArqEntrada, listaVertices, tamanhoLista))
        {
            cout << "Inicialização completa" << endl;
        }
        else
        {
            cout << "Erro na leitura do arquivo" << endl;
            return 0;
        }
    }
    else
    {
        cout << "Insira o caso de teste" << endl;
        inicializarPorTerminal(listaVertices, tamanhoLista);
    }

    cout << "Executando..." << endl;

    PCVSolver pcvSolver(listaVertices, tamanhoLista);
    
    pcvSolver.setSeed(seed);
    resultado = pcvSolver.solveAlgoritmoGenetico(tamPopulacao, taxaMutacao, operacaoCrossover, maxIteracoes, nomeArqSaida);

    cout << "Resultado Obtido: ";
    cout << resultado << "\n";

    return 0;
}