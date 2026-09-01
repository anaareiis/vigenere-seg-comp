#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/*
Para importar em outro projeto CMake, adicione esta pasta com add_subdirectory,
vincule o alvo com target_link_libraries(seu_programa PRIVATE vigenere::vigenere),
inclua este arquivo com #include <vigenere/vigenere_cipher.hpp> e instancie
vigenere::VigenereCipher cifrador{"CHAVE"} antes de chamar cifrador.encrypt(texto).
*/

namespace vigenere {

class VigenereCipher final {
public:
    explicit VigenereCipher(std::string_view key);

    [[nodiscard]] std::string encrypt(std::string_view plaintext) const;
    void encrypt_in_place(std::string& plaintext) const noexcept;
    [[nodiscard]] std::string decrypt(std::string_view ciphertext) const;
    void decrypt_in_place(std::string& ciphertext) const noexcept;

    [[nodiscard]] std::size_t key_length() const noexcept;

private:
    std::vector<std::uint8_t> key_shifts_;
};

}
