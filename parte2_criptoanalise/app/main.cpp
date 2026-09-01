#include "pipeline.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

Idioma parse_idioma(const std::string& valor) {
    if(valor == "pt") {
        return Idioma::Portugues;
    }
    if(valor == "en") {
        return Idioma::Ingles;
    }
    throw std::invalid_argument("idioma deve ser pt ou en");
}

std::string le_arquivo(const std::string& caminho) {
    std::ifstream arquivo{caminho};
    if(!arquivo) {
        throw std::runtime_error("nao foi possivel abrir o arquivo: " + caminho);
    }
    std::ostringstream conteudo;
    conteudo << arquivo.rdbuf();
    return conteudo.str();
}

int inteiro_positivo(const char* valor, const std::string& nome) {
    std::size_t caracteres_lidos = 0;
    const int numero = std::stoi(valor, &caracteres_lidos);
    if(valor[caracteres_lidos] != '\0' || numero <= 0) {
        throw std::invalid_argument(nome + " deve ser um inteiro positivo");
    }
    return numero;
}

} // namespace

int main(const int argc, char* argv[]) {
    if(argc < 4 || argc > 5) {
        std::cerr << "Uso: " << argv[0]
                  << " <pt|en> <tamanho-maximo> <arquivo> [candidatos]\n";
        return 2;
    }

    try {
        const Idioma idioma = parse_idioma(argv[1]);
        const int tamanho_maximo = inteiro_positivo(argv[2], "tamanho maximo");
        const int quantidade = argc == 5
            ? inteiro_positivo(argv[4], "quantidade de candidatos")
            : 5;
        const std::string criptograma = le_arquivo(argv[3]);
        const auto resultados = executa_ataque(
            criptograma,
            idioma,
            tamanho_maximo,
            quantidade
        );

        for(std::size_t i = 0; i < resultados.size(); ++i) {
            const auto& resultado = resultados[i];
            std::cout << "=== Candidato " << i + 1 << " ===\n"
                      << "Tamanho da chave: " << resultado.tamanho_chave << '\n'
                      << "Chave: " << resultado.chave << '\n'
                      << "Texto claro:\n" << resultado.texto_claro << "\n\n";
        }
        return 0;
    } catch(const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
