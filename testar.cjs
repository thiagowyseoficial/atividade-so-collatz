// Testes de integracao: executa os dois programas reais do Windows.
const fs = require('fs');
const path = require('path');
const os = require('os');
const assert = require('assert/strict');
const { spawn, spawnSync } = require('child_process');
process.chdir(__dirname);
fs.mkdirSync('evidencias', { recursive: true });
const resultados = [];
const ambiente = {
  dataUTC: new Date().toISOString(), plataforma: process.platform,
  sistema: os.version(), versao: os.release(), arquitetura: process.arch,
  commit: process.env.GITHUB_SHA || 'execucao-local',
  execucao: process.env.GITHUB_RUN_ID ? `https://github.com/${process.env.GITHUB_REPOSITORY}/actions/runs/${process.env.GITHUB_RUN_ID}` : null
};
function executar(programa, args = []) {
  const r = spawnSync(path.join(__dirname, programa + '.exe'), args,
    { encoding: 'utf8', windowsHide: true, timeout: 10000 });
  if (r.error) throw r.error;
  return { codigo: r.status, saida: r.stdout + r.stderr };
}
async function testarPar(n, esperada, testarDuplicado = false) {
  let stdout = '', stderr = '', publicado = false;
  const produtor = spawn(path.join(__dirname, 'produtor.exe'), [String(n)], { windowsHide: true });
  const terminou = new Promise((resolve, reject) => {
    produtor.once('error', reject);
    produtor.once('close', resolve);
  });
  // Evita rejeicao nao tratada se o processo nao puder iniciar.
  terminou.catch(() => {});
  const pronto = new Promise((resolve, reject) => {
    produtor.stdout.on('data', dados => {
      stdout += dados;
      if (stdout.includes('Aguardando consumidor')) { publicado = true; resolve(); }
    });
    produtor.stderr.on('data', dados => { stderr += dados; });
    produtor.once('error', reject);
    produtor.once('close', () => { if (!publicado) reject(new Error(stdout + stderr)); });
  });
  const limite = setTimeout(() => produtor.kill(), 20000);
  try {
    await pronto;
    if (testarDuplicado) {
      const outro = executar('produtor', ['5']);
      assert.equal(outro.codigo, 1);
      assert.match(outro.saida, /sessao ativa/);
      resultados.push({ teste: 'segundo produtor simultaneo', resultado: 'APROVADO', ...outro });
    }
    const consumidor = executar('consumidor');
    const codigoProdutor = await terminou;
    fs.writeFileSync(`evidencias/teste_${n}.txt`,
      `COMANDO: produtor.exe ${n}\n${stdout}${stderr}Codigo produtor: ${codigoProdutor}\n\n` +
      `COMANDO: consumidor.exe\n${consumidor.saida}Codigo consumidor: ${consumidor.codigo}\n`);
    assert.equal(codigoProdutor, 0);
    assert.equal(consumidor.codigo, 0);
    assert.match(stdout, /Leitura confirmada/);
    const obtida = consumidor.saida.match(/Sequencia: (.+)/)?.[1].trim();
    assert.equal(obtida, esperada);
    const quantidade = Number(consumidor.saida.match(/Quantidade: (\d+)/)?.[1]);
    assert.equal(quantidade, esperada.split(' ').length);
    assert.match(stdout, new RegExp(`Elementos gravados: ${quantidade}\\b`));
    const pidProdutor = stdout.match(/PID=(\d+)/)?.[1];
    const pidConsumidor = consumidor.saida.match(/PID=(\d+)/)?.[1];
    assert.ok(pidProdutor && pidConsumidor);
    assert.notEqual(pidProdutor, pidConsumidor);
    resultados.push({ teste: `Collatz ${n}`, resultado: 'APROVADO', quantidade, sequencia: obtida, pidProdutor, pidConsumidor });
  } finally {
    clearTimeout(limite);
    if (produtor.exitCode === null) produtor.kill();
  }
}
(async () => {
  assert.equal(process.platform, 'win32', 'Estes testes exigem Windows');
  await testarPar(5, '5 16 8 4 2 1');
  await testarPar(10, '10 5 16 8 4 2 1');
  await testarPar(13, '13 40 20 10 5 16 8 4 2 1', true);
  await testarPar(1, '1');
  for (const args of [[], ['0'], ['-5'], ['abc'], ['5.5'], ['5', '10'], ['18446744073709551616'], ['18446744073709551615']]) {
    const r = executar('produtor', args);
    assert.equal(r.codigo, 1);
    assert.match(r.saida, /Erro:/);
    resultados.push({ teste: `entrada invalida ${JSON.stringify(args)}`, resultado: 'APROVADO', ...r });
  }
  const semProdutor = executar('consumidor');
  assert.equal(semProdutor.codigo, 1);
  assert.match(semProdutor.saida, /Inicie primeiro/);
  resultados.push({ teste: 'consumidor sem produtor', resultado: 'APROVADO', ...semProdutor });
  fs.writeFileSync('evidencias/resultados.json', JSON.stringify({ ambiente, total: resultados.length, testes: resultados }, null, 2));
  console.log(`${resultados.length} testes APROVADOS. Evidencias salvas.`);
})().catch(erro => {
  fs.writeFileSync('evidencias/falha.txt', String(erro.stack || erro));
  console.error(erro);
  process.exitCode = 1;
});
