#include "compartilhado.h"
#include <limits>
#include <vector>

std::uint64_t lerEntrada(int argc, char* argv[]) {
    if (argc != 2) throw std::runtime_error("Uso: produtor.exe <inteiro positivo>");
    const std::string texto = argv[1];
    if (texto.empty()) throw std::runtime_error("Entrada vazia");
    std::uint64_t valor = 0;
    const auto maximo = (std::numeric_limits<std::uint64_t>::max)();
    for (char c : texto) {
        if (c < '0' || c > '9') throw std::runtime_error("Informe somente digitos de um inteiro positivo");
        const auto digito = static_cast<std::uint64_t>(c - '0');
        if (valor > (maximo - digito) / 10) throw std::runtime_error("Entrada excede uint64_t");
        valor = valor * 10 + digito;
    }
    if (valor == 0) throw std::runtime_error("O numero inicial deve ser maior que zero");
    return valor;
}

std::vector<std::uint64_t> collatz(std::uint64_t n) {
    std::vector<std::uint64_t> sequencia;
    const auto maximo = (std::numeric_limits<std::uint64_t>::max)();
    while (true) {
        if (sequencia.size() == CAPACIDADE)
            throw std::runtime_error("Sequencia excede a capacidade de 4096 elementos");
        sequencia.push_back(n);
        if (n == 1) return sequencia;
        if (n % 2 == 0) n /= 2;
        else {
            if (n > (maximo - 1) / 3)
                throw std::runtime_error("Overflow: 3*n+1 excederia uint64_t");
            n = 3 * n + 1;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        const auto inicial = lerEntrada(argc, argv);
        const auto sequencia = collatz(inicial);

        // Eventos de reset manual, inicialmente nao sinalizados.
        // Recusar objetos existentes evita misturar duas execucoes.
        Handle pronto(CreateEventW(nullptr, TRUE, FALSE, NOME_PRONTO));
        const DWORD estadoPronto = GetLastError();
        if (!pronto.valor) erroWindows("CreateEvent(pronto)");
        if (estadoPronto == ERROR_ALREADY_EXISTS)
            throw std::runtime_error("Ja existe uma sessao ativa. Encerre-a antes de iniciar outra");
        Handle lido(CreateEventW(nullptr, TRUE, FALSE, NOME_LIDO));
        const DWORD estadoLido = GetLastError();
        if (!lido.valor) erroWindows("CreateEvent(lido)");
        if (estadoLido == ERROR_ALREADY_EXISTS) throw std::runtime_error("Evento lido ja esta em uso");

        // INVALID_HANDLE_VALUE: respaldo no arquivo de paginacao, sem arquivo de dados.
        Handle memoria(CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
            0, static_cast<DWORD>(sizeof(DadosCompartilhados)), NOME_MEMORIA));
        const DWORD estadoMemoria = GetLastError();
        if (!memoria.valor) erroWindows("CreateFileMapping");
        if (estadoMemoria == ERROR_ALREADY_EXISTS) throw std::runtime_error("Memoria ja esta em uso");
        Vista vista(MapViewOfFile(memoria.valor, FILE_MAP_WRITE, 0, 0, sizeof(DadosCompartilhados)));
        if (!vista.endereco) erroWindows("MapViewOfFile");
        auto* dados = static_cast<DadosCompartilhados*>(vista.endereco);
        for (std::size_t i = 0; i < sequencia.size(); ++i) dados->valores[i] = sequencia[i];
        dados->quantidade = static_cast<std::uint64_t>(sequencia.size());

        std::cout << "PRODUTOR PID=" << GetCurrentProcessId()
                  << " endereco virtual=" << vista.endereco << '\n'
                  << "Entrada: " << inicial << "\nElementos gravados: " << dados->quantidade
                  << "\nMemoria reservada: " << sizeof(DadosCompartilhados) << " bytes\n";
        // Publicar apenas depois de escrever todos os dados.
        if (!SetEvent(pronto.valor)) erroWindows("SetEvent(pronto)");
        std::cout << "Dados prontos. Aguardando consumidor (120 segundos)..." << std::endl;
        // Mantem o objeto vivo para que o consumidor consiga abri-lo.
        aguardar(lido.valor, "consumidor nao confirmou a leitura");
        std::cout << "Leitura confirmada. Recursos serao liberados.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
}
