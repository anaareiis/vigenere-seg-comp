#pragma once

#include <string>
#include <vector>

enum class Idioma {
    Portugues,
    Ingles
};

struct CandidatoDeslocamento {
    char letra;
    double pontuacao;
};

// Ordena os 26 deslocamentos do mais para o menos provavel.
// Uma pontuacao menor (qui-quadrado) indica melhor aderencia ao idioma.
std::vector<CandidatoDeslocamento> analisa_grupo(
    const std::string& grupo,
    Idioma idioma
);

// Recupera uma letra para cada posicao da chave. O criptograma pode conter
// espacos e pontuacao; apenas letras ASCII consomem posicoes da chave.
std::string recupera_chave(
    const std::string& criptograma,
    int tamanho_chave,
    Idioma idioma
);
