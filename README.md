# Cifra de Vigenère — Implementação e Criptoanálise

Trabalho de Implementação 1 da disciplina CIC0201 – Segurança Computacional (Profa. Priscila Solis).

## Grupo

- Gabriel de Sousa — matrícula 211056000
- Integrante 2
- Integrante 3
- Integrante 4

## Estrutura do projeto

```text
vigenere-seg-comp/
├── parte1_cifrador/        # Cifrador C++ reutilizável, CLI, testes e benchmark
├── parte2_criptoanalise/   # Estimativa de tamanho de chave, análise de frequência, quebra da cifra
├── textos/                 # Criptogramas fornecidos e textos de referência (frequência PT/EN)
├── relatorio/              # Relatório técnico (máx. 4 páginas)
└── README.md
```

## Parte I — Cifrador e Decifrador

Implementação própria em C++17 (sem bibliotecas prontas de Vigenère) de:

- Cifração: chave + texto claro → criptograma
- Decifração: será implementada em uma etapa posterior

Como compilar e executar:

```bash
cmake -S parte1_cifrador -B parte1_cifrador/build -DCMAKE_BUILD_TYPE=Release
cmake --build parte1_cifrador/build --parallel
./parte1_cifrador/build/vigenere_cifrador LEMON "Attack at dawn!"
```

Instruções da biblioteca, testes, benchmark e política do alfabeto estão em [`parte1_cifrador/README.md`](parte1_cifrador/README.md).

## Parte II — Ataque de Recuperação da Chave

Dado um criptograma, o ataque:

1. Estima o tamanho provável da chave (índice de coincidência / método de Kasiski).
2. Separa o criptograma em subconjuntos por posição da chave.
3. Faz análise de frequência de letras em cada subconjunto.
4. Compara com as distribuições esperadas para português/inglês.
5. Reconstrói a chave candidata e decifra o criptograma.

Como executar:

```bash
python3 parte2_criptoanalise/quebra_vigenere.py
```

## Requisitos

- CMake 3.16+ e compilador compatível com C++17 para a Parte I
- Python 3.10+ para a Parte II
- Sem bibliotecas externas de cifra

## Entregáveis

- [ ] Código-fonte (cifração da Parte I concluída; decifração e Parte II pendentes)
- [ ] Relatório técnico (`relatorio/`)
- [ ] Link deste repositório
