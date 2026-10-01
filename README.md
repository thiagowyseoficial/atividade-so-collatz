# Collatz com memória compartilhada no Windows

**Disciplina:** Sistemas Operacionais  
**Professor:** Michael Douglas C Alves  
**Alunos:** Vitor Hugo, Thiago Wyse e Matheus Bertemes

**Repositório:** https://github.com/thiagowyseoficial/atividade-so-collatz

**Validação:** [14 testes aprovados no Windows](https://github.com/thiagowyseoficial/atividade-so-collatz/actions/runs/36939906782), em 01/10/2026. [Resultados](evidencias/resultados.json) · [Log de compilação](evidencias/compilacao.txt).

Dois programas independentes em C++17: o produtor calcula a sequência de Collatz e a grava em memória compartilhada; o consumidor lê e apresenta os valores. Dois eventos nomeados controlam a publicação dos dados e a confirmação da leitura.

## Arquivos

- `produtor.cpp`: validação da entrada, cálculo de Collatz e publicação dos dados.
- `consumidor.cpp`: leitura sincronizada da memória e apresentação da sequência.
- `compartilhado.h`: estrutura de dados, nomes dos objetos e liberação de recursos.
- [relatorio.pdf](relatorio.pdf): fundamentação, implementação, diagrama, respostas às 15 questões e conclusão.
- `compilar.bat` e `testar.cjs`: compilação e testes reproduzíveis.
- `evidencias/`: logs reais da compilação e das execuções no Windows do GitHub Actions.

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

Para automatizar, execute `compilar.bat` e depois `node testar.cjs` (requer Node.js). O workflow em `.github/workflows/testes-windows.yml` também executa essas etapas em um Windows Server 2022 do GitHub Actions.

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
- [x] Compilação de ambos os fontes concluída sem erros nem avisos em 01/10/2026 no Windows Server 2022 x64, usando o compilador MSVC 19.44.35229. O comando terminou com código 0.
- [x] Anexar o [log real de compilação](evidencias/compilacao.txt), também reproduzido no PDF.

### Execução e resultados

- [x] Executar produtor e consumidor com entrada **5** e [registrar as saídas](evidencias/teste_5.txt).
- [x] Executar produtor e consumidor com entrada **10** e [registrar as saídas](evidencias/teste_10.txt).
- [x] Executar produtor e consumidor com entrada **13** e [registrar as saídas](evidencias/teste_13.txt).
- [x] Anexar as evidências reais e atualizar os resultados no relatório.
- [ ] Ensaiar a demonstração para execução em sala na semana seguinte.

| Entrada | Sequência observada | Quantidade |
| --- | --- | --- |
| 5 | 5 16 8 4 2 1 | 6 |
| 10 | 10 5 16 8 4 2 1 | 7 |
| 13 | 13 40 20 10 5 16 8 4 2 1 | 10 |

**Resultados reais:** em cada teste acima, produtor e consumidor executaram como processos diferentes, retornaram código 0 e confirmaram a leitura. Os valores observados coincidiram com os esperados. O bloqueio de aplicativos ocorreu no Windows local; a validação foi concluída no Windows remoto do GitHub Actions.

Foram aprovadas 14 verificações: as entradas 5, 10, 13 e 1; segundo produtor simultâneo; oito entradas inválidas; e consumidor sem produtor. A [execução verificável](https://github.com/thiagowyseoficial/atividade-so-collatz/actions/runs/36939906782) corresponde ao commit `180bce14e27235806f719514f57bb52dac328a0e`. As alterações posteriores de documentação não modificam os fontes testados.

O roteiro não exercita automaticamente o consumidor aguardando antes da publicação, o timeout de 120 segundos nem o limite de 4096 elementos. A ordenação da escrita e da sinalização foi revisada no código; os testes não provam ausência de todas as condições de corrida.

Os logs foram preservados neste repositório. Os executáveis compilados também estão no artefato `evidencias-windows` da execução, com retenção de 30 dias.

### Roteiro para a demonstração em sala

1. Compilar com `compilar.bat` e abrir dois terminais nesta pasta.
2. Executar `produtor.exe 13` e mostrar que ele permanece aguardando a confirmação.
3. Executar `consumidor.exe` no segundo terminal e mostrar a sequência e os PIDs distintos.
4. Comparar os endereços virtuais exibidos: eles podem ser diferentes, embora as vistas acessem os mesmos dados.
5. Explicar os eventos: `Pronto` libera a leitura; `Lido` permite o encerramento do produtor. Repetir com 5 e 10.

O roteiro está preparado; o ensaio presencial ainda depende dos alunos e do computador da apresentação.

### Formato e entrega

- [x] Publicar os fontes e o relatório no repositório público.
- [x] Inserir a URL do repositório no PDF.
- [ ] Realizar a entrega no canal definido pelo professor. O prazo informado no enunciado é **01/10/2026, das 19h00 às 22h30**; publicar no GitHub não comprova a entrega nesse canal.

Checklist elaborado com base no PDF “Atividade Complementar — Memória Virtual, Memória Compartilhada e Comunicação entre Processos”, fornecido na disciplina.
