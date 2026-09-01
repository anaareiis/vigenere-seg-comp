# Textos e resultados de teste

Esta pasta reúne os arquivos usados na demonstração do pipeline de
criptoanálise. Como os criptogramas previstos no roteiro não foram
fornecidos, a professora autorizou o grupo a produzir os próprios dados de
teste.

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

## Demonstração do efeito do tamanho da chave

Arquivos:

- `demo_chaves.sh`: cifra `texto_claro_teste_portugues.txt` (ou outra
  mensagem informada) com cada chave de um arquivo de lista e roda o ataque
  de criptoanálise em cima do resultado, mostrando se o pipeline conseguiu
  recuperar a chave de volta;
- `chaves_pequenas.txt`: uma chave curta (`BOA`, 3 letras);
- `chaves_grandes.txt`: uma chave longa (33 letras), para ilustrar o caso em
  que a chave é maior que o texto e a criptoanálise estatística deixa de
  funcionar;
- `tipo_chave.txt`: exemplos de invocação do script.

Requer o build unificado (`build/parte1_cifrador/vigenere_cifrador` e
`build/vigenere_ataque`, ver seção "Pipeline de recuperação da chave" no
README da raiz). Executando a partir da raiz do repositório:

```bash
# chave curta, mensagem padrão
"textos para testes/demo_chaves.sh"

# chave longa, mensagem própria
"textos para testes/demo_chaves.sh" "textos para testes/chaves_grandes.txt" "MENSAGEM CURTA"
```

Com `chaves_pequenas.txt` (chave de 3 letras), o pipeline recupera a chave
normalmente. Com `chaves_grandes.txt` (chave de 33 letras) sobre uma
mensagem curta, cada posição da chave recebe poucas ou nenhuma letra
repetida, e a análise de frequência não tem amostra suficiente para
convergir — o ataque falha ou erra a chave, ilustrando por que uma chave do
tamanho da mensagem (o limite do One-Time Pad) inviabiliza esse método
estatístico.
