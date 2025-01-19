#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

/* 
Estrutura que define os dados que serão armazenados em cada registro.
*/
struct Dados {
    string measure;
    string quantile;
    string area;
    string sex;
    string age;
    string geography;
    string ethnic;
    int value;
};

/* 
Estrutura que representa um contêiner de dados.
*/
struct Container {
    Dados* registros;
    int tamanhoAtual;
    Container* proximo;

    Container(int capacidade) {
        registros = new Dados[capacidade];
        tamanhoAtual = 0;
        proximo = nullptr;
    }

    ~Container() {
        delete[] registros;
    }

    // Função para encontrar o valor mínimo em um contêiner
    int MenorValor() {
        int menor = registros[0].value;
        for (int i = 1; i < tamanhoAtual; i++) {
            if (registros[i].value < menor) {
                menor = registros[i].value;
            }
        }
        return menor;
    }
};

/* 
Classe SequenceSet: gerencia a lista de contêineres e as operações relacionadas.
*/
class SequenceSet {
private:
    Container* cabeca;
    Container* cauda;
    int capacidade;
    int quantidadeConteineres;

public:
    // Construtor
    SequenceSet(int capacidadeInicial = 10) {
        capacidade = capacidadeInicial;
        cabeca = new Container(capacidade);
        cauda = cabeca;
        quantidadeConteineres = 1; // Começamos com um contêiner
    }

    // Destrutor
    ~SequenceSet() {
        Container* atual = cabeca;
        while (atual != nullptr) {
            Container* proximo = atual->proximo;
            delete atual;
            atual = proximo;
        }
    }

    // Método para ler registros de um arquivo CSV
    bool LeituraDeArquivo(string& LerArquivo) {
        ifstream arquivo(LerArquivo);
        if (!arquivo.is_open()) {
            cout << "Erro ao abrir o arquivo: " << LerArquivo << "\n";
            return false;
        }

        string linha;
        getline(arquivo, linha); // Ignora o cabeçalho

        while (getline(arquivo, linha)) {
            if (cauda->tamanhoAtual == capacidade) {
                AdicionarNovoContainer();
            }

            istringstream stream(linha);
            string value;
            Dados& registro = cauda->registros[cauda->tamanhoAtual];

            getline(stream, registro.measure, ',');
            getline(stream, registro.quantile, ',');
            getline(stream, registro.area, ',');
            getline(stream, registro.sex, ',');
            getline(stream, registro.age, ',');
            getline(stream, registro.geography, ',');
            getline(stream, registro.ethnic, ',');
            getline(stream, value, ',');
            registro.value = stoi(value);

            cauda->tamanhoAtual++;
        }

        arquivo.close();
        return true;
    }

    // Método para adicionar um novo contêiner
    void AdicionarNovoContainer() {
        Container* novoContainer = new Container(capacidade);
        cauda->proximo = novoContainer;
        cauda = novoContainer;
        quantidadeConteineres++;
    }

    // Função para exibir os registros e informações do contêiner
    void Exibir() {
        Container* atual = cabeca;
        int numeroDoConteiner = 0;

        if (atual == nullptr) {
            cout << "Nenhum registro disponível para exibição.\n";
            return;
        }

        cout << "Quantidade total de contêineres: " << quantidadeConteineres << "\n";
        cout << "Primeiro contêiner: 0\n";
        cout << "Próximo contêiner disponível: " << quantidadeConteineres << "\n\n";

        while (atual != nullptr) {
            // Informações do contêiner
            cout << "========= Contêiner " << numeroDoConteiner << " =========\n";
            cout << "Quantidade de elementos no contêiner: " << atual->tamanhoAtual << "\n";
            cout << "Número do contêiner: " << numeroDoConteiner << "\n";
            cout << "-------------------------------------\n";

            // Exibe os registros do contêiner
            for (int i = 0; i < atual->tamanhoAtual; i++) {
                cout << "Registro " << i + 1 << ":\n";
                cout << "  Measure: " << atual->registros[i].measure << "\n";
                cout << "  Quantile: " << atual->registros[i].quantile << "\n";
                cout << "  Area: " << atual->registros[i].area << "\n";
                cout << "  Sex: " << atual->registros[i].sex << "\n";
                cout << "  Age: " << atual->registros[i].age << "\n";
                cout << "  Geography: " << atual->registros[i].geography << "\n";
                cout << "  Ethnic: " << atual->registros[i].ethnic << "\n";
                cout << "  Value: " << atual->registros[i].value << "\n";
                cout << "-------------------------------------\n";
            }

            numeroDoConteiner++;
            atual = atual->proximo; // Passa para o próximo contêiner
        }
    }

    // Função de ordenação QuickSort
    void QuickSort() {
        int totalRegistros = 0;
        Container* atual = cabeca;
        
        // Primeiro, conta o número total de registros
        while (atual != nullptr) {
            totalRegistros += atual->tamanhoAtual;
            atual = atual->proximo;
        }

        // Cria um array dinâmico para armazenar todos os registros
        Dados* todosRegistros = new Dados[totalRegistros];
        int index = 0;
        atual = cabeca;

        // Copia os registros para o array
        while (atual != nullptr) {
            for (int i = 0; i < atual->tamanhoAtual; i++) {
                todosRegistros[index++] = atual->registros[i];
            }
            atual = atual->proximo;
        }

        // Realiza o QuickSort no array de registros
        quickSortRecursivo(todosRegistros, 0, totalRegistros - 1);

        // Recria os contêineres e distribui os registros ordenados
        Container* novoCabeca = new Container(capacidade);
        Container* novoCauda = novoCabeca;
        index = 0;
        for (int i = 0; i < totalRegistros; i++) {
            if (novoCauda->tamanhoAtual == capacidade) {
                Container* novoContainer = new Container(capacidade);
                novoCauda->proximo = novoContainer;
                novoCauda = novoContainer;
            }
            novoCauda->registros[novoCauda->tamanhoAtual] = todosRegistros[i];
            novoCauda->tamanhoAtual++;
        }

        // Atualiza a cabeça e a cauda
        cabeca = novoCabeca;
        cauda = novoCauda;

        // Libera a memória do array dinâmico
        delete[] todosRegistros;
    }

    // Função auxiliar para realizar o QuickSort recursivo
    void quickSortRecursivo(Dados* registros, int baixo, int alto) {
        if (baixo < alto) {
            int pivo = Particionar(registros, baixo, alto);
            quickSortRecursivo(registros, baixo, pivo - 1);
            quickSortRecursivo(registros, pivo + 1, alto);
        }
    }

    // Função para particionar os registros
    int Particionar(Dados* registros, int baixo, int alto) {
        int pivo = registros[alto].value;
        int i = baixo - 1;

        for (int j = baixo; j < alto; j++) {
            if (registros[j].value <= pivo) {
                i++;
                swap(registros[i], registros[j]);
            }
        }
        swap(registros[i + 1], registros[alto]);
        return i + 1;
    }
};

/* 
Função principal para interação com o usuário.
*/
int main() {
    SequenceSet operacaoDe; // Criação do SequenceSet
    int opcao;

    cout << "******** Bem-vindo à Criação do SequenceSet **********\n";

    do {
        cout << "\nMenu de opções:\n";
        cout << "1 - Leitura de arquivo\n";
        cout << "2 - Exibir registros\n";
        cout << "3 - Ordenar registros\n";
        cout << "4 - Sair\n";
        cout << "Opção: ";
        cin >> opcao;

        switch (opcao) {
            case 1: {
                cout << "Digite o nome do arquivo a ser lido: ";
                string arquivo;
                cin >> arquivo;

                if (operacaoDe.LeituraDeArquivo(arquivo)) {
                    cout << "Arquivo lido com sucesso!\n";
                } else {
                    cout << "Erro ao ler o arquivo!\n";
                }
                break;
            }
            case 2:
                cout << "\nExibindo registros:\n";
                operacaoDe.Exibir();
                break;
            case 3:
                operacaoDe.QuickSort();
                cout << "Registros ordenados pelo valor.\n";
                break;
            case 4:
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }
    } while (opcao != 4);

    return 0;
}
