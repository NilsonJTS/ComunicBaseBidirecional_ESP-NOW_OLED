<?php
// Configurações de Conexão com o Banco de Dados Hostgator
$servername = "localhost";
$username   = "estude43_userNilson"; // Seu usuário do banco
$password   = "GjTX@@jqyD@E";        // A senha que você criou
$dbname     = "estude43_estacao_db"; // O nome completo do banco

// 1. Recebe o conteúdo JSON enviado na requisição HTTP POST
$json_recebido = file_get_contents('php://input');

// 2. Decodifica o JSON para um objeto/array PHP
$dados = json_decode($json_recebido, true);

// 3. Valida se os dados chegaram corretamente
if ($dados !== null) {
    // Abre a conexão com o banco MySQL
    $conn = new mysqli($servername, $username, $password, $dbname);

    // Verifica se houve erro de conexão
    if ($conn->connect_error) {
        http_response_code(500);
        die("Falha na conexao com o banco: " . $conn->connect_error);
    }

    // Extrai as variáveis do JSON (com valores padrão para segurança)
    $estacao_id       = isset($dados['estacao_id']) ? $dados['estacao_id'] : 'ESTACAO_01';
    $temperatura      = (float)$dados['temperatura'];
    $umidade          = (float)$dados['umidade'];
    $contador         = (int)$dados['contador'];
    $energia_ok       = $dados['energia_ok'] ? 1 : 0;
    $aquecedor_ligado = $dados['aquecedor_ligado'] ? 1 : 0;
    $exaustao_ligada  = $dados['exaustao_ligada'] ? 1 : 0;
    $exaustao_ok      = $dados['exaustao_ok'] ? 1 : 0;

    // 4. Prepara o comando SQL de Inserção (Prepared Statement para Segurança)
    $stmt = $conn->prepare("INSERT INTO leituras_sensores 
        (estacao_id, temperatura, umidade, contador, energia_ok, aquecedor_ligado, exaustao_ligada, exaustao_ok) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    $stmt->bind_param("sddiiiii", 
        $estacao_id, 
        $temperatura, 
        $umidade, 
        $contador, 
        $energia_ok, 
        $aquecedor_ligado, 
        $exaustao_ligada, 
        $exaustao_ok
    );

    // 5. Executa a gravação no MySQL
    if ($stmt->execute()) {
        http_response_code(200);
        echo json_encode(["status" => "sucesso", "mensagem" => "Dado gravado com sucesso!"]);
    } else {
        http_response_code(500);
        echo json_encode(["status" => "erro", "mensagem" => "Erro ao gravar no banco: " . $stmt->error]);
    }

    $stmt->close();
    $conn->close();
} else {
    // Retorna erro se nenhum JSON válido for recebido
    http_response_code(400);
    echo json_encode(["status" => "erro", "mensagem" => "Nenhum dado JSON valido recebido."]);
}
?>