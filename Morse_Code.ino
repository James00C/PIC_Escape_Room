#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

#define MAX_COMMANDS_SIZE 64
char stringToMorseCode[9][32] = {"MORSE\0", "RADIOTELEPHONE\0", "TELEGRAPH\0", "TELEGRAPHIST\0", "SAVE OUR SOULS\0", "TRANSMISSION\0", "SAMUEL MORSE\0", "COMPUTATION MUSEUM\0", "CODIFICATION\0"};

bool stringSent = false;
RF24 radio(A0,A1);  // CE,CSN               
RF24Network network(radio);      
const uint16_t this_node = 03;   
const uint16_t master00 = 00; 

// DOT LEDs
uint8_t ledDOT1 = 2;      // blink an led on output 2
uint8_t ledDOT2 = 3;      // blink an led on output 3
uint8_t ledDOT3 = 4;      // blink an led on output 4
uint8_t ledDOT4 = 5;      // blink an led on output 5

//DASH LEDs
uint8_t ledDASH1 = 6;     // blink an led on output 6
uint8_t ledDASH2 = 7;     // blink an led on output 7
uint8_t ledDASH3 = 8;     // blink an led on output 8

//START TRANSLATION TO MORSE BUTTON
uint8_t startButton = 9;  // Replay Morse Message

//PIEZZO SPEAKER
uint8_t audio8 = 10;       // output audio on pin 10
uint16_t note = 2300;      // music note/pitch

uint8_t dotLen = 125;     // length of the morse code 'dot'
uint16_t dashLen = dotLen * 3;    // length of the morse code 'dash'
uint8_t elemPause = dotLen * 5;  // length of the pause between elements of a character
uint16_t Spaces = dotLen * 7;     // length of the spaces between characters
uint16_t wordPause = dotLen * 10;  // length of the pause between words

char selectedWord[32];

bool INTERACTION_SOLVED, INTERACTION_RUNNING;

void setup() {
  Serial.begin(9600);
  radio.begin();
  network.begin(90, this_node);
  randomSeed(analogRead(A2));
  randomSeed(analogRead(A3));
  randomSeed(analogRead(A4));
  randomSeed(analogRead(A5));
  randomSeed(analogRead(A6));
  randomSeed(analogRead(A7));
  randomSeed(millis());
  pinMode(ledDOT1, OUTPUT);
  pinMode(ledDOT2, OUTPUT);
  pinMode(ledDOT3, OUTPUT);
  pinMode(ledDOT4, OUTPUT);
  pinMode(ledDASH1, OUTPUT);
  pinMode(ledDASH2, OUTPUT);
  pinMode(ledDASH3, OUTPUT);
  pinMode(startButton, INPUT);
}

void loop() {
  network.update();    // (Address where the data is going)
  char incomingData[MAX_COMMANDS_SIZE] = "";

  while ( network.available() ) {
    RF24NetworkHeader header;
    Serial.println("Cheguei aqui");
    network.read(header, &incomingData, sizeof(incomingData));
    Serial.println(incomingData);
    processNetworkMessage(incomingData); 
    }
  if (!Serial) {
    Serial.begin(9600);
  }
  if (Serial.available()) {
    processSerialMessage();
  }
  if (INTERACTION_SOLVED == false && INTERACTION_RUNNING == true) {
    RF24NetworkHeader header(master00);
    const char text[] = "Morse Code is Communicating!";
    bool ok = network.write(header, text, sizeof(text));
    gameloop();

  } 
  if (INTERACTION_SOLVED == true && INTERACTION_RUNNING == false && !stringSent) {
    RF24NetworkHeader header(master00);
    const char text[] = "Morse Code is Finished!";
    bool ok = network.write(header, text, sizeof(text)); // Send the string
    if (ok) {
      stringSent = true; // Update the flag indicating the string has been sent
    }
  }
}

void gameloop() {
  if (digitalRead(startButton) == HIGH) {
    for (int i = 0; i < sizeof(selectedWord) - 1; i++)
    {
      Serial.println(selectedWord[i]);
      GetChar(toLowerCase(selectedWord[i]));
    }
  }
}

// DOT
void MorseDot()
{
  digitalWrite(ledDOT1, HIGH);
  digitalWrite(ledDOT2, HIGH);
  digitalWrite(ledDOT3, HIGH);
  digitalWrite(ledDOT4, HIGH);
  tone(audio8, note, dotLen); // start playing a tone
  delay(dotLen);              // hold in this position
}

// DASH
void MorseDash()
{
  digitalWrite(ledDASH1, HIGH);
  digitalWrite(ledDASH2, HIGH);
  digitalWrite(ledDASH3, HIGH);
  tone(audio8, note, dashLen);  // start playing a tone
  delay(dashLen);               // hold in this position
}

// Turn Off
void LightsOff(int delayTime)
{
  digitalWrite(ledDOT1, LOW);
  digitalWrite(ledDOT2, LOW);
  digitalWrite(ledDOT3, LOW);
  digitalWrite(ledDOT4, LOW);
  digitalWrite(ledDASH1, LOW);
  digitalWrite(ledDASH2, LOW);
  digitalWrite(ledDASH3, LOW);
  noTone(audio8);             // stop playing a tone
  delay(delayTime);             // hold in this position
}

// *** Characters to Morse Code Conversion *** //
void GetChar(char tmpChar)
{
  // Take the passed character and use a switch case to find the morse code for that character
  switch (tmpChar) {
    case 'a':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'b':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'c':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'd':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'e':
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'f':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'g':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'h':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'i':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'j':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'k':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'l':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'm':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'n':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'o':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'p':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 'q':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'r':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 's':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case 't':
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'u':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'v':
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'w':
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'x':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'y':
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      break;
    case 'z':
      MorseDash();
      LightsOff(elemPause);
      MorseDash();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      MorseDot();
      LightsOff(elemPause);
      break;
    case ' ':
      LightsOff(wordPause);
      break;

    default:
      // In case the character is null (which happens when the words don't add up to 32 chars)
      return;
  }
  LightsOff(Spaces);
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
  } else if (strcmp(command[1], "INTERACTION_SOLVED") == 0) {
    checkWinning();
  } else if (strcmp(command[1], "INTERACTION_SOLVED_ACK") == 0) {
    setInteractionSolved();
  } else if (strcmp(command[1], "PING") == 0) {
    ping(command[3]);
  } else if (strcmp(command[1], "BAUD") == 0) {
    setBaudRate(atoi(command[3]), command[5]);
  } else if (strcmp(command[1], "SETUP") == 0) {
    Serial.println("COM:SETUP;INT_NAME:Morse Code;BAUD:9600");
    Serial.flush();
  }
}

void startSequence(char* TIMESTAMP) {
  INTERACTION_SOLVED = false;
  INTERACTION_RUNNING = true;

  for (uint8_t i = 0; i < sizeof(selectedWord); i++) {
    selectedWord[i] = 0;
  }
  randomSeed(millis());
  strcpy(selectedWord, stringToMorseCode[random(0, 9)]);

  Serial.print("COM:START_ACK;MSG:");
  Serial.print(selectedWord);
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
  INTERACTION_SOLVED = true;
  Serial.println("COM:INTERACTION_SOLVED;MSG:User Guessed Message Correctly;PNT:2000");
  Serial.flush();
}