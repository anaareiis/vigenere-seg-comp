#include <vigenere/vigenere_cipher.hpp>

#include <exception>
#include <iostream>
#include <string>

namespace {

int run_interactive() {
    std::string key;
    std::string message;

    std::cout << "Chave: ";
    if(!std::getline(std::cin, key)) {
        std::cerr << "Erro: nao foi possivel ler a chave.\n";
        return 1;
    }

    std::cout << "Mensagem: ";
    if(!std::getline(std::cin, message)) {
        std::cerr << "Erro: nao foi possivel ler a mensagem.\n";
        return 1;
    }

    const vigenere::VigenereCipher cipher{key};
    std::cout << "Criptograma: " << cipher.encrypt(message) << '\n';
    return 0;
}

int run_from_arguments(const char* key, const char* message) {
    const vigenere::VigenereCipher cipher{key};
    std::cout << "Criptograma: " << cipher.encrypt(message) << '\n';
    return 0;
}

}

int main(const int argument_count, char* arguments[]) {
    try {
        if(argument_count == 1) {
            return run_interactive();
        }
        if(argument_count == 3) {
            return run_from_arguments(arguments[1], arguments[2]);
        }

        std::cerr << "Uso: " << arguments[0] << " CHAVE \"MENSAGEM\"\n";
        return 2;
    } catch(const std::exception& error) {
        std::cerr << "Erro: " << error.what() << '\n';
        return 1;
    }
}
