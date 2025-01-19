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
    
    // Método para inserir dados via entrada padrão de forma ordenada
	void InserirDadoViaEntradaPadrao() {
		// Criação de um novo registro
		Dados novoRegistro;

		// Leitura dos dados via entrada padrão
		cout << "Digite os dados do novo registro:\n";
		cout << "Measure: ";
		cin >> novoRegistro.measure;
		cout << "Quantile: ";
		cin >> novoRegistro.quantile;
		cout << "Area: ";
		cin >> novoRegistro.area;
		cout << "Sex: ";
		cin >> novoRegistro.sex;
		cout << "Age: ";
		cin >> novoRegistro.age;
		cout << "Geography: ";
		cin >> novoRegistro.geography;
		cout << "Ethnic: ";
		cin >> novoRegistro.ethnic;
		cout << "Value: ";
		cin >> novoRegistro.value;

		// Verificar se o contêiner atual está cheio
		if (cauda->tamanhoAtual == capacidade) {
			AdicionarNovoContainer();
		}

		// Adicionar o novo registro ao contêiner
		cauda->registros[cauda->tamanhoAtual] = novoRegistro;
		cauda->tamanhoAtual++;

		// Realizar a ordenação após a inserção
		QuickSort();
		cout << "Dado inserido com sucesso e registros ordenados!\n";
	}
	
	// Função para remover um dado específico baseado no valor
	void RemoverDadoEspecifico(int valor) {
		Container* atual = cabeca;
		bool encontrado = false;

		while (atual != nullptr) {
			// Procura o dado no contêiner atual
			for (int i = 0; i < atual->tamanhoAtual; i++) {
				if (atual->registros[i].value == valor) {
					// Dado encontrado, remove-o
					for (int j = i; j < atual->tamanhoAtual - 1; j++) {
						atual->registros[j] = atual->registros[j + 1];
					}
					atual->tamanhoAtual--; // Reduz o tamanho do contêiner
					encontrado = true;
					break;
				}
			}

			// Se encontrado, não precisa procurar mais
			if (encontrado) {
				break;
			}

			atual = atual->proximo;  // Avança para o próximo contêiner
		}

		if (encontrado) {
			cout << "Dado com valor " << valor << " removido com sucesso.\n";
		} else {
			cout << "Dado com valor " << valor << " não encontrado.\n";
		}
	}
	
	// Função para buscar um dado específico baseado no valor
	void BuscarDadoEspecifico(int valor) {
		Container* atual = cabeca;
		bool encontrado = false;

		while (atual != nullptr) {
			// Procura o dado no contêiner atual
			for (int i = 0; i < atual->tamanhoAtual; i++) {
				if (atual->registros[i].value == valor) {
					// Dado encontrado, exibe as informações do registro
					cout << "Dado encontrado: \n";
					cout << "Measure: " << atual->registros[i].measure << "\n";
					cout << "Quantile: " << atual->registros[i].quantile << "\n";
					cout << "Area: " << atual->registros[i].area << "\n";
					cout << "Sex: " << atual->registros[i].sex << "\n";
					cout << "Age: " << atual->registros[i].age << "\n";
					cout << "Geography: " << atual->registros[i].geography << "\n";
					cout << "Ethnic: " << atual->registros[i].ethnic << "\n";
					cout << "Value: " << atual->registros[i].value << "\n";
					encontrado = true;
					break;
				}
			}

			// Se encontrado, não precisa procurar mais
			if (encontrado) {
				break;
			}

			atual = atual->proximo;  // Avança para o próximo contêiner
		}

		if (!encontrado) {
			cout << "Dado com valor " << valor << " não encontrado.\n";
		}
	}
};

/* 
Função principal para interação com o usuário.
*/
#include <iostream>
#include <string>
using namespace std;

int main() {
    SequenceSet operacaoDe; // Criacao do SequenceSet
    int opcao;

    cout << "******************************************************\n";
    cout << "*****    BEM-VINDO A CRIACAO DO SEQUENCESET    *****\n";
    cout << "******************************************************\n";
    cout << "\n";

    do {
        cout << "***********************************************\n";
        cout << "             MENU DE OPCOES                    \n";
        cout << "***********************************************\n";
        cout << " 1 - Leitura de arquivo                        \n";
        cout << " 2 - Exibir registros                          \n";
        cout << " 3 - Inserir dados via arquivo texto           \n";
        cout << " 4 - Salvar dados em arquivo CSV               \n";
        cout << " 5 - Inserir dado via entrada padrao           \n";
        cout << " 6 - Remover dado especifico                   \n";
        cout << " 7 - Buscar dado especifico                    \n";
        cout << " 8 - Sair                                      \n";
        cout << "***********************************************\n";
        cout << " Selecione uma opcao (1-8): ";
        cin >> opcao;

        cout << "\n";

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
                cout << "Digite o nome do arquivo de texto para insercao: ";
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
            case 5: {
                operacaoDe.InserirDadoViaEntradaPadrao();  // Chama o método de inserção
                break;
            }
            case 6: {
                cout << "Digite o valor do dado a ser removido: ";
                int valor;
                cin >> valor;

                operacaoDe.RemoverDadoEspecifico(valor);  // Chama o método de remoção
                break;
            }
            case 7: {
                cout << "Digite o valor do dado a ser buscado: ";
                int valor;
                cin >> valor;

                operacaoDe.BuscarDadoEspecifico(valor);  // Chama o método de busca
                break;
            }
            case 8:
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opcao invalida! Tente novamente.\n";
        }

        cout << "\n";
    } while (opcao != 8);

    return 0;
}

