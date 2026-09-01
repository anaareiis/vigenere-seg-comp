#include "analise_frequencia.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace {

// Frequencias relativas de A a Z. A soma de cada tabela e aproximadamente 1.
constexpr std::array<double, 26> FREQUENCIAS_PT = {
    0.1463, 0.0104, 0.0388, 0.0499, 0.1257, 0.0102, 0.0130,
    0.0128, 0.0618, 0.0040, 0.0002, 0.0278, 0.0474, 0.0505,
    0.1073, 0.0252, 0.0120, 0.0653, 0.0781, 0.0434, 0.0463,
    0.0167, 0.0001, 0.0021, 0.0001, 0.0047
};

constexpr std::array<double, 26> FREQUENCIAS_EN = {
    0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
    0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,
    0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056, 0.02758,
    0.00978, 0.02360, 0.00150, 0.01974, 0.00074
};

bool letra_ascii(char caractere) {
    const unsigned char c = static_cast<unsigned char>(caractere);
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

char maiuscula_ascii(char caractere) {
    if (caractere >= 'a' && caractere <= 'z') {
        return static_cast<char>(caractere - 'a' + 'A');
    }
    return caractere;
}

const std::array<double, 26>& frequencias_do_idioma(Idioma idioma) {
    return idioma == Idioma::Portugues ? FREQUENCIAS_PT : FREQUENCIAS_EN;
}

std::string limpa_texto(const std::string& texto) {
    std::string limpo;
    limpo.reserve(texto.size());
    for (char caractere : texto) {
        if (letra_ascii(caractere)) {
            limpo.push_back(maiuscula_ascii(caractere));
        }
    }
    return limpo;
}

} // namespace

std::vector<CandidatoDeslocamento> analisa_grupo(
    const std::string& grupo,
    Idioma idioma
) {
    const std::string texto = limpa_texto(grupo);
    if (texto.empty()) {
        throw std::invalid_argument("o grupo deve conter ao menos uma letra ASCII");
    }

    std::array<int, 26> observadas{};
    for (char letra : texto) {
        ++observadas[static_cast<std::size_t>(letra - 'A')];
    }

    const auto& esperadas_idioma = frequencias_do_idioma(idioma);
    std::vector<CandidatoDeslocamento> candidatos;
    candidatos.reserve(26);

    for (int deslocamento = 0; deslocamento < 26; ++deslocamento) {
        double qui_quadrado = 0.0;

        // Se C = P + deslocamento, a frequencia esperada na letra cifrada C
        // e a frequencia do idioma em P = C - deslocamento.
        for (int cifrada = 0; cifrada < 26; ++cifrada) {
            const int original = (cifrada - deslocamento + 26) % 26;
            const double esperado =
                static_cast<double>(texto.size()) * esperadas_idioma[original];
            const double diferenca = observadas[cifrada] - esperado;
            qui_quadrado += diferenca * diferenca / esperado;
        }

        candidatos.push_back({
            static_cast<char>('A' + deslocamento),
            qui_quadrado
        });
    }

    std::sort(candidatos.begin(), candidatos.end(),
        [](const CandidatoDeslocamento& a, const CandidatoDeslocamento& b) {
            if (std::abs(a.pontuacao - b.pontuacao) < 1e-12) {
                return a.letra < b.letra;
            }
            return a.pontuacao < b.pontuacao;
        });

    return candidatos;
}

std::string recupera_chave(
    const std::string& criptograma,
    int tamanho_chave,
    Idioma idioma
) {
    if (tamanho_chave <= 0) {
        throw std::invalid_argument("o tamanho da chave deve ser positivo");
    }

    const std::string texto = limpa_texto(criptograma);
    if (texto.empty()) {
        throw std::invalid_argument("o criptograma deve conter letras ASCII");
    }
    if (static_cast<std::size_t>(tamanho_chave) > texto.size()) {
        throw std::invalid_argument("o tamanho da chave nao pode exceder o texto");
    }

    std::vector<std::string> grupos(static_cast<std::size_t>(tamanho_chave));
    for (std::size_t i = 0; i < texto.size(); ++i) {
        grupos[i % grupos.size()].push_back(texto[i]);
    }

    std::string chave;
    chave.reserve(grupos.size());
    for (const std::string& grupo : grupos) {
        chave.push_back(analisa_grupo(grupo, idioma).front().letra);
    }
    return chave;
}
