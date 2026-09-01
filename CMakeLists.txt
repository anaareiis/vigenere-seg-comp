cmake_minimum_required(VERSION 3.16)

project(vigenere_criptoanalise VERSION 1.0.0 LANGUAGES CXX)

set(VIGENERE_BUILD_CLI ON CACHE BOOL "" FORCE)
set(VIGENERE_BUILD_TESTS ON CACHE BOOL "" FORCE)
set(VIGENERE_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
add_subdirectory(parte1_cifrador)

add_library(
    vigenere_criptoanalise
    parte2_criptoanalise/estimativa_tamanho_chave.cpp
    parte2_criptoanalise/analise_frequencia.cpp
    parte2_criptoanalise/pipeline.cpp
)
target_compile_features(vigenere_criptoanalise PUBLIC cxx_std_17)
target_include_directories(vigenere_criptoanalise PUBLIC parte2_criptoanalise)
target_link_libraries(vigenere_criptoanalise PUBLIC vigenere::vigenere)

add_executable(
    vigenere_ataque
    parte2_criptoanalise/app/main.cpp
)
target_link_libraries(vigenere_ataque PRIVATE vigenere_criptoanalise)

include(CTest)
if(BUILD_TESTING)
    add_executable(
        teste_estimativa_chave
        parte2_criptoanalise/testes/teste_estimativa_chave.cpp
    )
    target_link_libraries(teste_estimativa_chave PRIVATE vigenere_criptoanalise)
    add_test(NAME estimativa_tamanho_chave COMMAND teste_estimativa_chave)

    add_executable(
        teste_analise_frequencia
        parte2_criptoanalise/testes/teste_analise_frequencia.cpp
    )
    target_link_libraries(teste_analise_frequencia PRIVATE vigenere_criptoanalise)
    add_test(NAME analise_frequencia COMMAND teste_analise_frequencia)

    add_executable(
        teste_pipeline
        parte2_criptoanalise/testes/teste_pipeline.cpp
    )
    target_link_libraries(teste_pipeline PRIVATE vigenere_criptoanalise)
    add_test(NAME vigenere_pipeline COMMAND teste_pipeline)
endif()

if(MSVC)
    target_compile_options(vigenere_criptoanalise PRIVATE /W4 /permissive-)
    target_compile_options(vigenere_ataque PRIVATE /W4 /permissive-)
else()
    target_compile_options(vigenere_criptoanalise PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(vigenere_ataque PRIVATE -Wall -Wextra -Wpedantic)
endif()
