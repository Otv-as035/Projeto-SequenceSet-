#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>  // Para usar setw
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

        // Após a leitura, ordena os registros
        QuickSort();
        return true;
    }

    // Método para adicionar um novo contêiner
    void AdicionarNovoContainer() {
        Container* novoContainer = new Container(capacidade);
        cauda->proximo = novoContainer;
        cauda = novoContainer;
        quantidadeConteineres++;
    }

    // Função de exibição com formatação
    void Exibir() {
        Container* atual = cabeca;
        int numeroDoConteiner = 0;

        if (atual == nullptr) {
            cout << "Nenhum registro disponível para exibição.\n";
            return;
        }

        cout << "Quantidade total de contêineres: " << quantidadeConteineres << "\n\n";

        while (atual != nullptr) {
            cout << "\n========= Contêiner " << numeroDoConteiner << " =========\n";
            cout << "Quantidade de elementos: " << atual->tamanhoAtual << "\n\n";

            // Cabeçalho
            cout << left << setw(8) << "Measure" << " | "
                 << setw(8) << "Quantile" << " | "
                 << setw(20) << "Area" << " | "
                 << setw(8) << "Sex" << " | "
                 << setw(10) << "Age" << " | "
                 << setw(20) << "Geography" << " | "
                 << setw(8) << "Ethnic" << " | "
                 << setw(6) << "Value" << "\n";
            cout << string(110, '-') << "\n";

            // Dados
            for (int i = 0; i < atual->tamanhoAtual; i++) {
                Dados& registro = atual->registros[i];
                cout << left << setw(8) << registro.measure << " | "
                     << setw(8) << registro.quantile << " | "
                     << setw(20) << registro.area << " | "
                     << setw(8) << registro.sex << " | "
                     << setw(10) << registro.age << " | "
                     << setw(20) << registro.geography << " | "
                     << setw(8) << registro.ethnic << " | "
                     << setw(6) << registro.value << "\n";
            }

            numeroDoConteiner++;
            atual = atual->proximo;
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

    // Método para salvar os dados em um arquivo CSV
    void SalvarEmArquivoCSV(string nomeArquivo) {
        ofstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cout << "Erro ao abrir o arquivo para salvar os dados.\n";
            return;
        }

        // Cabeçalho
        arquivo << "Measure,Quantile,Area,Sex,Age,Geography,Ethnic,Value\n";

        Container* atual = cabeca;
        while (atual != nullptr) {
            for (int i = 0; i < atual->tamanhoAtual; i++) {
                Dados& registro = atual->registros[i];
                arquivo << registro.measure << ","
                        << registro.quantile << ","
                        << registro.area << ","
                        << registro.sex << ","
                        << registro.age << ","
                        << registro.geography << ","
                        << registro.ethnic << ","
                        << registro.value << "\n";
            }
            atual = atual->proximo;
        }

        arquivo.close();
        cout << "Dados salvos com sucesso no arquivo " << nomeArquivo << ".\n";
    }

    // Método para inserir dados a partir de um arquivo de texto
    void InserirDadosDeArquivoTexto(string nomeArquivo) {
        ifstream arquivo(nomeArquivo);
        if (!arquivo.is_open()) {
            cout << "Erro ao abrir o arquivo de texto.\n";
            return;
        }

        string linha;
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
        cout << "Dados inseridos a partir do arquivo de texto.\n";
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
        cout << "3 - Inserir dados via arquivo texto\n";
        cout << "4 - Salvar dados em arquivo CSV\n";
        cout << "5 - Sair\n";
        cout << "Opção: ";
        cin >> opcao;

        switch (opcao) {
            case 1: {
                cout << "Digite o nome do arquivo a ser lido: ";
                string arquivo;
                cin >> arquivo;

                if (operacaoDe.LeituraDeArquivo(arquivo)) {
                    cout << "Arquivo lido e registros ordenados com sucesso!\n";
                } else {
                    cout << "Erro ao ler o arquivo!\n";
                }
                break;
            }
            case 2:
                cout << "\nExibindo registros:\n";
                operacaoDe.Exibir();
                break;
            case 3: {
                cout << "Digite o nome do arquivo de texto para inserção: ";
                string arquivoTexto;
                cin >> arquivoTexto;

                operacaoDe.InserirDadosDeArquivoTexto(arquivoTexto);
                break;
            }
            case 4: {
                cout << "Digite o nome do arquivo CSV para salvar os dados: ";
                string nomeArquivo;
                cin >> nomeArquivo;

                operacaoDe.SalvarEmArquivoCSV(nomeArquivo);
                break;
            }
            case 5:
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }
    } while (opcao != 5);

    return 0;
}
