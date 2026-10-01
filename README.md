# Collatz com memória compartilhada no Windows

**Disciplina:** Sistemas Operacionais  
**Professor:** Michael Douglas C Alves  
**Alunos:** Vitor Hugo, Thiago Wyse e Matheus Bertemes

**Repositório:** https://github.com/thiagowyseoficial/atividade-so-collatz

Dois programas independentes em C++17: o produtor calcula a sequência de Collatz e a grava em memória compartilhada; o consumidor lê e apresenta os valores. Dois eventos nomeados controlam a publicação dos dados e a confirmação da leitura.

## Arquivos

- `produtor.cpp`: validação da entrada, cálculo de Collatz e publicação dos dados.
- `consumidor.cpp`: leitura sincronizada da memória e apresentação da sequência.
- `compartilhado.h`: estrutura de dados, nomes dos objetos e liberação de recursos.
- [relatorio.pdf](relatorio.pdf): fundamentação, implementação, diagrama, respostas às 15 questões e conclusão.

## Compilar e executar

No **Developer Command Prompt x64** do Visual Studio com C++ e Windows SDK, dentro desta pasta:

```bat
cl /nologo /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 produtor.cpp /Fe:produtor.exe
cl /nologo /std:c++17 /EHsc /W4 /WX /O2 /MT /utf-8 consumidor.cpp /Fe:consumidor.exe
```

Em um primeiro terminal PowerShell:

```powershell
.\produtor.exe 5
```

Após aparecer “Dados prontos. Aguardando consumidor”, em outro terminal na mesma pasta:

```powershell
.\consumidor.exe
```

Inicie o consumidor em até 120 segundos. Use a mesma sessão do Windows e aguarde ambos encerrarem antes de iniciar outra rodada.

## Checklist de conformidade com o enunciado

### Implementação concluída e revisada no código

- [x] Dois programas independentes em C++ para Windows.
- [x] Produtor recebe o valor inicial pela linha de comando.
- [x] Regras de Collatz, incluindo o valor inicial e o 1 final.
- [x] Validação de entrada, overflow e capacidade máxima.
- [x] Memória compartilhada real com `CreateFileMapping` e `MapViewOfFile`.
- [x] Consumidor usa `OpenFileMapping` e mapeia a mesma região.
- [x] Consumidor lê os dados sem recalcular a sequência.
- [x] Sincronização com eventos, `SetEvent` e `WaitForSingleObject`.
- [x] Leitura somente após a publicação dos dados.
- [x] Organização por quantidade e vetor de `uint64_t`.
- [x] Justificativa dos 32.776 bytes, capacidade de 4096 elementos e limites.
- [x] Liberação com `UnmapViewOfFile` e `CloseHandle`.

### Relatório e compilação

- [x] Objetivo, fundamentação teórica e explicação da implementação.
- [x] Diagrama com processos, espaços virtuais, região compartilhada e sincronização.
- [x] Respostas às 15 questões conceituais e à questão central.
- [x] Conclusão e referências da documentação Windows.
- [x] Identificação dos três alunos.
- [x] Compilação de ambos os fontes concluída sem erros nem avisos em 01/10/2026, usando MSVC 14.51.36231 e as opções acima. O comando de compilação terminou com código 0.
- [ ] Anexar captura ou log de compilação à versão final para facilitar a conferência pelo professor. O registro externo da primeira compilação foi removido durante a limpeza da pasta; o relatório descreve o procedimento observado.

### Execução e resultados ainda pendentes

- [ ] Executar produtor e consumidor com entrada **5** e registrar ambos os terminais.
- [ ] Executar produtor e consumidor com entrada **10** e registrar ambos os terminais.
- [ ] Executar produtor e consumidor com entrada **13** e registrar ambos os terminais.
- [ ] Anexar as evidências reais e atualizar os resultados no relatório.
- [ ] Ensaiar a demonstração para execução em sala na semana seguinte.

| Entrada | Sequência esperada | Quantidade |
| --- | --- | --- |
| 5 | 5 16 8 4 2 1 | 6 |
| 10 | 10 5 16 8 4 2 1 | 7 |
| 13 | 13 40 20 10 5 16 8 4 2 1 | 10 |

**Esses resultados são esperados, não observados.** O Windows bloqueou a inicialização dos executáveis com a mensagem “An Application Control policy has blocked this file”. Não foi possível confirmar a comunicação entre processos em execução nesta máquina. Compilar sem erros não substitui os testes exigidos.

### Formato e entrega

- [x] Publicar os fontes e o relatório no repositório público.
- [x] Inserir a URL do repositório no PDF.
- [ ] Confirmar com o professor a aceitação de **três alunos**, pois a capa do enunciado especifica atividade **individual**, embora outras seções mencionem “grupo”.
- [ ] Confirmar que o **link do GitHub substitui o RAR** pedido no enunciado. Por orientação dos alunos, este projeto não contém RAR.
- [ ] Realizar a entrega no canal definido pelo professor. O prazo informado no enunciado é **01/10/2026, das 19h00 às 22h30**; publicar no GitHub não comprova a entrega nesse canal.

Checklist elaborado com base no PDF “Atividade Complementar — Memória Virtual, Memória Compartilhada e Comunicação entre Processos”, fornecido na disciplina.
