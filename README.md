# Cifra de Vigenère - Implementação e Criptoanálise

Trabalho de Implementação 1 da disciplina CIC0201 - Segurança Computacional,
ministrada pela Profa. Priscila Solis na Universidade de Brasília.

## Equipe e divisão do trabalho

- **Pessoa A - Gabriel de Sousa (211056000):** cifrador/decifrador, política do alfabeto e
  validação das entradas.
- **Pessoa B - Pedro de Paula Campos (231036050):** estimativa do tamanho da chave por Índice de
  Coincidência e separação do criptograma por posição da chave.
- **Pessoa C - Marina Pimentel Moreno (222014071):** análise de frequência em português e inglês,
  recuperação dos caracteres e reconstrução da chave.
- **Pessoa D - Ana Luísa Reis Nascente (211045688):** integração, execução do pipeline no
  criptograma de teste, validação dos resultados e relatório técnico.

A professora esclareceu que o trabalho poderia ser realizado por quatro
integrantes e que, como os criptogramas previstos no roteiro não foram
fornecidos, o grupo poderia produzir os próprios dados de teste.

## O que foi implementado

- Cifração de Vigenère em C++17, com CLI, testes e benchmark.
- Tratamento documentado de maiúsculas, minúsculas, espaços, números e
  caracteres especiais.
- Validação da chave de entrada.
- Estimativa de tamanhos prováveis da chave por Índice de Coincidência.
- Testes da estimativa com chaves de tamanhos 5, 7, 11 e 16.
- Decifração com chave conhecida.
- Análise de frequência para português e inglês.
- Recuperação das letras e reconstrução da chave para tamanho conhecido.
- Decifração automática dos criptogramas com as chaves candidatas.
- Integração e validação do pipeline completo com dados de teste.
- Resultados finais e conclusão do relatório.

## Estrutura do projeto

```text
vigenere-seg-comp/
|-- parte1_cifrador/        # biblioteca, CLI, testes e benchmark
|-- parte2_criptoanalise/   # estimativa da chave e etapas do ataque
|-- textos para testes/     # criptograma e texto claro de demonstracao
|-- relatorio/              # relatorio tecnico em LaTeX
`-- README.md
```

## Parte I - Cifrador

A implementação atual cifra textos com uma chave alfabética. Letras maiúsculas
e minúsculas preservam a caixa; caracteres fora de `A-Z` e `a-z` são copiados
sem alteração e não consomem uma posição da chave. Chaves vazias ou contendo
caracteres não alfabéticos ASCII são rejeitadas.

Para compilar e testar:

```bash
cmake -S parte1_cifrador -B parte1_cifrador/build -DCMAKE_BUILD_TYPE=Release
cmake --build parte1_cifrador/build --parallel
ctest --test-dir parte1_cifrador/build --output-on-failure
```

Exemplo de cifração:

```bash
./parte1_cifrador/build/vigenere_cifrador LEMON "Attack at dawn!"
```

Detalhes da API, testes e benchmark estão em
[`parte1_cifrador/README.md`](parte1_cifrador/README.md).

Esse comando cria uma build independente, restrita à Parte I, em
`parte1_cifrador/build/`. A build integrada usada no pipeline, descrita a
seguir, é criada separadamente em `build/`; os binários de uma não aparecem
no diretório da outra.

## Parte II - Estimativa do tamanho da chave

O módulo desenvolvido por Pedro testa os tamanhos de `1` até um limite
informado. Para cada tamanho candidato, ele:

1. remove caracteres não alfabéticos e converte as letras para maiúsculas;
2. separa o criptograma em grupos associados a cada posição da chave;
3. calcula o Índice de Coincidência de cada grupo válido;
4. calcula o IC médio dos grupos;
5. ordena os tamanhos por probabilidade, priorizando tamanhos menores quando
   os valores de IC ficam dentro da tolerância estatística definida.

O módulo devolve um vetor de tamanhos candidatos; ele ainda não recupera as
letras da chave. Para compilar e executar seus testes:

```bash
g++ -std=c++17 \
  parte2_criptoanalise/estimativa_tamanho_chave.cpp \
  parte2_criptoanalise/testes/teste_estimativa_chave.cpp \
  -o teste_estimativa_chave
./teste_estimativa_chave
```

Os quatro casos de teste existentes identificam corretamente chaves de
tamanhos 5, 7, 11 e 16. A explicação detalhada do método está em
[`parte2_criptoanalise/README.md`](parte2_criptoanalise/README.md).

## Pipeline de recuperação da chave

O executável integrado realiza as seguintes etapas:

1. Estimar tamanhos prováveis com o Índice de Coincidência.
2. Dividir o criptograma segundo cada tamanho candidato.
3. Comparar as frequências de cada grupo com distribuições de português ou
   inglês.
4. Obter candidatos para os deslocamentos, reconstruir a chave e decifrar.
5. Exibir os melhores candidatos para avaliação de legibilidade e eventual
   refinamento.

Para compilar todo o projeto e executar os testes:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Para atacar um criptograma salvo em arquivo:

```bash
./build/vigenere_ataque pt 20 "textos para testes/criptograma_teste_portugues.txt"
```

Os argumentos são o idioma (`pt` ou `en`), o tamanho máximo de chave e o
caminho do arquivo. Um quarto argumento opcional define quantos candidatos
serão exibidos; o padrão é cinco.

## Relatório

O relatório segue o formato IEEE de conferência e o limite específico de
quatro páginas definido no roteiro do trabalho. Para compilar:

```bash
cd relatorio
latexmk -pdf relatorio.tex
```

## Requisitos

- compilador compatível com C++17;
- CMake 3.16 ou superior para a Parte I;
- distribuição LaTeX com `IEEEtran` e `latexmk` para o relatório;
- nenhuma biblioteca que implemente diretamente Vigenère ou sua criptoanálise.
