# Seminário de Análise de Algoritmos: Insertion Sort e Quick Sort

Este repositório contém os artefatos do seminário da disciplina de Análise de Algoritmos, com foco no tema Insertion Sort e no comparativo com Quick Sort.

## 👥 Discentes
* Lucas Nobre
* Guilherme Ramos

## 👨‍🏫 Professor
* Prof. Dr. Herbert Oliveira Rocha

## 📅 Informações do Seminário
* **Tema:** 4) Insertion Sort
* **Prazo de Entrega:** 28/04/2026
* **Formato da Apresentação:** A apresentação será feita por meio de slides, com duração máxima de 10 minutos para a equipe.

## 🎯 Objetivos do Trabalho
De acordo com os requisitos da disciplina, as soluções e análises documentadas neste repositório foram descritas com o máximo de detalhes possível. Os objetivos contemplados incluem:

1. **Definição e Análise:** Apresentação da função de custo e complexidade do algoritmo.
2. **Implementação:** Disponibilização do código em linguagem C para o algoritmo proposto.
3. **Experimentação:** Execução do algoritmo com diferentes conjuntos de entradas e coleta dos respectivos tempos de execução.
4. **Análise Assintótica:** Geração de um gráfico de linha relacionando o tempo de execução a cada entrada, incluindo a análise da tendência do comportamento assintótico.
5. **Comparação de Eficiência:** Apresentação e comparação conceitual com um algoritmo que seja mais eficiente, em termos de complexidade, do que o Insertion Sort.

## 📁 Estrutura do Repositório

* `generate_txt.c`: Gera os arquivos `.txt` com entradas para os testes.
* `insertion_sort.c`: Lê um `.txt` e executa apenas o Insertion Sort.
* `quick_sort.c`: Lê um `.txt` e executa apenas o Quick Sort.
* `compare_sorts.c`: Executa os dois algoritmos, compara os tempos e salva o último resultado em `ultimo_compare_sorts.csv`.
* `plot_graficos.py`: Gera gráficos em `matplotlib` lendo os dados da última execução do comparador.
* `entrada_10000.txt`, `entrada_100000.txt`, `entrada_1000000.txt`: Arquivos de entrada gerados na raiz do projeto.

## 🚀 Como Compilar e Executar

```bash
gcc -o generate_txt generate_txt.c
gcc -o insertion_sort insertion_sort.c
gcc -o quick_sort quick_sort.c
gcc -o compare_sorts compare_sorts.c

./generate_txt
./insertion_sort entrada_10000.txt
./quick_sort entrada_10000.txt
./compare_sorts
python plot_graficos.py
```

O script `plot_graficos.py` usa por padrão o arquivo `ultimo_compare_sorts.csv`, gerado na última execução do `compare_sorts`.

## Observação técnica

Em C portável não existe uma instrução universal para limpar a cache da CPU. O arquivo `compare_sorts.c` usa um buffer grande para reduzir o efeito de dados quentes entre uma execução e outra, o que é uma prática comum em comparações acadêmicas simples.
