cmake_minimum_required(VERSION 3.16)

project(vigenere_cifrador VERSION 1.0.0 LANGUAGES CXX)

set(VIGENERE_BUILD_TOOLS_BY_DEFAULT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(VIGENERE_BUILD_TOOLS_BY_DEFAULT ON)
endif()

option(
    VIGENERE_BUILD_CLI
    "Compilar o programa de linha de comando"
    ${VIGENERE_BUILD_TOOLS_BY_DEFAULT}
)
option(
    VIGENERE_BUILD_TESTS
    "Compilar os testes do cifrador"
    ${VIGENERE_BUILD_TOOLS_BY_DEFAULT}
)
option(
    VIGENERE_BUILD_BENCHMARKS
    "Compilar o benchmark do cifrador"
    ${VIGENERE_BUILD_TOOLS_BY_DEFAULT}
)

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(
            ${target}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wsign-conversion
        )
    endif()
endfunction()

add_library(vigenere src/vigenere_cipher.cpp)
add_library(vigenere::vigenere ALIAS vigenere)

target_compile_features(vigenere PUBLIC cxx_std_17)
target_include_directories(
    vigenere
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
enable_project_warnings(vigenere)

if(VIGENERE_BUILD_CLI)
    add_executable(vigenere_cifrador app/main.cpp)
    target_link_libraries(vigenere_cifrador PRIVATE vigenere::vigenere)
    enable_project_warnings(vigenere_cifrador)
endif()

if(VIGENERE_BUILD_TESTS)
    include(CTest)
    add_executable(vigenere_testes tests/vigenere_tests.cpp)
    target_link_libraries(vigenere_testes PRIVATE vigenere::vigenere)
    enable_project_warnings(vigenere_testes)
    add_test(NAME vigenere_testes COMMAND vigenere_testes)
    if(VIGENERE_BUILD_CLI)
        add_test(
            NAME vigenere_cli_vetor_classico
            COMMAND vigenere_cifrador LEMON "Attack at dawn!"
        )
        set_tests_properties(
            vigenere_cli_vetor_classico
            PROPERTIES PASS_REGULAR_EXPRESSION "Criptograma: Lxfopv ef rnhr!"
        )
        add_test(
            NAME vigenere_cli_chave_invalida
            COMMAND vigenere_cifrador "CHAVE 1" "Mensagem"
        )
        set_tests_properties(
            vigenere_cli_chave_invalida
            PROPERTIES WILL_FAIL TRUE
        )
    endif()
endif()

if(VIGENERE_BUILD_BENCHMARKS)
    add_executable(vigenere_benchmark benchmarks/vigenere_benchmark.cpp)
    target_link_libraries(vigenere_benchmark PRIVATE vigenere::vigenere)
    enable_project_warnings(vigenere_benchmark)
endif()

include(GNUInstallDirs)
install(TARGETS vigenere EXPORT VigenereTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
    EXPORT VigenereTargets
    NAMESPACE vigenere::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Vigenere
)
