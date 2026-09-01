#include <vigenere/vigenere_cipher.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class TestFailure final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

void expect_equal(
    const std::string_view actual,
    const std::string_view expected,
    const std::string_view context
) {
    if(actual != expected) {
        throw TestFailure{
            std::string{context} + ": esperado [" + std::string{expected} +
            "], obtido [" + std::string{actual} + "]"
        };
    }
}

void expect_equal(
    const std::size_t actual,
    const std::size_t expected,
    const std::string_view context
) {
    if(actual != expected) {
        throw TestFailure{
            std::string{context} + ": valores numericos diferentes"
        };
    }
}

template<typename Callable>
void expect_invalid_argument(Callable&& callable, const std::string_view context) {
    try {
        std::forward<Callable>(callable)();
    } catch(const std::invalid_argument&) {
        return;
    }
    throw TestFailure{std::string{context} + ": invalid_argument nao foi lancada"};
}

std::string reference_encrypt(const std::string_view plaintext, const std::string_view key) {
    std::string ciphertext{plaintext};
    std::size_t key_index = 0;

    for(char& raw_character : ciphertext) {
        const auto character = static_cast<unsigned char>(raw_character);
        const bool upper = character >= static_cast<unsigned char>('A') &&
                           character <= static_cast<unsigned char>('Z');
        const bool lower = character >= static_cast<unsigned char>('a') &&
                           character <= static_cast<unsigned char>('z');
        if(!upper && !lower) {
            continue;
        }

        const auto base = upper ? static_cast<unsigned char>('A')
                                : static_cast<unsigned char>('a');
        const auto key_character = static_cast<unsigned char>(key[key_index]);
        const auto key_base = key_character >= static_cast<unsigned char>('a')
            ? static_cast<unsigned char>('a')
            : static_cast<unsigned char>('A');
        const auto encrypted = static_cast<unsigned int>(character - base) +
                               static_cast<unsigned int>(key_character - key_base);
        raw_character = static_cast<char>(base + encrypted % 26U);
        key_index = (key_index + 1U) % key.size();
    }

    return ciphertext;
}

void test_classic_vector() {
    const vigenere::VigenereCipher cipher{"LEMON"};
    expect_equal(cipher.encrypt("ATTACKATDAWN"), "LXFOPVEFRNHR", "vetor classico");
    expect_equal(cipher.decrypt("LXFOPVEFRNHR"), "ATTACKATDAWN", "decifracao classica");
}

void test_case_preservation() {
    const vigenere::VigenereCipher cipher{"leMoN"};
    expect_equal(cipher.encrypt("AttackAtDawn"), "LxfopvEfRnhr", "preservacao de caixa");
}

void test_non_letters_do_not_advance_key() {
    const vigenere::VigenereCipher cipher{"BC"};
    expect_equal(cipher.encrypt("A-BC 12!"), "B-DD 12!", "avanco da chave");
}

void test_utf8_is_preserved() {
    const vigenere::VigenereCipher cipher{"CHAVE"};
    expect_equal(cipher.encrypt("Segurança 2026!"), "Ulgpvcuça 2026!", "texto UTF-8");
}

void test_empty_message() {
    const vigenere::VigenereCipher cipher{"KEY"};
    expect_equal(cipher.encrypt(""), "", "mensagem vazia");
}

void test_in_place_api() {
    const vigenere::VigenereCipher cipher{"B"};
    std::string message(1'000'000, 'Z');
    cipher.encrypt_in_place(message);
    expect_equal(message, std::string(1'000'000, 'A'), "API in-place");
    cipher.decrypt_in_place(message);
    expect_equal(message, std::string(1'000'000, 'Z'), "decifracao in-place");
}

void test_key_length() {
    const vigenere::VigenereCipher cipher{"SegComp"};
    expect_equal(cipher.key_length(), 7U, "tamanho da chave");
}

void test_generated_cases() {
    std::uint32_t state = 0xC1F0201U;
    const auto next_value = [&state] {
        state ^= state << 13U;
        state ^= state >> 17U;
        state ^= state << 5U;
        return state;
    };
    constexpr std::string_view alphabet{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 0123456789-_,.!?"
    };

    for(std::size_t case_index = 0; case_index < 500U; ++case_index) {
        const std::size_t key_size = 1U + next_value() % 31U;
        const std::size_t message_size = next_value() % 2048U;
        std::string key(key_size, 'A');
        std::string message(message_size, ' ');

        for(char& character : key) {
            const auto letter = static_cast<char>('A' + next_value() % 26U);
            character = next_value() % 2U == 0U
                ? letter
                : static_cast<char>(letter - 'A' + 'a');
        }
        for(char& character : message) {
            character = alphabet[next_value() % alphabet.size()];
        }

        const vigenere::VigenereCipher cipher{key};
        expect_equal(
            cipher.encrypt(message),
            reference_encrypt(message, key),
            "caso deterministico " + std::to_string(case_index)
        );
        expect_equal(
            cipher.decrypt(cipher.encrypt(message)),
            message,
            "ida e volta " + std::to_string(case_index)
        );
    }
}

void test_invalid_keys() {
    expect_invalid_argument(
        [] { const vigenere::VigenereCipher cipher{""}; },
        "chave vazia"
    );
    expect_invalid_argument(
        [] { const vigenere::VigenereCipher cipher{"ABC1"}; },
        "chave com numero"
    );
    expect_invalid_argument(
        [] { const vigenere::VigenereCipher cipher{"SEG COMP"}; },
        "chave com espaco"
    );
    expect_invalid_argument(
        [] { const vigenere::VigenereCipher cipher{"CHAVÉ"}; },
        "chave com acento"
    );
}

}

int main() {
    const std::vector<std::pair<std::string_view, std::function<void()>>> tests{
        {"vetor classico", test_classic_vector},
        {"preservacao de caixa", test_case_preservation},
        {"caracteres nao alfabeticos", test_non_letters_do_not_advance_key},
        {"preservacao de UTF-8", test_utf8_is_preserved},
        {"mensagem vazia", test_empty_message},
        {"API in-place", test_in_place_api},
        {"tamanho da chave", test_key_length},
        {"500 casos determinísticos", test_generated_cases},
        {"validacao de chaves", test_invalid_keys},
    };

    std::size_t passed = 0;
    for(const auto& [name, test] : tests) {
        try {
            test();
            ++passed;
            std::cout << "[OK] " << name << '\n';
        } catch(const std::exception& error) {
            std::cerr << "[FALHOU] " << name << ": " << error.what() << '\n';
        }
    }

    std::cout << passed << '/' << tests.size() << " testes passaram.\n";
    return passed == tests.size() ? EXIT_SUCCESS : EXIT_FAILURE;
}
