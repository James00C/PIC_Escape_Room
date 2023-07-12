#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;
const uint16_t node02 = 02;
const uint16_t node03 = 03;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
}

void loop() {
  // Sending
  network.update();
  String text;

  while (Serial.available() > 0) {
    String text_send = Serial.readString();

    RF24NetworkHeader header1(node01);     // Address where the data is going
    bool ok1 = network.write(header1, text_send.c_str(), text_send.length() + 1);
    Serial.println(text_send);

    RF24NetworkHeader header2(node02);
    bool ok2 = network.write(header2, text_send.c_str(), text_send.length() + 1);

    RF24NetworkHeader header3(node03);
    bool ok3 = network.write(header3, text_send.c_str(), text_send.length() + 1);

  }

  while (network.available()) {  // Is there any incoming data?
    RF24NetworkHeader header;
    char text[32] = "";
    network.read(header, &text, sizeof(text));

    // Check the header and process responses accordingly
    if (header.from_node == node01) {
      // Response from node01
      Serial.print("Response from node01: ");
      Serial.println(text);
    } else if (header.from_node == node02) {
      // Response from node02
      Serial.print("Response from node02: ");
      Serial.println(text);
    } else if (header.from_node == node03) {
      // Response from node03
      Serial.print("Response from node03: ");
      Serial.println(text);
    }
  }
}
 
  