function atualizarDados() {
  // Adiciona a hora atual na URL para evitar que o navegador use dados em cache
  fetch('/dados?t=' + new Date().getTime())
    .then(response => response.json())
    .then(data => {
      document.getElementById('temp').innerText = data.temperatura;
      document.getElementById('umid').innerText = data.umidade;
      document.getElementById('contador').innerText = data.contador;
    })
    .catch(err => console.log('Erro ao buscar dados:', err));
}

// Executa a função a cada 2000ms (2 segundos)
setInterval(atualizarDados, 2000);