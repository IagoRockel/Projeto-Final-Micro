# Manual do usuário

## Inicialização
Quando o dispositivo é iniciado pela primeira vez no display é exibido a mensagem *"Configurando horario"*.
Na UART é exibido *"CONFIGURACAO DE HORARIO"* e é solicitado as horas e depois os minutos.

Ambas as solicitações requerem um número de dois algarismos, refazendo a solicitação caso o usuário informe um valor inválido, sendo este qualquer caractér que não seja um número, um número negativo, um número que não seja de dois dígitos, ou um número que exceda o valor máximo do campo respectivo (23 para as horas e 59 para os minutos).

Após o fornecimento de valores válidos para os campos, é exibido a mensagem *"Horario configurado!"* na UART e o display LCD mostra o horário no formato *hh:mm:ss*.

## Menu
Após a inicialização, o menu pode ser acessado, através do botão *SW1*, exibindo *"Menu aberto"* no LCD e *"MENU"* no UART, seguido das opções:
1. Cadastrar remedio
1. Remover remedio
1. Reabastecer remedio
1. Listar remedios
1. Configurar horario
1. Voltar

O terminal fica aguardando algum desses digitos, qualquer outro não fará nada. Após clicar em um dos números, o usuário é direcionado para a opção escolhida, mesmo sem apertar a tecla *"ENTER"*.
