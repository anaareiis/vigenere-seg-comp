# Textos e resultados de teste

Esta pasta reúne os arquivos usados na demonstração do pipeline de
criptoanálise. Eles foram produzidos pelo grupo e não correspondem aos dois
criptogramas oficiais mencionados no roteiro.

## Teste em português

Arquivos:

- `texto_claro_teste_portugues.txt`: mensagem original conhecida;
- `criptograma_teste_portugues.txt`: mensagem cifrada usada como entrada do
  ataque.

Parâmetros usados para gerar o criptograma:

- idioma: português;
- chave: `SEGURANCA`;
- tamanho da chave: 9;
- política do alfabeto: somente letras ASCII consomem posições da chave;
  espaços, pontuação e quebras de linha são preservados.

### Execução

Na raiz do repositório:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/vigenere_ataque pt 20 \
  "textos para testes/criptograma_teste_portugues.txt" 10
```

### Resultado observado

O primeiro candidato devolvido pelo pipeline foi:

```text
Tamanho da chave: 9
Chave: SEGURANCA
```

O texto claro foi recuperado integralmente e coincidiu com
`texto_claro_teste_portugues.txt`, incluindo espaços, pontuação e parágrafos.

O segundo candidato apresentou tamanho 18 e chave
`SEGURANCASEGURANCA`. Ele também recuperou o texto porque é uma repetição da
chave fundamental. Esse resultado ilustra por que tamanhos múltiplos podem
aparecer com Índice de Coincidência elevado e por que o menor período deve ser
priorizado.

### Critérios de aprovação

O teste é considerado aprovado quando:

1. o tamanho 9 aparece como primeiro candidato;
2. a chave recuperada é `SEGURANCA`;
3. o texto claro produzido coincide com o arquivo de referência;
4. caracteres não alfabéticos permanecem inalterados.

## Criptogramas oficiais

Quando os arquivos oficiais forem disponibilizados, eles devem ser salvos na
pasta `textos/`, com nomes distintos, por exemplo:

```text
textos/criptograma_portugues.txt
textos/criptograma_ingles.txt
```

Eles poderão ser processados com:

```bash
./build/vigenere_ataque pt 20 textos/criptograma_portugues.txt 10
./build/vigenere_ataque en 20 textos/criptograma_ingles.txt 10
```

Os tamanhos, chaves, textos recuperados e eventuais refinamentos deverão ser
registrados no relatório final.
