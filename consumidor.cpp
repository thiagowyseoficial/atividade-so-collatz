#include "compartilhado.h"

int main(int argc, char*[]) {
    try {
        if (argc != 1) throw std::runtime_error("Uso: consumidor.exe (sem argumentos)");
        Handle memoria(OpenFileMappingW(FILE_MAP_READ, FALSE, NOME_MEMORIA));
        if (!memoria.valor) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
                throw std::runtime_error("Memoria nao encontrada. Inicie primeiro produtor.exe <numero>");
            erroWindows("OpenFileMapping");
        }
        Vista vista(MapViewOfFile(memoria.valor, FILE_MAP_READ, 0, 0, sizeof(DadosCompartilhados)));
        if (!vista.endereco) erroWindows("MapViewOfFile");
        Handle pronto(OpenEventW(SYNCHRONIZE, FALSE, NOME_PRONTO));
        if (!pronto.valor) erroWindows("OpenEvent(pronto)");
        Handle lido(OpenEventW(EVENT_MODIFY_STATE, FALSE, NOME_LIDO));
        if (!lido.valor) erroWindows("OpenEvent(lido)");
        std::cout << "CONSUMIDOR PID=" << GetCurrentProcessId()
                  << " endereco virtual=" << vista.endereco << '\n'
                  << "Aguardando sinal de dados prontos..." << std::endl;
        aguardar(pronto.valor, "produtor nao sinalizou dados prontos");

        // Nenhum campo compartilhado e lido antes da espera terminar.
        const auto* dados = static_cast<const DadosCompartilhados*>(vista.endereco);
        const auto quantidade = dados->quantidade;
        if (quantidade == 0 || quantidade > CAPACIDADE)
            throw std::runtime_error("Quantidade invalida na memoria compartilhada");
        std::cout << "Quantidade: " << quantidade << "\nSequencia: ";
        for (std::uint64_t i = 0; i < quantidade; ++i) {
            if (i != 0) std::cout << ' ';
            std::cout << dados->valores[i];
        }
        std::cout << std::endl;
        if (!std::cout) throw std::runtime_error("Falha ao apresentar a sequencia");
        if (!SetEvent(lido.valor)) erroWindows("SetEvent(lido)");
        std::cout << "Leitura concluida. Recursos serao liberados.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }
}
