// C++ code
//
/**/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --- MAPEAMENTO DE PINOS ---
const int PIN_TMP36 = A0;        // Sensor de Temperatura TMP36
const int PIN_POT_RAD = A1;      // Potenciômetro de Radiação
const int PIN_LED_VERDE = 2;     // LED Status OK
const int PIN_LED_VERMELHO = 3;  // LED de Alerta Crítico
const int PIN_BUZZER = 4;        // Alerta Sonoro (Piezo)
const int PIN_SERVO = 9;         // Fio Laranja do Servo

// --- CONFIGURAÇÃO DO LCD (Endereço 32 / 0x20 para MCP23008) ---
LiquidCrystal_I2C lcd(32, 16, 2); 
Servo servoBlindagem;

// --- CONTROLE DE TEMPO (MILLIS) ---
unsigned long tempoAnteriorLeitura = 0;
unsigned long tempoAnteriorPisca = 0;
unsigned long tempoAnteriorBuzzer = 0;
const long intervaloLeitura = 1000; 
const long intervaloPisca = 300;     

// --- TELEMETRIA DE BORDO ---
float ultimaTemperaturaValida = 25.0;
int ultimaRadiacaoValida = 0;
bool estadoLedAlerta = LOW;
bool estadoBuzzer = LOW;

void setup() {
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  servoBlindagem.attach(PIN_SERVO);
  servoBlindagem.write(0); // Abre a blindagem original
  
  // Inicialização baseada na sua inspiração
  lcd.init();               
  lcd.backlight(); 
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("OME-TECH ORION");
  lcd.setCursor(0, 1);
  lcd.print("SISTEMA ONLINE");
  
  unsigned long start = millis();
  while(millis() - start < 2000); 
  lcd.clear();
}

void loop() {
  unsigned long tempoAtual = millis();

  // --- LEITURA E PROCESSAMENTO (A CADA 1 SEGUNDO) ---
  if (tempoAtual - tempoAnteriorLeitura >= intervaloLeitura) {
    tempoAnteriorLeitura = tempoAtual;

    // 1. Cálculo da Temperatura (A0)
    int leituraBrutaTemp = analogRead(PIN_TMP36);
    float voltagem = (leituraBrutaTemp * 5.0) / 1024.0;
    float temperaturaCalculada = (voltagem - 0.5) * 100; 

    if (temperaturaCalculada >= -50.0 && temperaturaCalculada <= 125.0) {
      ultimaTemperaturaValida = temperaturaCalculada;
    }

    // 2. Cálculo da Radiação (A1)
    int leituraBrutaRad = analogRead(PIN_POT_RAD);
    int radiacaoCalculada = map(leituraBrutaRad, 0, 1023, 0, 500);
    
    if (radiacaoCalculada >= 0 && radiacaoCalculada <= 500) {
      ultimaRadiacaoValida = radiacaoCalculada;
    }

    // Chama as funções de atuação e display
    atualizarDisplay();
    verificarSegurancaMissao();
  }

  // --- LÓGICA DE ALERTA (SINALIZAÇÃO E SOM) ---
  if (ultimaTemperaturaValida < 15.0 || ultimaTemperaturaValida > 45.0 || ultimaRadiacaoValida > 100) {
    digitalWrite(PIN_LED_VERDE, LOW); 
    
    if (tempoAtual - tempoAnteriorPisca >= intervaloPisca) {
      tempoAnteriorPisca = tempoAtual;
      estadoLedAlerta = !estadoLedAlerta;
      digitalWrite(PIN_LED_VERMELHO, estadoLedAlerta);
    }

    if (tempoAtual - tempoAnteriorBuzzer >= 500) { 
      tempoAnteriorBuzzer = tempoAtual;
      estadoBuzzer = !estadoBuzzer;
      if (estadoBuzzer) {
        int frequenciaAlerta = (ultimaRadiacaoValida > 100) ? 1500 : 800;
        tone(PIN_BUZZER, frequenciaAlerta);
      } else {
        noTone(PIN_BUZZER);
      }
    }
  } else {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_VERMELHO, LOW);
    noTone(PIN_BUZZER); 
  }
}

// ---ATUALIZAÇÃO DO DISPLAY---
void atualizarDisplay() {
  lcd.clear(); // Limpa a tela no início do ciclo
  
  // Linha 0: Mostra a temperatura usando a conversão String()
  lcd.setCursor(0, 0);
  lcd.print("TEMP: " + String(ultimaTemperaturaValida, 1) + " C");
  
  // Linha 1: Mostra a radiação usando a conversão String()
  lcd.setCursor(0, 1);
  lcd.print("RAD: " + String(ultimaRadiacaoValida) + " mSv/h");
}

// --- ATUADORES E FLAGS VISUAIS ---
void verificarSegurancaMissao() {
  if (ultimaRadiacaoValida > 100) {
    servoBlindagem.write(180); 
    lcd.setCursor(11, 0);
    lcd.print("[PRT]"); // Adiciona o status de protegido no fim da linha 0     
  } else {
    servoBlindagem.write(0);   
  }

  if (ultimaTemperaturaValida < 15.0) {
    lcd.setCursor(11, 1);
    lcd.print("[AQD]"); // Adiciona a flag de aquecedor no fim da linha 1      
  }
  
  if (ultimaTemperaturaValida > 45.0) {
    lcd.setCursor(11, 1);
    lcd.print("[RES]"); // Adiciona a flag de resfriamento no fim da linha 1      
  }
}
