#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <NTPClient.h>

#include <time.h>


#define LED_PIN 22
#define TOUCH_PIN 4
#define LED_STATE_FILE "/led_state.txt"

bool ledState = false;
bool buttonState = false;

const char *ssid = "NPITI-IoT";
const char *password = "NPITI-IoT";

WiFiUDP udp;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);
String hora;
String date;

String s;
String msg;

void writeFile(String state, String path) { //escreve conteúdo em um arquivo
  File rFile = SPIFFS.open(path, "a");//a para anexar
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    rFile.print(state);
    rFile.print(" - ");
    rFile.print(date);
    rFile.print(" às ");
    rFile.print(hora);
    rFile.println("\n");
  }
  rFile.close();
}

boolean verifyLastCharacter() {
  int penultimo =-1;
  int ultimo =-1;
  char primeiroCaractere;

  for(int i=0; i<msg.length(); i++){
    if(msg[i] == '\n'){
      penultimo = ultimo;
      ultimo=i;
    }
  }
  if(penultimo!=1){
    primeiroCaractere = msg[penultimo+1];
  }
  else{
    primeiroCaractere = msg[0];
  }  

  msg ="";

  if(primeiroCaractere == '1'){
    return true;
  }
  else{
    return false;
  }
}

boolean readFile(String path) {
  Serial.println("Read file");
  File rFile = SPIFFS.open(path, "r");//r+ leitura e escrita
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("----------Lendo arquivo ");
    Serial.print(path);
    Serial.print(rFile.size());

    while(rFile.position() < rFile.size()) {    
      s = rFile.readStringUntil('\n');
      s.trim();

      msg += s;
      msg += "\n";
    }

    int size = msg.length();
    msg = msg.substring(0, size - 1);

    Serial.print(msg);
    
    rFile.close();
    boolean aux = verifyLastCharacter();

    return aux;
  }
}

void formatFile() {
  Serial.println("Formantando SPIFFS");
  SPIFFS.format();
  Serial.println("Formatou SPIFFS");
}

void openFS(void) {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

String convertSecondsToDate(unsigned long segundos) {
  // Calcula o número de dias desde 1º de janeiro de 1970
  unsigned long dias = segundos / 86400; // 86400 segundos em um dia
  
  // Define a data de referência (1º de janeiro de 1970)
  int anoBase = 1970;
  int mesBase = 1;
  int diaBase = 1;
  
  // Calcula o ano, mês e dia atual
  int anoAtual = anoBase;
  int mesAtual = mesBase;
  int diaAtual = diaBase + dias;
  
  // Ajusta o ano e o mês
  while (diaAtual > 31) {
    if ((mesAtual == 4 || mesAtual == 6 || mesAtual == 9 || mesAtual == 11) && diaAtual > 30) {
      diaAtual -= 30;
      mesAtual++;
    } else if (mesAtual == 2) {
      // Verifica se é um ano bissexto
      if ((anoAtual % 4 == 0 && anoAtual % 100 != 0) || (anoAtual % 400 == 0)) {
        if (diaAtual > 29) {
          diaAtual -= 29;
          mesAtual++;
        }
      } else {
        if (diaAtual > 28) {
          diaAtual -= 28;
          mesAtual++;
        }
      }
    } else {
      diaAtual -= 31;
      mesAtual++;
    }
    
    if (mesAtual > 12) {
      mesAtual = 1;
      anoAtual++;
    }
  }
  
  // Formata a data
  String dataFormatada = String(diaAtual) + "/" + String(mesAtual) + "/" + String(anoAtual);
  return dataFormatada;
}


void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  
  Serial.println("Iniciado...");
  delay(200);
  // formatFile();

  /*---- Tenta conectar ao Wi-Fi e a senha ---- */
  WiFi.begin(ssid, password);
  /*---- Enquanto estiver conectando ---- */
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);                                 /* Aguarda meio segundo */
    Serial.println("Conectando ao WiFi..");     /* Conectando */
  }
  Serial.println("Conectado"); /* Conectado */

  ntp.begin();               /* Inicia o protocolo */
  ntp.forceUpdate();    /* Atualização */

  openFS(); 
  Serial.println("Lendo arquivo...");
  ledState = readFile(LED_STATE_FILE);

  if(ledState) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void loop() {
  int buttonTouch = touchRead(TOUCH_PIN);
  hora = ntp.getFormattedTime();
  date = convertSecondsToDate(ntp.getEpochTime());

  if(buttonTouch < 15) {
    buttonState = !buttonState;
  
    // Alterna o estado do LED
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    
    // Salva o estado atual do LED no arquivo
    if(ledState) {
      writeFile("1", LED_STATE_FILE);
    } else {
      writeFile("0", LED_STATE_FILE);
    }
  }

  delay(100); // Pequeno delay para evitar múltiplas leituras do botão
}
