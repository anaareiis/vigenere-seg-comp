# Módulo: Estimativa de Tamanho da Chave (Cifra de Vigenère)

Este módulo é responsável por estimar o tamanho da palavra-chave utilizada na criptografia do texto. Ele não descobre as letras da chave, mas fornece uma lista de tamanhos prováveis para que o próximo passo do algoritmo possa realizar a análise de frequência.

## Funcionamento

A estimativa é feita utilizando o Método do Índice de Coincidência (IoC).

O Índice de Coincidência mede a probabilidade de duas letras escolhidas ao acaso em um texto serem iguais.

O algoritmo realiza os seguintes passos:

- Testa todos os tamanhos de chave possíveis, de 1 até um valor máximo pré-definido (MAX_TAMANHO_CHAVE).
- Para cada tamanho testado, divide o texto em colunas (grupos de letras que teriam sido cifradas pela mesma letra da chave).
- Calcula o IoC de cada grupo utilizando a fórmula matemática de combinações:

$$
IC = \frac{\sum_{i=A}^{Z} f_i(f_i - 1)}{N(N - 1)}
$$

- Tira a média dos IoCs válidos de todos os grupos.

O tamanho de chave que resultar em uma média mais próxima do IoC padrão do idioma (~0.073) é o candidato mais forte.

## Decisões de Projeto e Implementação

Para garantir que o código seja robusto e eficiente em C++, as seguintes decisões foram tomadas na implementação do arquivo estimativa_tamanho_chave.cpp:

- Limpeza Automática de Texto: Antes de qualquer cálculo, o algoritmo remove todos os caracteres não-alfabéticos (espaços, pontuações, números) e converte todas as letras para maiúsculas utilizando funções seguras da biblioteca <cctype>.
- Tratamento de Exceções Silenciosas (Divisão por Zero): Caso o texto fornecido seja muito curto e resulte em grupos de tamanho 1 ou menor, o cálculo daquele grupo é ignorado. Isso evita divisões por zero, que corromperiam o IoC com valores nulos.
- Contagem de Grupos Válidos: A média final do IoC é dividida apenas pelo número de grupos que efetivamente contribuíram para o cálculo, garantindo a precisão estatística da média.
- Ordenação Inteligente e Tolerância: A função retorna um vetor ordenado (vector<int>) do tamanho mais provável para o menos provável. Na etapa de ordenação, aplica-se uma margem de tolerância estatística: se dois tamanhos de chave apresentam IoCs muito semelhantes, o algoritmo prioriza o menor tamanho. Isso mitiga "falsos positivos" gerados por pequenos textos e múltiplos da chave real.

## Utilização

Para utilizar este módulo em outras partes do projeto, basta incluir o arquivo de cabeçalho correspondente:

```cpp
#include "estimativa_tamanho_chave.h"
```

## Análise de frequência e recuperação da chave

O arquivo `analise_frequencia.cpp` implementa a etapa seguinte do ataque.
Recebido um tamanho de chave, o criptograma é normalizado e separado em uma
coluna para cada posição da chave. Cada coluna equivale a uma cifra de César.

Para cada coluna, os 26 deslocamentos são comparados com as frequências de
letras de português ou inglês usando a estatística de qui-quadrado. O menor
valor indica a melhor aderência ao idioma. A função `analisa_grupo` devolve
todos os deslocamentos em ordem de probabilidade e `recupera_chave` combina o
melhor deslocamento de cada coluna.

Exemplo:

```cpp
#include "analise_frequencia.h"

std::string chave = recupera_chave(
    criptograma,
    tamanho_estimado,
    Idioma::Portugues
);
```

Para compilar e executar os testes com GCC:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic \
  parte2_criptoanalise/analise_frequencia.cpp \
  parte2_criptoanalise/testes/teste_analise_frequencia.cpp \
  -o teste_analise_frequencia
./teste_analise_frequencia
```

### Limitações

A análise estatística precisa de grupos suficientemente longos. Textos curtos,
um tamanho de chave incorreto ou um idioma diferente do selecionado podem gerar
letras erradas. Por isso, o ranking completo retornado por `analisa_grupo`
também pode ser usado pela integração para testar candidatos alternativos.
