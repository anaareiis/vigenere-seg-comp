#include "analise_frequencia.h"
#include "estimativa_tamanho_chave.h"
#include "pipeline.h"

#include <vigenere/vigenere_cipher.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

using Relogio = std::chrono::steady_clock;

volatile std::uint64_t checksum_global = 0;

constexpr std::array<double, 26> FREQUENCIAS_PT = {
    0.1463, 0.0104, 0.0388, 0.0499, 0.1257, 0.0102, 0.0130,
    0.0128, 0.0618, 0.0040, 0.0002, 0.0278, 0.0474, 0.0505,
    0.1073, 0.0252, 0.0120, 0.0653, 0.0781, 0.0434, 0.0463,
    0.0167, 0.0001, 0.0021, 0.0001, 0.0047
};

bool letra_ascii(const char caractere) {
    return (caractere >= 'A' && caractere <= 'Z') ||
           (caractere >= 'a' && caractere <= 'z');
}

char maiuscula_ascii(const char caractere) {
    return caractere >= 'a' && caractere <= 'z'
        ? static_cast<char>(caractere - 'a' + 'A')
        : caractere;
}

std::string ler_arquivo(const std::filesystem::path& caminho) {
    std::ifstream arquivo{caminho};
    if(!arquivo) {
        throw std::runtime_error{"nao foi possivel abrir " + caminho.string()};
    }
    return {std::istreambuf_iterator<char>{arquivo}, std::istreambuf_iterator<char>{}};
}

std::string somente_letras(const std::string_view texto) {
    std::string letras;
    letras.reserve(texto.size());
    for(const char caractere : texto) {
        if(letra_ascii(caractere)) {
            letras.push_back(maiuscula_ascii(caractere));
        }
    }
    return letras;
}

std::vector<std::string> extrair_palavras(const std::string_view texto) {
    std::vector<std::string> palavras;
    std::string atual;
    for(const char caractere : texto) {
        if(letra_ascii(caractere)) {
            atual.push_back(maiuscula_ascii(caractere));
        } else if(!atual.empty()) {
            palavras.push_back(atual);
            atual.clear();
        }
    }
    if(!atual.empty()) {
        palavras.push_back(atual);
    }
    return palavras;
}

std::string montar_amostra(
    const std::vector<std::string>& palavras,
    const std::size_t quantidade_letras,
    const std::size_t inicio
) {
    std::string texto;
    texto.reserve(quantidade_letras + quantidade_letras / 5U);
    std::size_t letras_inseridas = 0;
    std::size_t indice = inicio % palavras.size();
    while(letras_inseridas < quantidade_letras) {
        if(!texto.empty()) {
            texto.push_back(' ');
        }
        const auto& palavra = palavras[indice];
        const auto restante = quantidade_letras - letras_inseridas;
        const auto usar = std::min(restante, palavra.size());
        texto.append(palavra.data(), usar);
        letras_inseridas += usar;
        indice = (indice + 1U) % palavras.size();
    }
    return texto;
}

std::string embaralhar_letras(std::string texto, std::mt19937& gerador) {
    std::vector<std::size_t> posicoes;
    for(std::size_t i = 0; i < texto.size(); ++i) {
        if(letra_ascii(texto[i])) {
            posicoes.push_back(i);
        }
    }
    std::string letras;
    letras.reserve(posicoes.size());
    for(const auto posicao : posicoes) {
        letras.push_back(texto[posicao]);
    }
    std::shuffle(letras.begin(), letras.end(), gerador);
    for(std::size_t i = 0; i < posicoes.size(); ++i) {
        texto[posicoes[i]] = letras[i];
    }
    return texto;
}

std::string chave_aleatoria(const int tamanho, std::mt19937& gerador) {
    std::uniform_int_distribution<int> letra{0, 25};
    std::string chave(static_cast<std::size_t>(tamanho), 'A');
    do {
        for(char& caractere : chave) {
            caractere = static_cast<char>('A' + letra(gerador));
        }
    } while(std::adjacent_find(chave.begin(), chave.end(), std::not_equal_to<char>{}) ==
            chave.end());
    return chave;
}

double indice_coincidencia(const std::string_view texto) {
    std::array<std::uint64_t, 26> contagens{};
    std::uint64_t total = 0;
    for(const char caractere : texto) {
        if(letra_ascii(caractere)) {
            ++contagens[static_cast<std::size_t>(maiuscula_ascii(caractere) - 'A')];
            ++total;
        }
    }
    if(total < 2) {
        return 0.0;
    }
    std::uint64_t numerador = 0;
    for(const auto contagem : contagens) {
        numerador += contagem * (contagem - 1U);
    }
    return static_cast<double>(numerador) /
           static_cast<double>(total * (total - 1U));
}

double ioc_medio_por_periodo(const std::string& texto, const int periodo) {
    const std::string limpo = somente_letras(texto);
    double soma = 0.0;
    int validos = 0;
    for(int coluna = 0; coluna < periodo; ++coluna) {
        std::string grupo;
        for(std::size_t i = static_cast<std::size_t>(coluna); i < limpo.size();
            i += static_cast<std::size_t>(periodo)) {
            grupo.push_back(limpo[i]);
        }
        if(grupo.size() > 1U) {
            soma += indice_coincidencia(grupo);
            ++validos;
        }
    }
    return validos == 0 ? 0.0 : soma / static_cast<double>(validos);
}

template<typename Funcao>
double mediana_segundos(Funcao&& funcao, const int repeticoes) {
    std::vector<double> tempos;
    tempos.reserve(static_cast<std::size_t>(repeticoes));
    for(int repeticao = 0; repeticao < repeticoes; ++repeticao) {
        const auto inicio = Relogio::now();
        funcao();
        const auto fim = Relogio::now();
        tempos.push_back(std::chrono::duration<double>{fim - inicio}.count());
    }
    std::sort(tempos.begin(), tempos.end());
    return tempos[tempos.size() / 2U];
}

std::string entrada_sintetica(const std::size_t bytes, const std::size_t palavra) {
    std::string texto;
    texto.reserve(bytes);
    std::size_t coluna = 0;
    while(texto.size() < bytes) {
        if(palavra != 0U && coluna == palavra) {
            texto.push_back(' ');
            coluna = 0;
        } else {
            texto.push_back(static_cast<char>('A' + (coluna % 26U)));
            ++coluna;
        }
    }
    texto.resize(bytes);
    return texto;
}

void gerar_frequencias_e_ioc(
    const std::string& corpus,
    const std::filesystem::path& saida
) {
    const std::string limpo = somente_letras(corpus);
    const vigenere::VigenereCipher cifra{"SEGURANCA"};
    const std::string criptograma = cifra.encrypt(limpo);

    std::array<std::size_t, 26> claro{};
    std::array<std::size_t, 26> cifrado{};
    for(const char letra : limpo) {
        ++claro[static_cast<std::size_t>(letra - 'A')];
    }
    for(const char letra : criptograma) {
        ++cifrado[static_cast<std::size_t>(letra - 'A')];
    }

    std::ofstream frequencias{saida / "frequencias.csv"};
    frequencias << "letra,esperada_pt,claro,cifrado\n" << std::fixed
                << std::setprecision(8);
    for(std::size_t i = 0; i < 26U; ++i) {
        frequencias << static_cast<char>('A' + i) << ',' << FREQUENCIAS_PT[i] << ','
                    << static_cast<double>(claro[i]) / static_cast<double>(limpo.size())
                    << ',' << static_cast<double>(cifrado[i]) /
                        static_cast<double>(criptograma.size()) << '\n';
    }

    std::ofstream ioc{saida / "ioc_periodos.csv"};
    ioc << "periodo,ioc_medio\n" << std::fixed << std::setprecision(8);
    for(int periodo = 1; periodo <= 20; ++periodo) {
        ioc << periodo << ',' << ioc_medio_por_periodo(criptograma, periodo) << '\n';
    }
}

void gerar_interceptacao(
    const std::vector<std::string>& palavras,
    const std::filesystem::path& saida
) {
    constexpr std::array<int, 4> tamanhos_chave = {3, 6, 9, 12};
    constexpr std::array<int, 10> letras_por_posicao = {
        4, 8, 12, 16, 24, 32, 48, 64, 96, 128
    };
    constexpr int tentativas = 60;
    const std::array<std::string, 3> modos = {
        "ordem_original", "palavras_embaralhadas", "letras_embaralhadas"
    };

    std::mt19937 gerador{0xC1F4A2026U};
    std::uniform_int_distribution<std::size_t> inicio{0, palavras.size() - 1U};
    std::ofstream arquivo{saida / "interceptacao.csv"};
    arquivo << "modo,tamanho_chave,letras_por_posicao,letras_texto,tentativas,"
               "sucesso_periodo_conhecido,sucesso_pipeline\n";

    for(const auto& modo : modos) {
        for(const int tamanho_chave : tamanhos_chave) {
            for(const int razao : letras_por_posicao) {
                int acertos_conhecido = 0;
                int acertos_pipeline = 0;
                const std::size_t letras = static_cast<std::size_t>(tamanho_chave * razao);
                for(int tentativa = 0; tentativa < tentativas; ++tentativa) {
                    auto palavras_da_vez = palavras;
                    if(modo == "palavras_embaralhadas") {
                        std::shuffle(palavras_da_vez.begin(), palavras_da_vez.end(), gerador);
                    }
                    std::string texto = montar_amostra(
                        palavras_da_vez,
                        letras,
                        inicio(gerador)
                    );
                    if(modo == "letras_embaralhadas") {
                        texto = embaralhar_letras(std::move(texto), gerador);
                    }

                    const std::string chave = chave_aleatoria(tamanho_chave, gerador);
                    const vigenere::VigenereCipher cifra{chave};
                    const std::string criptograma = cifra.encrypt(texto);
                    if(recupera_chave(criptograma, tamanho_chave, Idioma::Portugues) == chave) {
                        ++acertos_conhecido;
                    }
                    const auto resultados = executa_ataque(
                        criptograma,
                        Idioma::Portugues,
                        tamanho_chave * 2,
                        1
                    );
                    if(!resultados.empty() &&
                       resultados.front().tamanho_chave == tamanho_chave &&
                       resultados.front().chave == chave) {
                        ++acertos_pipeline;
                    }
                }
                arquivo << modo << ',' << tamanho_chave << ',' << razao << ',' << letras
                        << ',' << tentativas << ','
                        << static_cast<double>(acertos_conhecido) / tentativas << ','
                        << static_cast<double>(acertos_pipeline) / tentativas << '\n';
            }
        }
    }
}

void gerar_desempenho_cifra(const std::filesystem::path& saida) {
    const vigenere::VigenereCipher cifra{"DESEMPENHO"};
    constexpr std::array<std::size_t, 9> tamanhos = {
        1024U, 4096U, 16384U, 65536U, 262144U,
        1048576U, 4194304U, 16777216U, 67108864U
    };
    std::ofstream arquivo{saida / "desempenho_tamanho.csv"};
    arquivo << "bytes,iteracoes,retorno_mib_s,in_place_mib_s\n" << std::fixed
            << std::setprecision(3);
    for(const auto bytes : tamanhos) {
        const std::size_t iteracoes = std::max<std::size_t>(1U, (64U * 1024U * 1024U) / bytes);
        const std::string entrada = entrada_sintetica(bytes, 7U);
        const double retorno = mediana_segundos([&] {
            std::uint64_t checksum = 0;
            for(std::size_t i = 0; i < iteracoes; ++i) {
                const auto resultado = cifra.encrypt(entrada);
                checksum += static_cast<unsigned char>(resultado[i % resultado.size()]);
            }
            checksum_global = checksum_global + checksum;
        }, 5);
        const double in_place = mediana_segundos([&] {
            std::string resultado = entrada;
            std::uint64_t checksum = 0;
            for(std::size_t i = 0; i < iteracoes; ++i) {
                cifra.encrypt_in_place(resultado);
                checksum += static_cast<unsigned char>(resultado[i % resultado.size()]);
            }
            checksum_global = checksum_global + checksum;
        }, 5);
        const double mib = static_cast<double>(bytes * iteracoes) / (1024.0 * 1024.0);
        arquivo << bytes << ',' << iteracoes << ',' << mib / retorno << ','
                << mib / in_place << '\n';
    }
}

void gerar_desempenho_palavras(const std::filesystem::path& saida) {
    const vigenere::VigenereCipher cifra{"DESEMPENHO"};
    constexpr std::array<std::size_t, 6> comprimentos = {1U, 3U, 5U, 10U, 20U, 0U};
    constexpr std::size_t bytes = 16U * 1024U * 1024U;
    constexpr int iteracoes = 6;
    std::ofstream arquivo{saida / "desempenho_palavras.csv"};
    arquivo << "comprimento_palavra,fracao_letras,vazao_mib_s\n" << std::fixed
            << std::setprecision(4);
    for(const auto comprimento : comprimentos) {
        const std::string entrada = entrada_sintetica(bytes, comprimento);
        const auto letras = static_cast<double>(std::count_if(
            entrada.begin(), entrada.end(), letra_ascii
        ));
        const double segundos = mediana_segundos([&] {
            std::string resultado = entrada;
            for(int i = 0; i < iteracoes; ++i) {
                cifra.encrypt_in_place(resultado);
            }
            checksum_global = checksum_global +
                static_cast<unsigned char>(resultado[resultado.size() / 2U]);
        }, 5);
        arquivo << (comprimento == 0U ? 1000U : comprimento) << ','
                << letras / static_cast<double>(bytes) << ','
                << (static_cast<double>(bytes * iteracoes) / (1024.0 * 1024.0)) /
                    segundos << '\n';
    }
}

void gerar_desempenho_threads(const std::filesystem::path& saida) {
    constexpr std::array<int, 5> quantidades = {1, 2, 4, 8, 16};
    constexpr std::size_t bytes_por_thread = 8U * 1024U * 1024U;
    constexpr int iteracoes = 6;
    std::ofstream arquivo{saida / "desempenho_threads.csv"};
    arquivo << "threads,bytes_por_thread,iteracoes,vazao_agregada_mib_s,speedup\n"
            << std::fixed << std::setprecision(3);
    double base = 0.0;
    for(const int quantidade : quantidades) {
        std::vector<double> tempos;
        tempos.reserve(5U);
        for(int repeticao = 0; repeticao < 5; ++repeticao) {
            std::atomic<int> prontas{0};
            std::atomic<bool> iniciar{false};
            std::vector<std::uint64_t> checksums(static_cast<std::size_t>(quantidade));
            std::vector<std::string> entradas;
            entradas.reserve(static_cast<std::size_t>(quantidade));
            for(int indice = 0; indice < quantidade; ++indice) {
                entradas.push_back(entrada_sintetica(bytes_por_thread, 7U));
            }
            std::vector<std::thread> threads;
            threads.reserve(static_cast<std::size_t>(quantidade));
            for(int indice = 0; indice < quantidade; ++indice) {
                threads.emplace_back([&, indice] {
                    vigenere::VigenereCipher cifra{"DESEMPENHO"};
                    auto& entrada = entradas[static_cast<std::size_t>(indice)];
                    prontas.fetch_add(1, std::memory_order_release);
                    while(!iniciar.load(std::memory_order_acquire)) {
                        std::this_thread::yield();
                    }
                    for(int i = 0; i < iteracoes; ++i) {
                        cifra.encrypt_in_place(entrada);
                    }
                    checksums[static_cast<std::size_t>(indice)] =
                        static_cast<unsigned char>(entrada[static_cast<std::size_t>(indice)]);
                });
            }
            while(prontas.load(std::memory_order_acquire) != quantidade) {
                std::this_thread::yield();
            }
            const auto inicio = Relogio::now();
            iniciar.store(true, std::memory_order_release);
            for(auto& thread : threads) {
                thread.join();
            }
            const auto fim = Relogio::now();
            tempos.push_back(std::chrono::duration<double>{fim - inicio}.count());
            checksum_global = checksum_global + std::accumulate(
                checksums.begin(), checksums.end(), std::uint64_t{0}
            );
        }
        std::sort(tempos.begin(), tempos.end());
        const double segundos = tempos[tempos.size() / 2U];
        const double total_mib = static_cast<double>(
            bytes_por_thread * static_cast<std::size_t>(iteracoes) *
            static_cast<std::size_t>(quantidade)
        ) / (1024.0 * 1024.0);
        const double vazao = total_mib / segundos;
        if(quantidade == 1) {
            base = vazao;
        }
        arquivo << quantidade << ',' << bytes_por_thread << ',' << iteracoes << ','
                << vazao << ',' << vazao / base << '\n';
    }
}

void gerar_desempenho_ataque(
    const std::vector<std::string>& palavras,
    const std::filesystem::path& saida
) {
    constexpr std::array<std::size_t, 8> tamanhos = {
        100U, 250U, 500U, 1000U, 2000U, 4000U, 8000U, 16000U
    };
    std::ofstream arquivo{saida / "desempenho_ataque.csv"};
    arquivo << "letras,iteracoes,estimador_ms,pipeline_ms\n" << std::fixed
            << std::setprecision(5);
    for(const auto letras : tamanhos) {
        const std::string texto = montar_amostra(palavras, letras, 0U);
        const std::string criptograma = vigenere::VigenereCipher{"SEGURANCA"}.encrypt(texto);
        const int iteracoes = letras <= 1000U ? 100 : (letras <= 4000U ? 40 : 15);
        const double estimador = mediana_segundos([&] {
            for(int i = 0; i < iteracoes; ++i) {
                const auto resultado = estima_tamanho_chave(criptograma, 20);
                checksum_global = checksum_global + static_cast<std::uint64_t>(resultado.front());
            }
        }, 5);
        const double pipeline = mediana_segundos([&] {
            for(int i = 0; i < iteracoes; ++i) {
                const auto resultado = executa_ataque(
                    criptograma, Idioma::Portugues, 20, 5
                );
                checksum_global = checksum_global +
                    static_cast<std::uint64_t>(resultado.front().tamanho_chave);
            }
        }, 5);
        arquivo << letras << ',' << iteracoes << ','
                << estimador * 1000.0 / iteracoes << ','
                << pipeline * 1000.0 / iteracoes << '\n';
    }
}

} // namespace

int main(const int argc, char* argv[]) {
    try {
        if(argc != 3) {
            std::cerr << "Uso: " << argv[0] << " CORPUS DIRETORIO_SAIDA\n";
            return 2;
        }
        const std::string corpus = ler_arquivo(argv[1]);
        const auto palavras = extrair_palavras(corpus);
        if(palavras.empty()) {
            throw std::runtime_error{"o corpus nao contem palavras ASCII"};
        }
        const std::filesystem::path saida{argv[2]};
        std::filesystem::create_directories(saida);

        gerar_frequencias_e_ioc(corpus, saida);
        gerar_interceptacao(palavras, saida);
        gerar_desempenho_cifra(saida);
        gerar_desempenho_palavras(saida);
        gerar_desempenho_threads(saida);
        gerar_desempenho_ataque(palavras, saida);

        std::cout << "Experimentos concluidos. Checksum: " << checksum_global << '\n';
        return 0;
    } catch(const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
