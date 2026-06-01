# OME-Tech Orion — Sistema de Monitoramento de Missão Crítica

Documentação oficial do firmware de controle embarcado do **Robô Orion**, desenvolvido para operação autônoma e telemetria de longo alcance em ambientes espaciais e superfícies planetárias hostis.

##  Apresentação do Projeto

O projeto **Orion** resolve o problema de segurança operacional em garras e módulos robóticos remotos expostos a variações térmicas extremas e picos de radiação ionizante sem a necessidade de supervisão humana constante. 

Através de uma arquitetura *Fail-Safe* baseada em multitarefa assíncrona temporizada (`millis()`), o sistema monitora variáveis ambientais em tempo real. Caso as leituras atinjam limites críticos, o firmware toma decisões autônomas instantâneas: ativa escudos mecânicos de chumbo (via servomotor), aciona subsistemas térmicos de mitigação e emite alertas visuais e sonoros codificados por frequência.

---

##  Hardware e Mapeamento de Pinos

Para reprodução do circuito físico ou simulação no **Tinkercad**, utilize a seguinte pinagem baseada na placa microcontroladora (compatível com Arduino Uno):

| Componente | Tipo de Sinal | Pino Arduino | Descrição Técnica |
| :--- | :--- | :--- | :--- |
| **Sensor TMP36** | Entrada Analógica | `A0` | Monitoramento térmico do núcleo operacional |
| **Potenciômetro** | Entrada Analógica | `A1` | Simulador de captação de radiação gama/ionizante |
| **LED Verde** | Saída Digital | `2` | Sinalizador de telemetria nominal (Status OK) |
| **LED Vermelho** | Saída Digital | `3` | Sinalizador de emergência/perigo (Pisca assíncrono) |
| **Buzzer Piezo** | Saída Digital | `4` | Alarme sonoro com modulação de frequência |
| **Servomotor** | Saída PWM | `9` | Atuador físico da blindagem de proteção [PRT] |
| **Display LCD 16x2** | Barramento I2C | `SDA (A4) / SCL (A5)` | Painel local. Endereço `0x20` via chip MCP23008 |

---

## Guia de Instalação e Configuração

### Pré-requisitos
Para compilar e carregar o firmware, você precisará de:
* **Arduino IDE** (Versão 2.0 ou superior) ou conta ativa no **Tinkercad**.
* Cabo de dados USB tipo A/B (para hardware físico).

### Dependências de Bibliotecas
O firmware utiliza comunicação I2C customizada para drivers baseados no chip **MCP23008**. Garanta que possui as seguintes bibliotecas instaladas no gerenciador da IDE:
1. `Wire.h` (Nativa do ecossistema Arduino)
2. `Servo.h` (Nativa para controle de servo-instâncias)
3. `LiquidCrystal_I2C.h` (Certifique-se de que a biblioteca instalada oferece suporte para o construtor de pinagem decimal do endereço `32` / `0x20`).

### Passo a Passo para Execução
1. Faça o download ou clone este repositório na sua máquina.
2. Abra o arquivo do firmware (extensão `.ino` ou `.cpp`) na **Arduino IDE**.
3. Vá em **Ferramentas > Placa** e selecione **Arduino Uno**.
4. Conecte a placa na porta USB do computador e selecione a porta COM correspondente em **Ferramentas > Porta**.
5. Clique no botão **Carregar (Upload)** para compilar e gravar o código no microcontrolador.

---

##  Como Utilizar e Interagir com o Sistema

Assim que o sistema for iniciado, a tela do LCD exibirá a mensagem de inicialização `OME-TECH ORION / SISTEMA ONLINE` por 2 segundos antes de liberar o painel de telemetria.

### Estados de Operação Prática

Para testar as respostas automatizadas do robô através dos sensores e potenciômetros, utilize os seguintes parâmetros:

####  Cenário 1: Operação Nominal (Segura)
* **Como simular:** Mantenha o sensor TMP36 entre $15^\circ\text{C}$ e $45^\circ\text{C}$ e o potenciômetro de radiação abaixo de 100 mSv/h.
* **Comportamento esperado:** O LED Verde permanecerá aceso, o LED Vermelho e o Buzzer ficarão desligados e o servo se manterá em $0^\circ$ (Blindagem aberta).

####  Cenário 2: Emergência Radiológica Crítica
* **Como simular:** Gire o potenciômetro de radiação (A1) para um valor **acima de 100 mSv/h**.
* **Comportamento esperado:** * O LED Verde apaga e o LED Vermelho começa a piscar rapidamente (a cada 300ms).
  * O Buzzer emite um alarme sonoro intermitente de **alta frequência (1500Hz)**.
  * O Servomotor gira instantaneamente para **$180^\circ$** e a flag **`[PRT]`** (Protegido) aparece na Linha 0 do LCD.

####  Cenário 3: Emergência Térmica (Frio ou Calor Extremo)
* **Como simular:** Altere o sensor TMP36 para menos de $15^\circ\text{C}$ ou mais de $45^\circ\text{C}$.
* **Comportamento esperado:** * O LED Vermelho pisca e o LED Verde se apaga.
  * O Buzzer emite um tom de alerta mais **grave (800Hz)**, indicando risco térmico.
  * Se a temperatura estiver abaixo de $15^\circ\text{C}$, a flag **`[AQD]`** (Aquecedor Ativo) surge na Linha 1 do LCD.
  * Se a temperatura superar $45^\circ\text{C}$, a flag **`[RES]`** (Resfriamento Ativo) surge na Linha 1 do LCD.

---

Desenvolvido por **Arthur Bergami Lucas e Davi Martins Herculano** (RM: 570679) (RM:572699). 
