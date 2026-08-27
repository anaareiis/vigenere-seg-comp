# Cifra de Vigenère — Implementação e Criptoanálise

Trabalho de Implementação 1 da disciplina CIC0201 – Segurança Computacional (Profa. Priscila Solis).

## Grupo

- Integrante 1
- Integrante 2
- Integrante 3
- Integrante 4

## Estrutura do projeto

```
vigenere-seg-comp/
├── parte1_cifrador/        # Cifração e decifração (Vigenère implementado do zero)
├── parte2_criptoanalise/   # Estimativa de tamanho de chave, análise de frequência, quebra da cifra
├── textos/                 # Criptogramas fornecidos e textos de referência (frequência PT/EN)
├── relatorio/              # Relatório técnico (máx. 4 páginas)
└── README.md
```

## Parte I — Cifrador e Decifrador

Implementação própria (sem bibliotecas prontas de Vigenère) de:

- Cifração: chave + texto claro → criptograma
- Decifração: chave + criptograma → texto claro

Como executar:

```bash
python3 parte1_cifrador/vigenere.py
```

## Parte II — Ataque de Recuperação da Chave

Dado um criptograma, o ataque:

1. Estima o tamanho provável da chave (índice de coincidência / método de Kasiski).
2. Separa o criptograma em subconjuntos por posição da chave.
3. Faz análise de frequência de letras em cada subconjunto.
4. Compara com as distribuições esperadas de português/inglês.
5. Reconstrói a chave candidata e decifra o criptograma.

Como executar:

```bash
python3 parte2_criptoanalise/quebra_vigenere.py
```

## Requisitos

- Python 3.10+
- Sem dependências externas além da biblioteca padrão (ver `requirements.txt`)

## Entregáveis

- [ ] Código-fonte (Parte I e Parte II)
- [ ] Relatório técnico (`relatorio/`)
- [ ] Link deste repositório
