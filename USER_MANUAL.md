# Manual do usuário

## Inicialização
Quando o dispositivo é iniciado pela primeira vez no display é exibido a mensagem _"Configurando horario"_.
No terminal é exibido _"CONFIGURACAO DE HORARIO"_ e é solicitado as horas (_"Hora: "_) e depois os minutos (_"Minuto: "_).

Ambas as solicitações requerem um número de dois algarismos, refazendo a solicitação caso o usuário informe um valor inválido, sendo este qualquer caractér que não seja um número, um número negativo, um número que não seja de dois dígitos, ou um número que exceda o valor máximo do campo respectivo (23 para as horas e 59 para os minutos).

Após o fornecimento de valores válidos para os campos, é exibido a mensagem _"Horario configurado!"_ no terminal e o display LCD mostra o horário no formato _hh:mm:ss_.

## Menu
Após a inicialização, o menu pode ser acessado, através do botão _SW1_, exibindo _"Menu aberto"_ no LCD e _"MENU"_ no terminal, seguido das opções:
1. Cadastrar remedio
1. Remover remedio
1. Reabastecer remedio
1. Listar remedios
1. Configurar horario
1. Voltar

O terminal fica aguardando algum desses digitos, qualquer outro não fará nada. Após clicar em um dos números, o usuário é direcionado para a opção escolhida, sem apertar a tecla _"ENTER"_.

### 1. Cadastrar remedio
Ao escolher esta opção, é exibido _"Cadastrando remedio"_ no LCD e _"CADASTRO DE REMEDIO"_ no terminal. Em seguida há duas possibilidades, uma delas ocorre quando todas as posições já possuem remédios associados, situação na qual são exibidas as mensagens _"Nenhuma posicao disponivel!"_ e _"CADASTRO ENCERRADO"_, retornando para o horário no LCD e finalizando a operação de cadastro.

A outra possibilidade é quando há pelo menos uma posição disponível, sendo então exibida a mensagem _"Posição: "_, solicitando um número para a posição. Respostas válidas para esta solicitação são os números 1-5, com exceção dos números cujas posições já possuem um remédio cadastrado. Esta solicitação não requer a tecla _"ENTER"_, indo para a próxima etapa logo que recebe um número válido.

Em sequência o dispositivo gira até a posição especificada e exibe _"Coloque os comprimidos no compartimento"_ seguido de _"Quantidade de comprimidos guardados: "_ no terminal. É esperado que os comprimidos sejam depositados **antes** de se informar a quantidade. A quantidade informada deve ser um número de dois algarismos entre 00-99, caso contrário _"Quantidade de comprimidos guardados: "_ é exibido novamente, aguardando nova resposta, até que o informado seja válido.

Obtendo uma resposta válida, o motor retorna para a posição inicial e depois exibe _"Quantidade de comprimidos por dose: "_ no terminal. Resposta do tipo número de um algarismo entre 1-9. Após receber um número correto prossegue sem a tecla _"ENTER"_.

_"Informe o horario que o remedio deve ser tomado!"_ é exibido, seguido de _"Hora: "_ e _"Minuto: "_, similarmente ao processo descrito em [Inicialização].

Depois _"Informe o intervalo que o remedio deve ser tomado!"_ é exibido, seguido de _"Intervalo em horas: "_ repetidamente até obter uma resposta válida (número de 00-24), sendo que 00 e 24 ambos representam 1 vez ao dia.

Por fim _"MEDICAMENTO CADASTRADO!"_ é exibido no terminal, indicando que o remédio foi cadastrado, e encerrando o menu.

### 2. Remover remedio
É exibido _"Removendo remedio"_ no LCD e _"REMOCAO DE REMEDIO"_ no terminal. Similarmente ao [1. Cadastrar remedio], há duas possibilidades.

Uma delas é que não há nenhum remédio cadastrado para ser removido, o que mostra as mensagens _"Nenhum remedio para remover!"_ e _"REMOCAO ENCERRADA!"_, encerrando a remoção dos remédios e o menu.

A outra ocorre quando existe pelo menos um remédio cadastrado, exibindo _"Posicao: "_ e solicitando um número de 1-5, desde que o número informado contenha um remédio cadastrado, e espera até receber o mesmo, prosseguindo direto para a próxima etapa, sem a tecla _"ENTER"_.

Caso existam comprimidos no compartimento escolhido, o motor gira até a posição correta e no terminal são exibidas as mensagens _"Retire os comprimidos do compartimento!"_ e _"Comprimidos retirados? [s/n]: "_. Os comprimidos devem ser retirados **antes** de se responder _"s"_, que causa o retorno do motor para a posição inicial. Qualquer outro caractér informado, incluindo _"n"_ causa a repetição das frases, mas somente quando o caracter for _"n"_ o mesmo será exibido no terminal.

Por fim, ou caso o número de comprimidos fosse 0, é exibido a mensagem _"REMEDIO REMOVIDO!"_, informando que o remédio foi removido e encerrando o menu.

### 3. Reabastecer remedio
Exibe _"Abastecendo remdio"_ no LCD e _"ABASTECIMENTO DE REMEDIO"_ no terminal. Realiza a mesma verificação de [2. Remover remedio], exibindo _"Nenhum remedio para abastecer!"_ e _"ABASTECIMENTO ENCERRADO!"_ caso não exista nenhum remédio cadastrado.

Caso contrário, é solicidado _"Posicao: "_, novamente sendo válido somente um número de 1-5 que contenha um remédio cadastrado, sem a tecla _"ENTER"_. O dispositivo gira até o compartimento especificado e pergunta _"Quantos comprimidos foram adicionados: "_, sendo válido números de 00-99. Os comprimidos devem ser guardados **antes** de responder, pois o motor retorna a posição inicial logo em seguida, exibindo _"ABASTECIMENTO REALIZADO!"_, indicando que o abastecimento foi computado e finalizando o menu.

### 4. Listar remedios
No LCD é mostrado _"Listando remedios"_, enquanto no terminal aparece _"LISTA DE REMEDIOS"_, seguido da lista de remédios, nos formatos seguintes:

Para posições com remédios cadastrados:
>REMEDIO 2  
> \- Proxima dose: 12h30  
> \- Comprimidos por dose: 1  
> \- Intervalo entre doses: 8h  
> \- Comprimidos em estoque: 10

Para posições com remédios não cadastrados:
>REMEDIO 4  
> \- Nao cadastrado

Após exibidos todos os remédios (1-5):
>FIM DA LISTA!

Depois de exibida, o menu é encerrado.

### 5. Configurar horario
Idem ao processo descrito em [Inicialização].

### 6. Voltar
Exibe _"Menu encerrado"_ e encerra o menu.

## Alarmes
Foram configurados dois alarmes, um para sinalizar que um ou mais remédios devem ser tomados e outro para sinalizar que algum remédio está sem comprimidos no estoque.

### Alarme de remédios
Este alarme é ativado quando está na hora de tomar, pelo menos, um remédio. Seu comportamento é fazer o LED verde piscar.

O alarme é dasativado quando todos os remédios que deviam ser tomados, foram tomados, indicado pelo botão _"SW2"_.

### Alarme de estoque
Este alarme é ativado quando pelo menos um dos remédios cadastrados está sem comprimidos no estoque, fazendo com que o LED vermelho pisque.

É esperado que o usuário, quando perceber este alarme, liste os remédios através de [4. Listar remedios] para verificar qual deles está sem comprimidos e depois, utilizando [3. Reabastecer remedio], acrescente mais comprimidos, fazendo com que o alarme seja desativado.

## Tomar um medicamento
Quando o [Alarme de remédios] estiver ativo, o dispositivo gira até o remédio que deve ser tomado, e mostra _"TOMAR REMEDIO X"_ e _"Y COMPRIMIDOS"_ no LCD.

**Após** tomar os remédios, deve ser apertado _"SW2"_, que faz com que o dispositivo retorne para a posição inicial, ou, caso exista outro remédio para ser tomado no mesmo horário (ou em outro horário, mas que tenha dado o tempo até a sinalização de tomar o remédio anterior), vai para o remédio seguinte, até não haver mais remédios a serem tomados.


[Inicialização]:USER_MANUAL.md#inicialização
[1. Cadastrar remedio]:USER_MANUAL.md#1-cadastrar-remedio
[2. Remover remedio]:USER_MANUAL.md#2-remover-remedio
[2. Remover remedio]:USER_MANUAL.md#3-reabastecer-remedio
[4. Listar remedios]:USER_MANUAL.md#4-listar-remedios
[Alarme de remédios]:USER_MANUAL.md#alarme-de-remédios
