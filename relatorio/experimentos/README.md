# Experimentos do relatório

Os gráficos do relatório são gerados a partir da implementação real do
projeto. O programa em C++ produz os CSVs de frequência, Índice de
Coincidência, taxa empírica de recuperação da chave e desempenho. O script em
Python transforma esses dados nas figuras PDF e PNG usadas pelo LaTeX.

## Reproduzir

Na raiz do repositório:

```bash
g++ -std=c++17 -O3 -DNDEBUG -Wall -Wextra -Wpedantic -pthread \
  -Iparte1_cifrador/include -Iparte2_criptoanalise \
  relatorio/experimentos/experimentos_relatorio.cpp \
  parte1_cifrador/src/vigenere_cipher.cpp \
  parte2_criptoanalise/estimativa_tamanho_chave.cpp \
  parte2_criptoanalise/analise_frequencia.cpp \
  parte2_criptoanalise/pipeline.cpp \
  -o /tmp/experimentos_vigenere

/tmp/experimentos_vigenere \
  "textos para testes/texto_claro_teste_portugues.txt" \
  relatorio/experimentos/dados

python3 relatorio/experimentos/gerar_graficos.py

cd relatorio
latexmk -pdf relatorio.tex
```

O gerador usa uma semente fixa para que as 7.200 tentativas de criptoanálise
sejam repetíveis. Os benchmarks usam a mediana de cinco rodadas, mas seus
valores absolutos variam conforme processador, compilador, frequência dinâmica
e carga da máquina.

## Interpretação da taxa de interceptação

Neste experimento, uma tentativa só conta como sucesso quando o primeiro
candidato do pipeline possui o tamanho e a chave exatos. Essa taxa mede o
sucesso do ataque implementado sobre o corpus local; não mede a probabilidade
de capturar tráfego e não deve ser generalizada como uma propriedade universal
da cifra.

O paralelismo mede mensagens independentes processadas simultaneamente. A API
atual continua monothread para uma única mensagem.
