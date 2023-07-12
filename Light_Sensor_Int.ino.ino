#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

#define MAX_COMMANDS_SIZE 64
bool stringSent = false; 
const uint8_t LDR_Pin = A0;
uint16_t LDR_Value = 0;
// pode ser 9 aqui 
uint8_t startButton = 9;
uint8_t  Speaker = 4;
const int SEQUENCE = 4;
int completion = 0;

RF24 radio(10, 9);  // CE,CSN               
RF24Network network(radio);      
const uint16_t this_node = 02;   
const uint16_t master00 = 00; 


int Final_Sequence[SEQUENCE];
int Current_Sequence[SEQUENCE];

bool INTERACTION_SOLVED, INTERACTION_RUNNING;

void setup() {

  Serial.begin(9600);
  radio.begin();
  network.begin(90, this_node);  
  pinMode(startButton, INPUT);
  randomSeed(analogRead(A1));

}

void loop() {
  network.update();    // (Address where the data is going)
  char incomingData[MAX_COMMANDS_SIZE] = "";

  // Receiving from master the command
  while ( network.available() ) {
    RF24NetworkHeader header;
    network.read(header, &incomingData, sizeof(incomingData));
    Serial.println(incomingData);
    processNetworkMessage(incomingData); 
    }

  if (!Serial) {
    Serial.begin(9600);
  }
  if (Serial.available()) {    // o que ele vai processar é a msg vinda na incoming data???
    processSerialMessage();
  }

  if (INTERACTION_SOLVED == false && INTERACTION_RUNNING == true) {
    gameLoop();
  }


  // Flag to track if the string has been sent
  if (INTERACTION_SOLVED == true && INTERACTION_RUNNING == false && !stringSent) {
    RF24NetworkHeader header(master00);
    const char text[] = "Light Sensor is Finished!";
    bool ok = network.write(header, text, sizeof(text)); // Send the string
    if (ok) {
      stringSent = true; // Update the flag indicating the string has been sent
    }
  }
  
  
}

void gameLoop() {
  int currentTone = 0;

  if (digitalRead(startButton) == HIGH) {
    delay(200);
    for (int i = 0; i < SEQUENCE; i++) {
      tone(Speaker, Final_Sequence[i]);
      delay(500);
      noTone(Speaker);
      delay(800);
    }
  }

  delay(800);

  LDR_Value = map(analogRead(LDR_Pin), 0, 50, 0, 4);
  
  currentTone = (LDR_Value + 2) * 100;

  if (LDR_Value > 0) {
    if (LDR_Value > 4) { //Setting a max limit of sounds frequency
      
      currentTone = 600;

      tone(Speaker, currentTone);
      delay(500);
      noTone(Speaker);
    }

    else {
      tone(Speaker, (LDR_Value + 2) * 100);
      delay(500);
      noTone(Speaker);
    }
  }

  

  if (currentTone != Final_Sequence[completion]) {
    for (int i = 0; i < SEQUENCE; i++) {
      Current_Sequence[i] = 0;
    }

    completion = 0;

    if (currentTone != Final_Sequence[0]) {
      return;
    }
  }

  Current_Sequence[completion] = currentTone;
  completion++;

  
  for (int i = 0; i < SEQUENCE; i++) {
    Serial.print(Current_Sequence[i]); 
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < SEQUENCE; i++) {
  Serial.print(Final_Sequence[i]); 
  Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < SEQUENCE; i++) {
    if (Current_Sequence[i] != Final_Sequence[i]) {
      return;
    }
  }

  delay(300);
  playWinSound();
  checkWinning();
  //Não resolvivel, arranjar solução, talvez com o morse
}

void processSerialMessage() {
  const uint8_t BUFF_SIZE = 64; // make it big enough to hold your longest command
  static char buffer[BUFF_SIZE + 1]; // +1 allows space for the null terminator
  static uint8_t length = 0; // number of characters currently in the buffer

  char c = Serial.read();
  if ((c == '\r') || (c == '\n')) {
    // end-of-line received
    if (length > 0) {
      tokenizeReceivedMessage(buffer);
    }
    length = 0;
  } else {
    if (length < BUFF_SIZE) {
      buffer[length++] = c; // append the received character to the array
      buffer[length] = 0; // append the null terminator
    }
  }
}

void processNetworkMessage(String msg) {
    tokenizeReceivedMessage(msg.c_str());
}

void tokenizeReceivedMessage(char *msg) {
  const uint8_t COMMAND_PAIRS = 10;
  char* tokenizedString[COMMAND_PAIRS + 1];
  uint8_t index = 0;

  char* command = strtok(msg, ";");
  while (command != 0) {
    char* separator = strchr(command, ':');
    if (separator != 0) {
      *separator = 0;
      tokenizedString[index++] = command;
      ++separator;
      tokenizedString[index++] = separator;
    }
    command = strtok(0, ";");
  }
  tokenizedString[index] = 0;

  processReceivedMessage(tokenizedString);
}

void processReceivedMessage(char** command) {
  if (strcmp(command[1], "START") == 0) {
    startSequence(command[3]);
  } else if (strcmp(command[1], "PAUSE") == 0) {
    pauseSequence(command[3]);
  } else if (strcmp(command[1], "STOP") == 0) {
    stopSequence(command[3]);
  } else if (strcmp(command[1], "INTERACTION_SOLVED_ACK") == 0) {
    setInteractionSolved();
  } else if (strcmp(command[1], "PING") == 0) {
    ping(command[3]);
  } else if (strcmp(command[1], "BAUD") == 0) {
    setBaudRate(atoi(command[3]), command[5]);
  } else if (strcmp(command[1], "SETUP") == 0) {
    Serial.println("COM:SETUP;INT_NAME:Light Sensor Interaction;BAUD:9600");
    Serial.flush();
  }
}

//TODO: Review This Method once Interaction Is Completed
void startSequence(char* TIMESTAMP) {
  INTERACTION_SOLVED = false;
  INTERACTION_RUNNING = true;

  generateFinalSequence();


  Serial.print("COM:START_ACK;MSG:Interaction Started Successfully");
  Serial.print(";ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void pauseSequence(char* TIMESTAMP) {
  INTERACTION_RUNNING = !INTERACTION_RUNNING;
  if (INTERACTION_RUNNING) {
    Serial.print("COM:PAUSE_ACK;MSG:Device is now running;ID:");
  } else {
    Serial.print("COM:PAUSE_ACK;MSG:Device is now paused;ID:");
  }
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void stopSequence(char* TIMESTAMP) {
  INTERACTION_RUNNING = false;
  Serial.print("COM:STOP_ACK;MSG:Device is now stopped;ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void setInteractionSolved() {
  INTERACTION_SOLVED = true;
  INTERACTION_RUNNING = false;
}

void ping(char* TIMESTAMP) {
  Serial.print("COM:PING;MSG:PING;ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void setBaudRate(int baudRate, char* TIMESTAMP) {
  Serial.flush();
  Serial.begin(baudRate);
  Serial.print("COM:BAUD_ACK;MSG:The Baud Rate was set to ");
  Serial.print(baudRate);
  Serial.print(";ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

bool checkWinning() {
  Serial.println("COM:INTERACTION_SOLVED;MSG:User Iluminated LDR Sensor;PNT:1750");
  Serial.flush();
  INTERACTION_SOLVED = true;
}

void playWinSound() {
  tone(Speaker, 784);
  delay(300); 
  noTone(Speaker);
  delay(150);
  tone(Speaker, 784);
  delay(200);
  tone(Speaker, 988);
  delay(300);
  tone(Speaker, 1175);
  delay(300);
  noTone(Speaker);
}

void generateFinalSequence() {
  for (int i = 0; i < SEQUENCE; i++) {
    Final_Sequence[i] = random(3, 7) * 100;
    Serial.print(Final_Sequence[i]);
    Serial.print(", ");
  }
  Serial.println();
}