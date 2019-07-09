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
* Quantidade de comprimidos guardados no compartimento
* Quantidade compreendida por uma dose do medicamento
* Intervalo em horas entre doses do remédio

Tendo medicamentos cadastrados, quando estiver na hora de serem ingeridos, o dispositivo aciona um alarme (LED verde) para o idoso perceber que deve tomar o remédio e, a tampa gira para o compartimento correspondente através de um motor de passo. No *display* será exibido a quantidade de remédios que devem ser tomados e, ao final, o idoso deve apertar um botão (SW2) para indicar que tomou o remédio, momento no qual o alarme para de piscar e a tampa retorna para sua posição inicial, onde não há medicamento.

### Melhorias futuras
Algumas melhorias que poderiam ser implementadas futuramente são a utilização da memória EEPROM, para armazenar as informações a respeito dos remédios cadastrados; a implementação de uma senha para editar informações; utilizar outras formas de alarmes/alertas; desenvolver um aplicativo para celular que possa realizar as configurações; implementar um módulo RTC; entre outras possibilidades.

### Log de desenvolvimento
O que foi feito até o momento.

#### 08/07/19
* Criação de um [manual de usuário](USER_MANUAL.md)
* Correção de um bug e na função _abasteceRemedio_
* Aprimoramento da lógica da função _abasteceRemedio_
* Adicionado opção de menu persistente
* Removido o **TODO**
* Acrescentado o **[Melhorias futuras](README.md#melhorias-futuras)**

#### 07/07/19
* Código do programa final completo

#### 26/06/19
* Alteração nas funções envolvendo horários
* Adição de funções para deixar o código mais legível
* Implementando *Adiciona_Remedio*

#### 18/06/19
* Adição de comentários ao código fonte
* Formatação do código fonte
* Atualização do **TODO**

#### 17/06/19
* Configuração do *display LCD* 
* Implementação do teclado matricial
* Configurar horário
* Adicionar medicamento
