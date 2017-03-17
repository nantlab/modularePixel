
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include <OSCBundle.h>
#include <OSCBoards.h>

#include "config.h"

int pins[HEIGHT * WIDTH];
int extraPins[8];

EthernetUDP Udp;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 3);

// ### helpers
int getIndex(int column, int row) {
  return row * WIDTH + column;
}

// ### setters
void setPixel(int column, int row, int value) {
  digitalWrite(pins[getIndex(column, row)], value);
  Serial.println("set pixel");
}
void setRow(int row, int value) {
  for (int column = 0; column < WIDTH; column++) {
    setPixel(column, row, value);
  }
}
void setColumn(int column, int value) {
  for (int row = 0; row < HEIGHT; row++) {
    setPixel(column, row, value);
  }
}
void setAll(int value) {
  for (int row = 0; row < HEIGHT; row++) {
    for (int column = 0; column < WIDTH; column++) {
      setPixel(column, row, value);
    }
  }
}

// ### router callbacks
void routePixel(OSCMessage &msg, int addrOffset ) {
  int column = msg.getInt(0);
  int row = msg.getInt(1);
  int value = msg.getInt(2);
  setPixel(column, row, value);
}
void routeRow(OSCMessage &msg, int addrOffset ) {
  int row = msg.getInt(0);
  int value = msg.getInt(1);
  setRow(row, value);
}
void routeColumn(OSCMessage &msg, int addrOffset ) {
  int column = msg.getInt(0);
  int value = msg.getInt(1);
  setColumn(column, value);
}

void routeAll(OSCMessage &msg, int addrOffset ) {
  int value = msg.getInt(0);
  setAll(value);
}

void setup() {
  //setup serial
  Serial.begin(115200);

  //setup pins
  for (int i = 0; i < 8; i++) {
    pins[i] = i + 2;
  }
  for (int i = 0; i < 8; i++) {
    pins[8 + i] = 38 + i * 2;
  }
  for (int i = 0; i < 8; i++) {
    pins[16 + i] = 39 + i * 2;
  }
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    pinMode(pins[i], OUTPUT);
  }

  for (int i = 0; i < 8; i++) {
    extraPins[i] = 23 + i + 2;
  }
  for (int i = 0; i < 8; i++) {
    pinMode(extraPins[i], OUTPUT);
  }

  //setup ethernet
  Ethernet.begin(mac, ip);
  Udp.begin(PORT);


  Serial.println("successfully setup arduino OSC server");
}

void loop() {
  for (int i = 0; i < 24; i++) {
    delay(1000);
    digitalWrite(pins[i], 1);
  }
  for (int i = 0; i < 8; i++) {
    delay(1000);
    digitalWrite(extraPins[i], 1);
  }
  OSCBundle bundleIN;
  int size;

  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      bundleIN.fill(Udp.read());

    if (!bundleIN.hasError()) {
      Serial.println("got bundle");
      bundleIN.route("/setPixel", routePixel);
      bundleIN.route("/setRow", routeRow);
      bundleIN.route("/setColumn", routeColumn);
      bundleIN.route("/setAll", routeAll);
    } else {
      Serial.println("got error");

    }
  }
}


