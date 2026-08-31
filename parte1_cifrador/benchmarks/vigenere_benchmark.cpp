#include <vigenere/vigenere_cipher.hpp>

#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {

std::size_t parse_positive(const char* input, const std::string_view name) {
    std::size_t value = 0;
    const std::string_view text{input};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if(error != std::errc{} || end != text.data() + text.size() || value == 0) {
        throw std::invalid_argument{std::string{name} + " deve ser um inteiro positivo"};
    }
    return value;
}

std::string make_input(const std::size_t size) {
    constexpr std::string_view pattern{
        "Seguranca Computacional 2026: Vigenere, teste de desempenho. "
    };
    std::string input;
    input.reserve(size);
    while(input.size() < size) {
        const auto remaining = size - input.size();
        input.append(pattern.substr(0, remaining < pattern.size() ? remaining : pattern.size()));
    }
    return input;
}

double mib_per_second(
    const std::size_t byte_count,
    const std::size_t iterations,
    const std::chrono::steady_clock::duration elapsed
) {
    const auto seconds = std::chrono::duration<double>{elapsed}.count();
    const auto mebibytes = static_cast<double>(byte_count) *
                           static_cast<double>(iterations) / (1024.0 * 1024.0);
    return mebibytes / seconds;
}

}

int main(const int argument_count, char* arguments[]) {
    try {
        const std::size_t size_mib = argument_count > 1
            ? parse_positive(arguments[1], "tamanho")
            : 16U;
        const std::size_t iterations = argument_count > 2
            ? parse_positive(arguments[2], "iteracoes")
            : 10U;
        if(argument_count > 3) {
            std::cerr << "Uso: " << arguments[0] << " [TAMANHO_MiB] [ITERACOES]\n";
            return 2;
        }
        if(size_mib > std::numeric_limits<std::size_t>::max() / (1024U * 1024U)) {
            throw std::invalid_argument{"tamanho excede o limite da plataforma"};
        }

        const std::size_t byte_count = size_mib * 1024U * 1024U;
        const std::string input = make_input(byte_count);
        const vigenere::VigenereCipher cipher{"DESEMPENHO"};
        std::uint64_t checksum = 0;

        const auto allocation_start = std::chrono::steady_clock::now();
        for(std::size_t iteration = 0; iteration < iterations; ++iteration) {
            const std::string output = cipher.encrypt(input);
            checksum += static_cast<unsigned char>(output[iteration % output.size()]);
        }
        const auto allocation_end = std::chrono::steady_clock::now();

        std::string output = input;
        const auto in_place_start = std::chrono::steady_clock::now();
        for(std::size_t iteration = 0; iteration < iterations; ++iteration) {
            cipher.encrypt_in_place(output);
            checksum += static_cast<unsigned char>(output[iteration % output.size()]);
        }
        const auto in_place_end = std::chrono::steady_clock::now();

        std::cout << std::fixed << std::setprecision(2)
                  << "Entrada: " << size_mib << " MiB\n"
                  << "Iteracoes: " << iterations << '\n'
                  << "API com retorno: "
                  << mib_per_second(byte_count, iterations, allocation_end - allocation_start)
                  << " MiB/s\n"
                  << "Nucleo in-place: "
                  << mib_per_second(byte_count, iterations, in_place_end - in_place_start)
                  << " MiB/s\n"
                  << "Checksum: " << checksum << '\n';
        return 0;
    } catch(const std::exception& error) {
        std::cerr << "Erro: " << error.what() << '\n';
        return 1;
    }
}
