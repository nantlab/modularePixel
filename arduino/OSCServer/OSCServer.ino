#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include <OSCBundle.h>
#include <OSCBoards.h>

#include "config.h"
enum MODE {
  STANDALONE,
  OSCSERVER
};

unsigned long lastTime = 0;

int pins[HEIGHT * WIDTH];

MODE mode = OSCSERVER;
//MODE mode = STANDALONE;

EthernetUDP Udp;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 3);

// ### helpers
int getIndex(int column, int row) {
  return row * WIDTH + column;
}

// ### setters
void setPixel(int column, int row, int value) {
  digitalWrite(pins[getIndex(column, row)], (value + 1) % 2);
  Serial.println("set pixel " + String(getIndex(column, row)) + ": " + String(column) + " " + String(row));
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
void routeMode(OSCMessage &msg, int addrOffset) {
  mode = (MODE) msg.getInt(0);
}
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

void routeBlob(OSCMessage &msg, int addrOffset ) {
  uint8_t blob[WIDTH * HEIGHT];
  msg.getBlob(0, blob, WIDTH * HEIGHT);
  for (int row = 0; row < HEIGHT; row++) {
    for (int column = 0; column < WIDTH; column++) {
      int index = row * WIDTH + column;
      setPixel(column, row, blob[index]);
    }
  }
}


void setup() {
  //setup serial
  Serial.begin(115200);

  //setup pins
  for (int i = 0; i < 8; i++) {
    pins[i] = i + 2;
  }
  for (int i = 0; i < 8; i++) {
    pins[8 + i] = 22 + i * 2;
  }
  for (int i = 0; i < 8; i++) {
    pins[16 + i] = 23 + i * 2;
  }
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    pinMode(pins[i], OUTPUT);
  }

  //setup ethernet
  Ethernet.begin(mac, ip);
  Udp.begin(PORT);


  Serial.println("successfully setup arduino OSC server");
  for (int i = 0; i < 24; i++) {
    digitalWrite(pins[i], 1);
  }
}

void loop() {
//  for (int row = 0; row < HEIGHT; row++) {
//    for (int column = 0; column < WIDTH; column++) {
//      setPixel(column, row, 0);
//    }
//  }
//  for (int row = 0; row < HEIGHT; row++) {
//    for (int column = 0; column < WIDTH; column++) {
//      setPixel(column, row, 1);
//      delay(1000);
//    }
//  }
  OSCBundle bundleIN;
  int size;
  if ( (size = Udp.parsePacket()) > 0) {
    while (size--)
      bundleIN.fill(Udp.read());
  }

  switch (mode) {
    case STANDALONE:
      {
        long currentTime = millis();
        if (currentTime - lastTime > 2 * 1000) {
          setAll(0);
          int maxPixel = random(10);
          for (int i = 0; i < maxPixel; i++) {
            int column = random(WIDTH);
            int row = random(HEIGHT);
            setPixel(column, row, 1);
          }
          if (!bundleIN.hasError()) {
            bundleIN.route("/setMode", routeMode);
          } else {
            Serial.println("got error");
          }
          lastTime = currentTime;
        }
        break;
      }

    case OSCSERVER:
      {
        if (!bundleIN.hasError()) {
          bundleIN.route("/set", routeBlob);
          bundleIN.route("/setPixel", routePixel);
          bundleIN.route("/setRow", routeRow);
          bundleIN.route("/setColumn", routeColumn);
          bundleIN.route("/setAll", routeAll);
        } else {
          Serial.println("got error");
        }
        break;
      }
  }




}


