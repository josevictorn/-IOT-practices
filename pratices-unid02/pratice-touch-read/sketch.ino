int ledPinBlue = 12;
int ledPinGreen = 13;
int ledPinRed = 14;
int touchPinStart = 4; // Sensor de início do jogo
int touchPinPlayer1 = 15; // Sensor de toque do jogador 1
int touchPinPlayer2 = 2; // Sensor de toque do jogador 2

bool gameStarted = false;
unsigned long startTime;
unsigned long player1Time;
unsigned long player2Time;

int scorePlayer1 = 0;
int scorePlayer2 = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(touchPinStart, INPUT);
  pinMode(touchPinPlayer1, INPUT);
  pinMode(touchPinPlayer2, INPUT);
  Serial.println("Hello, ESP32!");
  Serial.println("ESP32 touch teste!");
}

void loop() {
  if (!gameStarted) {
    Serial.print("Touch Start: ");
    int startTouch = touchRead(touchPinStart);
    //int startTouch = digitalRead(touchPinStart); //simula o codigo do touch acima

    Serial.println(startTouch);
    if (startTouch > 40) { // Se o sensor de início for ativado
      startGame();
    }
  } else {
    //Serial.print("Touch Player 1: ");
    int player1Touch = touchRead(touchPinPlayer1);
    //int player1Touch = digitalRead(touchPinPlayer1);//simula o codigo do touch acima

    //Serial.println(player1Touch);
    if (player1Touch > 40) { // Se o sensor do jogador 1 for ativado
      endGame(1);
    }
    
    //Serial.print("Touch Player 2: ");
    int player2Touch = touchRead(touchPinPlayer2);
    //int player2Touch = digitalRead(touchPinPlayer2);//simula o codigo do touch acima

    //Serial.println(player2Touch);
    if (player2Touch > 40) { // Se o sensor do jogador 2 for ativado
      endGame(2);
    }
  }
}

void startGame() {
  Serial.println("Game started!");
  gameStarted = true;
  for(int i=0; i<3; i++){
    digitalWrite(ledPinBlue, HIGH);
    digitalWrite(ledPinGreen, HIGH);
    digitalWrite(ledPinRed, HIGH);
    delay(500);
    digitalWrite(ledPinBlue, LOW);
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinRed, LOW);
    delay(500);
}
  digitalWrite(ledPinRed, HIGH);
  startTime = millis(); // Inicia a contagem do tempo
}

void endGame(int winner) {
  if (winner == 1) {
    player1Time = millis() - startTime;
    Serial.print("Player 1 wins! Time: ");
    Serial.print(player1Time);
    Serial.println("ms");
    digitalWrite(ledPinGreen, HIGH); // LED do jogador 1 acende
    scorePlayer1++;
    Serial.print("Score Player 1: ");
    Serial.println(scorePlayer1);
  } else if (winner == 2) {
    player2Time = millis() - startTime;
    Serial.print("Player 2 wins! Time: ");
    Serial.print(player2Time);
    Serial.println("ms");
    digitalWrite(ledPinBlue, HIGH); // LED do jogador 2 acende
    scorePlayer2++;
    Serial.print("Score Player 2: ");
    Serial.println(scorePlayer2);
  }
  Serial.print("Placar: jogador1 ");
  Serial.print(scorePlayer1);
  Serial.print(" X ");
  Serial.print(scorePlayer2);
  Serial.println(" jogador2");
  delay(5000); // Mantém a tela por 5 segundos
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinRed, LOW);
  gameStarted = false; // Reinicia o jogo
}
