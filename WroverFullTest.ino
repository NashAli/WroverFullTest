/*
  Created:      19:55 2022.01.02
  Last Updated: 14:45 2022.01.02
  MIT License

  Copyright (c) 2021 Zulfikar Naushad Ali

  Permission is hereby granted, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  This project uses the Devkit esp32 WROVER B module. TTGO T-Koala
  See docs --> WroverB FullTest.pdf for more info.
  I2C Device Listing
  0x20 - MCP23017 Port Expander
  0x3C - OLED - used



*/

//  DO NOT ALTER THE ORDER OF THESE INCLUDES
#include <Wire.h>
#include <WiFi.h>                 //  Wifi
#include <WiFiClient.h>           //  HTTP
#include <WiFiAP.h>               //  AP
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <EEPROM.h>               //  manage the auto-l
#include "BluetoothSerial.h"      //  BT for login
//#include "libssh_esp32.h"       //  SSH
#include <NTPClient.h>            //  time server
#include <time.h>                 //  standard time stuff

#include "system.h"               //  basic
#include "display_support.h"      //  oled
#include "sdcard_support.h"       //  sd card
#include "network.h"              //  network
#include "telnet_support.h"       //  telnet


void setup() {
  Serial.begin(115200);     //  for testing & debug
  InitDisplay();
  InitSDCard(SDCardSelect);
  ConfigNetwork();
  SetupTelnet();
  String logthis = ACyan + "System started@:" + AWhite + GetASCIITime();
  LogToSD(logthis);
  ShowTime();
}

void loop() {
  telnet.loop();
}
