// loratorch.ino
// -*- mode: C++ -*-
// Lora radio controlled torch

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Button.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define LAMP_SENSE_PIN D1

RH_RF95 driver(D8, D2); //
RHReliableDatagram manager(driver, SERVER_ADDRESS);
Button lampOnButton(LAMP_SENSE_PIN, false, false, 50);

void setup() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  // setup torch lamp control pin 1=lit 0=unlit
  pinMode(D4, OUTPUT);
  digitalWrite(D4, 0);

  // setup lamp on sense input pin
//  pinMode(LAMP_SENSE_PIN, INPUT);

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
float prevVoltage = 0;

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
        digitalWrite(D4, 0);
      } else if (strcmp((char *)buf, "off") == 0) {
        digitalWrite(D4, 1);
      }
    }
  }

  // detect lamp on button (only valid when lamp is remote controlled off - that is D4 set to 1)
  lampOnButton.read();
  if (lampOnButton.wasPressed()) {
    Serial.println("turn on");
  } else if (lampOnButton.wasReleased()) {
    Serial.println("turn off");
  }

  // measure battery voltage
  float voltage = analogRead(A0) * 6.02 / 789.0;
  if (abs(voltage - prevVoltage) > 0.05) {
    Serial.print(voltage);
    Serial.println("V");
    prevVoltage = voltage;
  }
}
