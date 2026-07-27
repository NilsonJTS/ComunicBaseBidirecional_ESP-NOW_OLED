Diário de dificuldades encontradas:

27/07/26: Comprei no MercadoLivre:
          Duas unidades de sensor de corrente não invasivo 100A Sct-013 R$94,80
          Kit 10 placas prototipo de fenolite 5x7cm perfurada R$19,00
          pontas de prova para meu multimetro que estragaram R$22,50

26/07/26: implementação de botão html no site que quando clicado envia mensagem para o receptor que envia para o emissor, fazendo com que ele acione o aquecedor por 30seg, e também foi instalado um led virtual(bolinha) no site, que acende(fica vermelho com glare) quando a porta 34 do esp recebe sinal (que será enviado por um optoacoplador) quando ficar confirmado que o aparelho foi energizado.
    Houve diversas complicações neste dia, primeiro nada funcionou porque a IA sugeriu mudar completamente um código, e mudou a pinagem sem avisar. Depois a IA sugeriu uma mudança de código com intenção de resolver um problema, mas não configurou o visor oled e ele não ligou, demorei muito até ver que não estava com defeito. E por fim, a saída gpio19 não conseguia enviar 3.3v padrão ao ser acionada pelo botão do site, ela enviava apenas 0.76v (causa ainda não determinada), eu consegui detectar que estava tudo funcionando porque coloquei o multimetro e vi que ela acionava por exatamente 30seg, como deveria ser, porém enviava tensão insuficiente. Tudo foi testado por horas, e o código foi alterado diversas vezes.     

26/07/26: Comprei no MercadoLivre:
            Duas unidades de relé de estado sólido Fotek Ssr-25da 25a 3-32vdc/24-380vac R$59,20
            Duas unidades de dissipador para relé de estado solido em alumínio R$39,90
            Duas unidades de protoboard 400furos R$28,23

25/07/26: Implementado servidor web assincrono com LittleFS(espaço na memoria flash do dispositivo destinada aos arquivos do site: html,css,js) no      dispositivo receptor, que ficará na sala de veterinários e se comunicará futuramente com o roteador. Não houve grandes dificuldades, a maior dificuldade foi aprender a subir os arquivos para LittleFS que precisam de um comando de prompt específico e não sobem junto com upload do receptor.
    Inicio de documentação. Provavelmente o item que mais tomou tempo. tem o objetivo de criar um documento que eu possa enviar para a IA em um novo prompt sempre que necessário, de forma que imediatamente a IA saiba de onde prosseguir apenas com envio de pdf.
    Instalei extensão Markdown PDF de yzane, para criar novo pdf a cada nova versão de documentação.

23/07/26: Foi necessário adquirir um hub para poder trabalhar os códigos simultaneamente e poder subir e testar sem precisar plugar e desplugar o tempo todo.
          Não pode ser um hub passivo, pois precisei de um cabo extensor para alcançar o computador, e o sinal não era forte o suficiente para alimentar os dois dispositivos e transitar dados, logo o hub precisava aceitar alimentação externa. Além do cabo de extensão precisei também de uma fonte de alimentação de 5v com 3A e cabos individuais para cada dispositivo, além de adaptadores. Tudo ficou em R$90,00

22/07/26: Inserção de DHT11, incremento de pacote de dados (struct) para receber os dados de Umidade e Temperatura além da contagem de pacotes já implementada antes. Não houve dificuldades. 

21/07/26: Comunicação básica entre dispositivos, não houve dificuldades, precisei etiquetar os dispositivos com sua mascara MAC e definir as funções de cada um a única comunicação neste dia foi a contagem de envios de pacotes a cada 1seg. 


### Lista de passos já executados ###
1 - Criar comunicação simples apenas transmitindo contador de envio de pacotes a cada 1seg
2 - Inserir DHT11 fornecendo temperatura e umidade em ambos visores (emissor, receptor)
3 - Implementar site no receptor (LittleFS), para acesso e visualização de dados DHT11 via roteador esp32
4 - Implementar site: botão que aciona porta GPIO23 emissor por 30seg e led virtual que acende 30seg com retorno de GPIO23 em GPIO34 