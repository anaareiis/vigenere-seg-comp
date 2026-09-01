#include "../analise_frequencia.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

std::string cifra(const std::string& texto, const std::string& chave) {
    std::string resultado = texto;
    std::size_t posicao_chave = 0;
    for (char& caractere : resultado) {
        const bool maiuscula = caractere >= 'A' && caractere <= 'Z';
        const bool minuscula = caractere >= 'a' && caractere <= 'z';
        if (!maiuscula && !minuscula) {
            continue;
        }
        const char base = maiuscula ? 'A' : 'a';
        const int deslocamento = chave[posicao_chave] - 'A';
        caractere = static_cast<char>(base + (caractere - base + deslocamento) % 26);
        posicao_chave = (posicao_chave + 1) % chave.size();
    }
    return resultado;
}

void exige(bool condicao, const std::string& mensagem) {
    if (!condicao) {
        throw std::runtime_error(mensagem);
    }
}

std::string monta_amostra(
    const std::array<int, 26>& contagens,
    int numero_de_grupos
) {
    std::string coluna;
    for (int letra = 0; letra < 26; ++letra) {
        coluna.append(static_cast<std::size_t>(contagens[letra]),
                      static_cast<char>('A' + letra));
    }

    // Intercala colunas com a mesma distribuicao para que cada posicao da
    // chave seja uma amostra estatistica completa e deterministica.
    std::string texto;
    texto.reserve(coluna.size() * static_cast<std::size_t>(numero_de_grupos));
    for (char letra : coluna) {
        texto.append(static_cast<std::size_t>(numero_de_grupos), letra);
    }
    return texto;
}

template<typename Funcao>
void exige_argumento_invalido(Funcao funcao, const std::string& mensagem) {
    try {
        funcao();
    } catch (const std::invalid_argument&) {
        return;
    }
    throw std::runtime_error(mensagem);
}

} // namespace

int main() {
    try {
        const std::array<int, 26> contagens_pt = {
            146, 10, 39, 50, 126, 10, 13, 13, 62, 4, 1, 28, 47,
            51, 107, 25, 12, 65, 78, 43, 46, 17, 1, 2, 1, 5
        };
        const std::array<int, 26> contagens_en = {
            82, 15, 28, 43, 127, 22, 20, 61, 70, 2, 8, 40, 24,
            67, 75, 19, 1, 60, 63, 91, 28, 10, 24, 2, 20, 1
        };
        const std::string portugues = monta_amostra(contagens_pt, 7);
        const std::string ingles = monta_amostra(contagens_en, 5);

        const std::string recuperada_pt =
            recupera_chave(cifra(portugues, "SEGREDO"), 7, Idioma::Portugues);
        exige(
            recuperada_pt == "SEGREDO",
            "nao recuperou a chave do texto em portugues: " + recuperada_pt
        );
        const std::string recuperada_en =
            recupera_chave(cifra(ingles, "LEMON"), 5, Idioma::Ingles);
        exige(
            recuperada_en == "LEMON",
            "nao recuperou a chave do texto em ingles: " + recuperada_en
        );

        const auto ranking = analisa_grupo(cifra(ingles, "D"), Idioma::Ingles);
        exige(ranking.size() == 26, "o ranking deve conter 26 deslocamentos");
        exige(ranking.front().letra == 'D', "o melhor deslocamento deveria ser D");

        exige_argumento_invalido(
            [] { recupera_chave("ABC", 0, Idioma::Portugues); },
            "tamanho zero deveria ser rejeitado"
        );
        exige_argumento_invalido(
            [] { recupera_chave("123 !", 1, Idioma::Portugues); },
            "texto sem letras deveria ser rejeitado"
        );
        exige_argumento_invalido(
            [] { recupera_chave("ABC", 4, Idioma::Portugues); },
            "chave maior que o texto deveria ser rejeitada"
        );

        std::cout << "Todos os testes da analise de frequencia passaram.\n";
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Teste falhou: " << erro.what() << '\n';
        return 1;
    }
}
