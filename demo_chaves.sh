#!/bin/bash
# Demo: tamanho da chave importa na cifra de Vigenere
cd "$(dirname "$0")"
ARQ=${1:-chaves_pequenas.txt}
MSG=${2:-"textos para testes/texto_claro_teste_portugues.txt"}
# MSG pode ser caminho de arquivo OU texto direto entre aspas
if [ -f "$MSG" ]; then TXT=$(cat "$MSG"); else TXT="$MSG"; fi
for K in $(cat "$ARQ"); do
  echo "########## CHAVE: $K (tamanho ${#K}) ##########"
  CT=$(./parte1_cifrador/build/vigenere_cifrador "$K" "$TXT" | sed 's/^Criptograma: //')
  echo "CIFRADO (trecho): ${CT:0:120}..."
  echo "--- ATAQUE (max 55) ---"
  echo "$CT" | ./build/vigenere_ataque pt 55 /dev/stdin 2>&1 | grep -E "Tamanho da chave|^Chave:" | head -4
  echo
done
