# Projeto final da disciplina de Sistemas Microcontrolados 
Desenvolvido pelos estudandes Iago Rockel e Renata Costa.

### Proposta
A proposta do projeto é desenvolver um dispositivo para auxiliar idosos a tomarem remédios nas horas e quantidades corretas, que será configurado por algum familiar ou cuidador.

### Estrutura
A estrutura do dispositivo será uma caixa circular com alguns compartimentos para colocar os remédios, e uma tampa rotativa que possui um furo para acessar o remédio do compartimento sob este.

### Funcionamento
O responsável deverá configurar a hora do dispositivo (pois não será utilizado um módulo RTC) e depois terá a opção de cadastrar ou remover medicamentos. Este cadastro inclui:
* Posição do remédio (qual compartimento da caixa)
* Horário que o remédio deve ser tomado
* Quantidade compreendida por uma dose do medicamento
* Intervalo em horas entre doses do remédio

Tendo medicamentos cadastrados, quando estiver na hora de serem ingeridos, o dispositivo toca um *beep* para o idoso perceber que deve tomar o remédio e, a tampa gira para o compartimento correspondente através de um motor de passo. No *display* será exibido a quantidade de remédios que devem ser tomados e, ao final, o idoso deve apertar um botão no teclado matricial para indicar que tomou o remédio, momento no qual o *beep* para de tocar e a tampa retorna para sua posição inicial, onde não há medicamento.

### Desenvolvimento
O que foi feito até o momento.

#### 18/06/19
* Adição de comentários ao código fonte
* Formatação do código fonte
* Atualização do **TODO**

#### 17/06/19
* Configuração do *display LCD* - OK 
* Implementação do teclado matricial - OK
* Configurar horário - OK
* Adicionar medicamento - **Incompleto**

### TODO
* Implementar menu através de interrupção
* Salvar os medicamentos na memória EEPROM
* Implementar função para remover medicamentos
* Implementar lógica para tomar um remédio (*beep, display* e motor de passo)
* Finalizar a função *Adiciona_Remedio*
* Otimizar código
* Padronizar nomenclaturas
