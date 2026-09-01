#include "pipeline.h"

#include <vigenere/vigenere_cipher.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

std::string texto_com_frequencias_portugues(const int tamanho_chave) {
    const int contagens[26] = {
        146, 10, 39, 50, 126, 10, 13, 13, 62, 4, 1, 28, 47,
        51, 107, 25, 12, 65, 78, 43, 46, 17, 1, 2, 1, 5
    };
    std::string texto;
    for(int letra = 0; letra < 26; ++letra) {
        texto.append(
            static_cast<std::size_t>(contagens[letra] * tamanho_chave),
            static_cast<char>('A' + letra)
        );
    }
    return texto;
}

} // namespace

int main() {
    const std::string chave = "SOL";
    const std::string texto = texto_com_frequencias_portugues(
        static_cast<int>(chave.size())
    );
    const vigenere::VigenereCipher cifra{chave};
    const std::string criptograma = cifra.encrypt(texto);
    const auto resultados = executa_ataque(
        criptograma,
        Idioma::Portugues,
        8,
        5
    );

    for(const auto& resultado : resultados) {
        if(resultado.tamanho_chave == static_cast<int>(chave.size()) &&
           resultado.chave == chave && resultado.texto_claro == texto) {
            std::cout << "Pipeline recuperou tamanho, chave e texto claro.\n";
            return EXIT_SUCCESS;
        }
    }

    std::cerr << "Pipeline nao recuperou o candidato esperado.\n";
    return EXIT_FAILURE;
}
