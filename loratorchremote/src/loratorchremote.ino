// rf95_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_server
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with the RFM95W
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Button.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_RF95 driver(D8, D2); //RH_RF95 driver(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

Button onButton(D3, true, true, 20);
Button offButton(D1, true, true, 20);

void setup() {
  WiFi.forceSleepBegin();

  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  Serial.println("serial ok");

  if (manager.init()) {
    Serial.println("trx init ok");
  } else {
    Serial.println("trx init failed");
  }
  driver.setFrequency(868.0);
}

void sendData(const char *data) {
  if (manager.sendtoWait((uint8_t *)data, strlen(data), SERVER_ADDRESS)) {
  } else {
    Serial.println("sendtoWait failed");
  }
}

void loop() {
  onButton.read();
  offButton.read();

  if (onButton.wasPressed()) {
    sendData("on");
  }

  if (offButton.wasPressed()) {
    sendData("off");
  }
}
