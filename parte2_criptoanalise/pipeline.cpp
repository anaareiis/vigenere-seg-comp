#include "pipeline.h"

#include "estimativa_tamanho_chave.h"

#include <vigenere/vigenere_cipher.hpp>

#include <algorithm>
#include <stdexcept>

std::vector<ResultadoAtaque> executa_ataque(
    const std::string& criptograma,
    const Idioma idioma,
    const int tamanho_maximo_chave,
    const int quantidade_candidatos
) {
    if(tamanho_maximo_chave <= 0) {
        throw std::invalid_argument("o tamanho maximo da chave deve ser positivo");
    }
    if(quantidade_candidatos <= 0) {
        throw std::invalid_argument("a quantidade de candidatos deve ser positiva");
    }

    const std::vector<int> tamanhos = estima_tamanho_chave(
        criptograma,
        tamanho_maximo_chave
    );
    const auto limite = std::min(
        tamanhos.size(),
        static_cast<std::size_t>(quantidade_candidatos)
    );

    std::vector<ResultadoAtaque> resultados;
    resultados.reserve(limite);
    for(std::size_t indice = 0; indice < limite; ++indice) {
        const int tamanho = tamanhos[indice];
        const std::string chave = recupera_chave(criptograma, tamanho, idioma);
        const vigenere::VigenereCipher cifra{chave};
        resultados.push_back({tamanho, chave, cifra.decrypt(criptograma)});
    }
    return resultados;
}
