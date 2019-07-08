# Manual do usuário

## Inicialização
Quando o dispositivo é iniciado pela primeira vez no display é exibido a mensagem _"Configurando horario"_.
No termial é exibido _"CONFIGURACAO DE HORARIO"_ e é solicitado as horas (_"Hora: "_) e depois os minutos (_"Minuto: "_).

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

#### 1. Cadastrar remedio
Ao escolher esta opção, é exibido _"Cadastrando remedio"_ no LCD e _"CADASTRO DE REMEDIO"_ no terminal. Em seguida há duas possibilidades, uma delas ocorre quando todas as posições já possuem remédios associados, situação na qual são exibidas as mensagens _"Nenhuma posicao disponivel!"_ e _"CADASTRO ENCERRADO"_, retornando para o horário no LCD e finalizando a operação de cadastro.

A outra possibilidade é quando há pelo menos uma posição disponível, sendo então exibida a mensagem _"Posição: "_, solicitando um número para a posição. Respostas válidas para esta solicitação são os números 1-5, com exceção dos números cujas posições já possuem um remédio cadastrado. Esta solicitação não requer a tecla _"ENTER"_, indo para a próxima etapa logo que recebe um número válido.

Em sequência o dispositivo gira até a posição especificada e exibe _"Coloque os comprimidos no compartimento"_ seguido de _"Quantidade de comprimidos guardados: "_ no terminal. É esperado que os comprimidos sejam depositados **antes** de se informar a quantidade. A quantidade informada deve ser um número de dois algarismos entre 00-99, caso contrário _"Quantidade de comprimidos guardados: "_ é exibido novamente, aguardando nova resposta, até que o informado seja válido.

Obtendo uma resposta válida, o motor retorna para a posição inicial e depois exibe _"Quantidade de comprimidos por dose: "_ no terminal. Resposta do tipo número de um algarismo entre 1-9. Após receber um número correto prossegue sem a tecla _"ENTER"_.

_"Informe o horario que o remedio deve ser tomado!"_ é exibido, seguido de _"Hora: "_ e _"Minuto: "_, similarmente ao processo descrito em [Inicialização](../master/USER_MANUAL.md#inicialização).

Depois _"Informe o intervalo que o remedio deve ser tomado!"_ é exibido, seguido de _"Intervalo em horas: "_ repetidamente até obter uma resposta válida (número de 00-24), sendo que 00 e 24 ambos representam 1 vez ao dia.

Por fim _"MEDICAMENTO CADASTRADO!"_ é exibido no terminal, indicando que o remédio foi cadastrado, e encerrando o menu.

#### 2. Remover remedio
É exibido _"Removendo remedio"_ no LCD e _"REMOCAO DE REMEDIO"_ no terminal. Similarmente ao [1. Cadastrar remedio], há duas possibilidades.

Uma delas é que não há nenhum remédio cadastrado para ser removido, o que mostra as mensagens _"Nenhum remedio para remover!"_ e _"REMOCAO ENCERRADA!"_, encerrando a remoção dos remédios e o menu.

#### 3. Reabastecer remedio

#### 4. Listar remedios

#### 5. Configurar horario
Idem ao processo descrito em [Inicialização].

#### 6. Voltar
Exibe _"Menu encerrado"_ e encerra o menu.

[Inicialização]:USER_MANUAL.md#inicialização
[1. Cadastrar remedio]:USER_MANUAL.md#1-cadastrar-remedio
