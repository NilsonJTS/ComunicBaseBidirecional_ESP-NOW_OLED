function atualizarDados() {

    const ledExaustao = document.getElementById('ledExaustao');
    const textoExaustao = document.getElementById('textoExaustao');

    fetch('/dados?t=' + new Date().getTime(), { cache: 'no-store' })
        .then(response => {
            if (!response.ok) throw new Error('Erro na resposta');
            return response.json();
        })
        .then(data => {
            // Atualiza os valores na tela
            document.getElementById('temp').innerText = data.temperatura;
            document.getElementById('umid').innerText = data.umidade;
            document.getElementById('contador').innerText = data.contador;
            
            // Atualiza o LED de retorno de energia
            const led = document.getElementById('ledStatus');
            if (data.energia_ok) {
                led.className = 'led ligado';
            } else {
                led.className = 'led desligado';
            }
        
             if (data.exaustao_ok) {
            ledExaustao.className = "led ligado";
            textoExaustao.innerText = "Ligado";
            }else{
                ledExaustao.className = "led desligado";
                textoExaustao.innerText = "Desligado";
            }
        
        })
        .catch(err => console.log('Aguardando dados...:', err));
}


function acionarAquecedor() {
    const btn = document.getElementById('btnAquecedor');
    if (btn) btn.disabled = true;

    fetch('/ligar-aquecedor', { 
        method: 'POST',
        cache: 'no-store'
    })
    .then(response => {
        console.log('Comando enviado com sucesso');
        // Força uma atualização imediata do painel após o clique
        atualizarDados();
    })
    .catch(err => console.log('Erro ao acionar:', err))
    .finally(() => {
        // Reabilita o botão após 1 segundo
        setTimeout(() => {
            if (btn) btn.disabled = false;
        }, 1000);
    });
}

// Garante o loop contínuo de atualização a cada 2 segundos
setInterval(atualizarDados, 2000);

// Executa a primeira leitura assim que a página carrega
document.addEventListener('DOMContentLoaded', atualizarDados);