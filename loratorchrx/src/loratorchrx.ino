// loratorch.ino
// -*- mode: C++ -*-
// Lora radio controlled torch

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

RH_RF95 driver(D8, D2); //
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  // setup torch lamp control pin 1=lit 0=unlit
  pinMode(D0, OUTPUT);
  digitalWrite(D0, 1);

  // setup lamp on sense input pin
  pinMode(D1, INPUT);

  // setup battery voltage sense pin
  pinMode(A0, INPUT);

  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to be available
  if (!manager.init()) {
    Serial.println("init failed");
  }
  Serial.println("init ok");

  driver.setFrequency(868.0); // Swedish lora frequency
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
int prevOnSense = 0;

void loop() {
  if (manager.available()) {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)) {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      buf[len] = 0;
      Serial.println((char*)buf);
      if (strcmp((char *)buf, "on") == 0) {
        digitalWrite(D0, 1);
      } else if (strcmp((char *)buf, "off") == 0) {
        digitalWrite(D0, 0);
      }
    }
  }

  int onSense = digitalRead(D1);
  if (prevOnSense != onSense) {
    Serial.printf("on sense %d\n", onSense);
    prevOnSense = onSense;
  }
}
