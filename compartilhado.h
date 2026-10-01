#pragma once
#define NOMINMAX
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

// Um produtor e um consumidor por vez, na mesma sessao do Windows.
constexpr wchar_t NOME_MEMORIA[] = L"Local\\AtividadeSO_Collatz_Memoria_v1";
constexpr wchar_t NOME_PRONTO[] = L"Local\\AtividadeSO_Collatz_Pronto_v1";
constexpr wchar_t NOME_LIDO[] = L"Local\\AtividadeSO_Collatz_Lido_v1";
constexpr std::uint64_t CAPACIDADE = 4096;
constexpr DWORD TEMPO_LIMITE_MS = 120000;

// Somente dados, nunca ponteiros: cada processo tem enderecos proprios.
struct DadosCompartilhados {
    std::uint64_t quantidade;
    std::uint64_t valores[CAPACIDADE];
};
static_assert(sizeof(DadosCompartilhados) == 32776, "Layout inesperado");

inline void erroWindows(const char* operacao) {
    const DWORD codigo = GetLastError();
    throw std::runtime_error(std::string(operacao) + ": erro Windows " + std::to_string(codigo));
}

// RAII: libera os recursos inclusive quando ocorre uma excecao.
struct Handle {
    HANDLE valor;
    explicit Handle(HANDLE h) : valor(h) {}
    ~Handle() { if (valor) CloseHandle(valor); }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
};
struct Vista {
    void* endereco;
    explicit Vista(void* p) : endereco(p) {}
    ~Vista() { if (endereco) UnmapViewOfFile(endereco); }
    Vista(const Vista&) = delete;
    Vista& operator=(const Vista&) = delete;
};

inline void aguardar(HANDLE evento, const char* descricao) {
    const DWORD resultado = WaitForSingleObject(evento, TEMPO_LIMITE_MS);
    if (resultado == WAIT_FAILED) erroWindows("WaitForSingleObject");
    if (resultado == WAIT_TIMEOUT)
        throw std::runtime_error(std::string("Tempo limite de 120 segundos: ") + descricao);
    if (resultado != WAIT_OBJECT_0) throw std::runtime_error("Espera retornou estado inesperado");
}
