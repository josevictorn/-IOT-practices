// RST  = 9


//Inclui as bibliotecas do SPI e do RC522
#include <SPI.h>
#include <MFRC522.h>

//Definição dos pinos de conexão do projeto
#define PINO_SS 10
#define PINO_RST 9
#define LED_VERMELHO_PIN 7
#define LED_VERDE_PIN 6
#define BUZZER_PIN 5

int tentativas = 0;
bool sistemaBloqueado = false;

//Cria o item para configurar o módulo RC522
MFRC522 mfrc522(PINO_SS, PINO_RST);


void setup() {
  Serial.begin(9600); // Inicializa a serial
  SPI.begin();// Inicializa a comunicação SPI
  mfrc522.PCD_Init(); // Inicializa o módulo MFRC522
  pinMode(LED_VERMELHO_PIN, OUTPUT);
  pinMode(LED_VERDE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  digitalWrite(LED_VERMELHO_PIN, HIGH);
  Serial.println("Sistema Inicializado: Aproxime o token");
}

void loop() {
  if (!sistemaBloqueado){
    if (!mfrc522.PICC_IsNewCardPresent()) return;// Aguarda a aproximação do token
    if (!mfrc522.PICC_ReadCardSerial()) return; // Seleciona qual token vai ser utilizado
    Serial.print("UID da tag:"); // Mostra UID do token na serial
    String conteudo= ""; //Cria uma variável vazia, do tipo string
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if(mfrc522.uid.uidByte[i] < 0x10){
        Serial.print(" 0");
      }
      else{
        Serial.print(" ");
     }
      Serial.print(mfrc522.uid.uidByte[i], HEX);// Printa a mensagem convertida em hexadecimal
      if(mfrc522.uid.uidByte[i] < 0x10){
        conteudo.concat(String(" 0"));
      }
      else{
        conteudo.concat(String(" "));
      }
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    conteudo.toUpperCase();//Coloca todas as letras da string em maiúscula
    if (conteudo.substring(1) == "60 85 73 A5") {
      acessoLiberado();
    }
    else {
      acessoNegado();
    }
  }
  else {    
    digitalWrite(BUZZER_PIN, HIGH);// Liga o buzzer po 30 segundos
    for (int i = 0; i < 6; i++) {
      digitalWrite(LED_VERMELHO_PIN, !digitalRead(LED_VERMELHO_PIN));
      delay(500);   
    }
    digitalWrite(BUZZER_PIN, LOW);// Desliga o buzzer   
  }
}

void acessoLiberado() {
  Serial.println("Acesso liberado");
  digitalWrite(LED_VERMELHO_PIN, LOW);
  digitalWrite(LED_VERDE_PIN, HIGH);
  delay(10000); // 10 segundos
  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_VERMELHO_PIN, HIGH);
}

void acessoNegado() {
  Serial.println("Cartão inválido");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_VERMELHO_PIN, LOW);
    delay(500);
    digitalWrite(LED_VERMELHO_PIN, HIGH);
    delay(500);
  }
  tentativas++;
  if (tentativas >= 5) {
    Serial.println("SISTEMA BLOQUEADO");
    sistemaBloqueado = true;
  }
}