#pragma once

#include "analise_frequencia.h"

#include <string>
#include <vector>

struct ResultadoAtaque {
    int tamanho_chave;
    std::string chave;
    std::string texto_claro;
};

// Executa as etapas do ataque para os tamanhos mais provaveis. Como a
// avaliacao estatistica pode produzir falsos positivos, os resultados sao
// devolvidos em ranking para validacao de legibilidade.
std::vector<ResultadoAtaque> executa_ataque(
    const std::string& criptograma,
    Idioma idioma,
    int tamanho_maximo_chave,
    int quantidade_candidatos = 5
);
