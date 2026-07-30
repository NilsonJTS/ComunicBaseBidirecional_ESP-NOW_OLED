CREATE TABLE leituras_sensores (
    id INT AUTO_INCREMENT PRIMARY KEY,
    estacao_id VARCHAR(20) NOT NULL DEFAULT 'ESTACAO_01',
    temperatura FLOAT(4,1) NOT NULL,
    umidade FLOAT(4,1) NOT NULL,
    contador INT NOT NULL,
    energia_ok TINYINT(1) NOT NULL,
    aquecedor_ligado TINYINT(1) NOT NULL DEFAULT 0,
    exaustao_ligada TINYINT(1) NOT NULL,
    exaustao_ok TINYINT(1) NOT NULL,
    data_hora TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);