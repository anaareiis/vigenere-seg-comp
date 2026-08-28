# Parte I — Cifrador de Vigenère

Implementação própria da cifração de Vigenère em C++17, organizada como biblioteca reutilizável, programa de linha de comando, testes e benchmark. Este módulo é responsável exclusivamente pela cifração e está preparado para integração com os demais componentes do trabalho.

## Responsável pelo cifrador

- Gabriel de Sousa
- Matrícula: 211056000

## Política do alfabeto

- A chave aceita exclusivamente letras ASCII de `A` a `Z`, sem diferença entre maiúsculas e minúsculas.
- Cada letra ASCII da mensagem é cifrada e conserva sua caixa.
- Espaços, números, pontuação, acentos e demais bytes UTF-8 são preservados e não consomem posições da chave.
- Chaves vazias ou com qualquer caractere fora de `A–Z` são rejeitadas.
- A mensagem pode ser vazia.

Para uma letra `Pᵢ` e o deslocamento correspondente `Kᵢ`, ambos no intervalo de 0 a 25, a implementação calcula:

```text
Cᵢ = (Pᵢ + Kᵢ) mod 26
```

A chave é convertida uma única vez em deslocamentos numéricos. Durante a cifração ela é percorrida circularmente sem construir uma chave expandida. O resultado é criado com uma única cópia da mensagem, e a API `encrypt_in_place` permite eliminar essa alocação quando o chamador pode alterar o próprio buffer.

## Estrutura

```text
parte1_cifrador/
├── app/             programa de linha de comando
├── benchmarks/      medição de vazão
├── include/         interface pública da biblioteca
├── resultados/      saídas obtidas nesta implementação
├── src/             implementação da cifra
├── tests/           testes automatizados
├── CMakeLists.txt
└── README.md
```

## Compilação

Execute a partir desta pasta:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Execução

Com argumentos:

```bash
./build/vigenere_cifrador LEMON "Attack at dawn!"
```

Ou no modo interativo:

```bash
./build/vigenere_cifrador
```

## Uso como biblioteca

Em outro `CMakeLists.txt`, considerando este repositório em `extern/vigenere-seg-comp`:

```cmake
add_subdirectory(extern/vigenere-seg-comp/parte1_cifrador)
target_link_libraries(seu_programa PRIVATE vigenere::vigenere)
```

No código consumidor:

```cpp
#include <vigenere/vigenere_cipher.hpp>

vigenere::VigenereCipher cipher{"LEMON"};
const std::string ciphertext = cipher.encrypt("Attack at dawn!");
```

## Testes

```bash
ctest --test-dir build --output-on-failure
./build/vigenere_testes
```

Os testes cobrem o vetor clássico conhecido, caixa, caracteres não alfabéticos, UTF-8, mensagem vazia, API sem alocação, mensagem grande e validação de chaves.

## Desempenho

O benchmark recebe opcionalmente o tamanho da entrada em MiB e o número de iterações:

```bash
./build/vigenere_benchmark 16 10
```

Ele mede separadamente a API que retorna uma nova string e a API in-place. O checksum apresentado serve para tornar o resultado observável e impedir a eliminação do trabalho pelo otimizador. Vazão varia conforme processador, compilador e carga da máquina.

## Referência de engenharia

A estrutura do laço foi comparada com a implementação pública do projeto [TheAlgorithms/C-Plus-Plus](https://github.com/TheAlgorithms/C-Plus-Plus/blob/master/ciphers/vigenere_cipher.cpp), que também percorre a chave circularmente em vez de expandi-la. O código deste trabalho foi escrito do zero e não usa bibliotecas de cifra, conforme a restrição do enunciado.
