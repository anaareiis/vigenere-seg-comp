#include <vigenere/vigenere_cipher.hpp>

#include <stdexcept>

namespace {

constexpr bool is_upper_ascii(const unsigned char value) noexcept {
    return value >= static_cast<unsigned char>('A') &&
           value <= static_cast<unsigned char>('Z');
}

constexpr bool is_lower_ascii(const unsigned char value) noexcept {
    return value >= static_cast<unsigned char>('a') &&
           value <= static_cast<unsigned char>('z');
}

// Normaliza letras maiúsculas e minúsculas como deslocamentos de 0 a 25.
constexpr std::uint8_t alphabet_index(const unsigned char value) noexcept {
    const auto base = is_upper_ascii(value) ? static_cast<unsigned char>('A')
                                            : static_cast<unsigned char>('a');
    return static_cast<std::uint8_t>(value - base);
}

// Implementa C_i = (P_i + K_i) mod 26 diretamente sobre o alfabeto ASCII.
constexpr char add_modulo_alphabet(
    const unsigned char value,
    const std::uint8_t shift
) noexcept {
    const auto base = is_upper_ascii(value) ? static_cast<unsigned char>('A')
                                            : static_cast<unsigned char>('a');
    auto encrypted = static_cast<unsigned int>(value - base) + shift;
    if(encrypted >= 26U) {
        encrypted -= 26U;
    }
    return static_cast<char>(base + encrypted);
}

// Implementa P_i = (C_i - K_i + 26) mod 26 sobre o alfabeto ASCII.
constexpr char subtract_modulo_alphabet(
    const unsigned char value,
    const std::uint8_t shift
) noexcept {
    const auto base = is_upper_ascii(value) ? static_cast<unsigned char>('A')
                                            : static_cast<unsigned char>('a');
    const auto encrypted = static_cast<unsigned int>(value - base);
    const auto decrypted = (encrypted + 26U - shift) % 26U;
    return static_cast<char>(base + decrypted);
}

}

namespace vigenere {

VigenereCipher::VigenereCipher(const std::string_view key) {
    if(key.empty()) {
        throw std::invalid_argument("a chave nao pode ser vazia");
    }

    // Converte a chave uma única vez e evita reconstruí-la para cada mensagem.
    key_shifts_.reserve(key.size());
    for(const char raw_character : key) {
        const auto character = static_cast<unsigned char>(raw_character);
        if(!is_upper_ascii(character) && !is_lower_ascii(character)) {
            throw std::invalid_argument("a chave deve conter somente letras ASCII de A a Z");
        }
        key_shifts_.push_back(alphabet_index(character));
    }
}

std::string VigenereCipher::encrypt(const std::string_view plaintext) const {
    std::string ciphertext{plaintext};
    encrypt_in_place(ciphertext);
    return ciphertext;
}

void VigenereCipher::encrypt_in_place(std::string& plaintext) const noexcept {
    std::size_t key_index = 0;

    for(char& raw_character : plaintext) {
        const auto character = static_cast<unsigned char>(raw_character);
        // Caracteres fora de A-Z e a-z são preservados e não avançam a chave.
        if(!is_upper_ascii(character) && !is_lower_ascii(character)) {
            continue;
        }

        raw_character = add_modulo_alphabet(character, key_shifts_[key_index]);
        ++key_index;
        if(key_index == key_shifts_.size()) {
            // Reinicia a chave circularmente sem criar uma versão expandida.
            key_index = 0;
        }
    }
}

std::string VigenereCipher::decrypt(const std::string_view ciphertext) const {
    std::string plaintext{ciphertext};
    decrypt_in_place(plaintext);
    return plaintext;
}

void VigenereCipher::decrypt_in_place(std::string& ciphertext) const noexcept {
    std::size_t key_index = 0;

    for(char& raw_character : ciphertext) {
        const auto character = static_cast<unsigned char>(raw_character);
        if(!is_upper_ascii(character) && !is_lower_ascii(character)) {
            continue;
        }

        raw_character = subtract_modulo_alphabet(character, key_shifts_[key_index]);
        ++key_index;
        if(key_index == key_shifts_.size()) {
            key_index = 0;
        }
    }
}

std::size_t VigenereCipher::key_length() const noexcept {
    return key_shifts_.size();
}

}
