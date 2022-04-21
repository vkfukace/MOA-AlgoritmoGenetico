#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <limits>
#include <fstream>

#define INF_POS std::numeric_limits<float>::max();

using namespace std;

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
    float melhorDistancia;
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
        for (int j = 0; j < (int)caminho.size(); j++)
        {
            cout << caminho[j] << " => ";
        }
        cout << caminho[0] << endl;
    }

    // Imprime as coordenadas de todos os vértices na tela
    void printVertices()
    {
        for (int j = 1; j < numVertices + 1; j++)
        {
            cout << j << " => x: " << vertices[j].x << " y: " << vertices[j].y << "\n";
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
    // Assume que i < j.
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

    // Retorna a soma das distâncias da população
    float somaDistanciasPopulacao(vector<float> &fitness){
        float total = 0;
        for(int i = 0; i < fitness.size(); i++){
            total += fitness[i];
        }
        return total;
    }

    // Retorna o índice do indivíduo que foi sorteado na roleta
    int resultadoRoleta(float random, float somaDistancias, vector<float> &fitness){
        int i;
        float somaAtual = 0;
        float porcentagemAtual;
        // cout << "#####################" << endl;
        // cout << "size: " << fitness.size() << endl;
        // cout << "random: " << random << endl;
        for(i = 0; i < fitness.size(); i++){
            somaAtual += fitness[i];
            porcentagemAtual = somaAtual/somaDistancias;
            // cout << "i " << i << ": " << porcentagemAtual << endl;
            if (random < porcentagemAtual) break;
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

    // Seleciona os k indivíduos da população que participarão na criação 
    // da próxima geração e os guarda em populacao.
    void selecaoRoleta(vector<vector<int>> &populacao, int k, vector<float> &fitness){
        vector<vector<int>> populacaoSelecionada(0);
        float somaDistancias = somaDistanciasPopulacao(fitness);
        float random; // Recebe valores entre 0 e 1
        int idxSorteado;

        for(int i = 0; i < k; i++){
            random = ((float) rand() / (RAND_MAX));
            idxSorteado = resultadoRoleta(random, somaDistancias, fitness);
            populacaoSelecionada.push_back(populacao[idxSorteado]);
            atualizaRoleta(idxSorteado, somaDistancias, populacao, fitness);
        }
        populacao = populacaoSelecionada;
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
        vector<vector<int>> indicesPais(2, vector<int>(numVertices + 1, -1));
        vector<vector<bool>> filhoContem(2, vector<bool>(numVertices + 1, false));
        int i = rand() % (numVertices - 1);
        int j = i + (1 + rand() % (numVertices - i - 1));

        cout << endl << "i: " << i << " j: " << j;

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

    // Inicializa a lista de posições aleatórias para serem utilizadas no POS.
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

        cout << endl << "Lista Pos: ";
        for(int pos: listaPosicoes){
            cout << pos << " ";
            filhos[0][pos] = pais[1][pos];
            filhos[0][indicesPais[0][filhos[0][pos]]] = pais[0][pos];
            filhos[1][pos] = pais[0][pos];
            filhos[1][indicesPais[1][filhos[1][pos]]] = pais[1][pos];
        }

        return filhos;
    }

    // Aplica o algoritmo genético para o PCV.
    // #######################################################
    // TODO: falar sobre os parametros
    // #######################################################
    float solveAlgoritmoGenetico(int tamPopulacao, float taxaSelecao, float taxaMutacao, int maxIteracoes){
        vector<vector<int>> populacao(tamPopulacao, vector<int>(numVertices));
        vector<vector<int>> filhos(2);
        vector<float> fitness(tamPopulacao);
        unsigned int geracao;
        int tamPopulacaoReproducao = ceil(tamPopulacao*taxaSelecao);

        gerarPopulacaoInicial(populacao, fitness);
        // #######################################################
        // TODO: adicionar outro criterio de parada
        // #######################################################
        for (geracao = 0; geracao < maxIteracoes; geracao++) {
            selecaoRoleta(populacao, tamPopulacaoReproducao, fitness);

            int x1 = rand() % tamPopulacaoReproducao, x2 = rand() % tamPopulacaoReproducao;
            cout << endl << "pai 0: ";
            printCaminho(populacao[x1]);
            cout << endl << "pai 1: ";
            printCaminho(populacao[x2]);

            cout  << endl << "OX1";
            filhos = orderCrossover(populacao[x1], populacao[x2]);
            for(int i = 0; i < 2; i++){
                cout << endl << "filho " << i << ": ";
                printCaminho(filhos[i]);
            }

            cout  << endl << "POS";
            filhos = positionBasedCrossover(populacao[x1], populacao[x2]);
            for(int i = 0; i < 2; i++){
                cout << endl << "filho " << i << ": ";
                printCaminho(filhos[i]);
            }
        }
        // for (int i = 0; i < fitness.size(); i++) {
        //     cout << i << ": "<< fitness[i] << endl;
        // }
        return 0.0;
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

    if (argc > 1)
    {
        if (inicializarPorArquivo(argv[1], listaVertices, tamanhoLista))
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

    PCVSolver pcvSolver(listaVertices, tamanhoLista);

    float resultado;
    clock_t tempoInicial = clock();
    cout << "Executando..." << endl;

    // pcvSolver.setSeed(1000);
    pcvSolver.setSeed(time(NULL));
    resultado = pcvSolver.solveAlgoritmoGenetico(20, 0.25, 0.05, 1);

    clock_t tempoFinal = clock();
    float tempoTotal = (tempoFinal - tempoInicial) / (float)CLOCKS_PER_SEC;
    cout << "Execução finalizada" << endl;
    cout << "Tempo: " << tempoTotal << "s" << endl;
    cout << "Resultado: ";
    cout << resultado << "\n";

    return 0;
}