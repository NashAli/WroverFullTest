#ifndef _SYSTEM_H
#define _SYSTEM_H
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

  This project uses the Devkit esp32 WROOM 32 module. TTGO
  See docs for more info.
  I2C Device Listing
  0x20 - MCP23017 Port Expander
  0x3C - OLED - used



*/
#include <MCP23017.h>



//***********************************************************************
//  DEFINES
//***********************************************************************
#define OFF false
#define ON true
#define MCP1 0x20
#define LED_BUILTIN 5   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Define MCP23017 registers values from datasheet
#define IODIRA    0x00  // IO direction A - 1= input 0 = output - set as all INPUT 0xFF 
#define IODIRB    0x01  // IO direction B - 1= input 0 = output - set as all OUTPUT 0x00
#define IPOLA     0x02  // Input polarity A
#define IPOLB     0x03  // Input polarity B
#define GPINTENA  0x04  // Interrupt-onchange A
#define GPINTENB  0x05  // Interrupt-onchange B
#define DEFVALA   0x06  // Default value for port A
#define DEFVALB   0x07  // Default value for port B
#define INTCONA   0x08  // Interrupt control register for port A
#define INTCONB   0x09  // Interrupt control register for port B
//#define IOCON     0x0A  // Configuration register //conflict with library macro??
#define GPPUA     0x0C  // Pull-up resistors for port A
#define GPPUB     0x0D  // Pull-up resistors for port B
#define INTFA     0x0E  // Interrupt condition for port A
#define INTFB     0x0F  // Interrupt condition for port B
#define INTCAPA   0x10  // Interrupt capture for port A
#define INTCAPB   0x11  // Interrupt capture for port B
#define GPIOA     0x12  // Data port A
#define GPIOB     0x13  // Data port B
#define OLATA     0x14  // Output latches A - all limit sw. - X,Y,Needle Position
#define OLATB     0x15  // Output latches B  - stepper motor controllers

//MCP23017 - port expand @ 0x20

//  MACHINE GLOBAL VARIABLES  ********************************************************

String OSNAME = "MicroBOS V1.0";

/*
   ANSI Terminal ESC sequences
*/
String ADarkGrey =      "\u001b[1;30m";
String ABlack =         "\u001b[30m";
String ARed =           "\u001b[31m";
String AGreen =         "\u001b[32m";
String AYellow =        "\u001b[33m";
String ABlue =          "\u001b[34m";
String AMagenta =       "\u001b[35m";
String ACyan =          "\u001b[36m";
String AWhite =         "\u001b[37m";
String AReset =         "\u001b[0m";
String ABrightBlack =   "\u001b[30;1m";
String ABrightRed =     "\u001b[31;1m";
String ABrightGreen =   "\u001b[32;1m";
String ABrightYellow =  "\u001b[33;1m";
String ABrightBlue =    "\u001b[34;1m";
String ABrightMagenta = "\u001b[35;1m";
String ABrightCyan =    "\u001b[36;1m";
String ABrightWhite =   "\u001b[37;1m";

String ABold =          "\u001b[1m";
String AClearScreen =   "\u001b[2J";
String AHomeCursor =    "\u001b[;H";
String ASlowBlink =     "\u001b[5m";
String AReverse =       "\u001b[7m";
String AUnderline =     "\u001b[4m";
String AUpaline =       "\u001b[1A";

String tab =            "\u0009";
String nl =             "\n";
String cr =             "\r";


// unicode special symbols

String revso_sym =      "\u005c";
String cr_sym =         "\u00A9";
String pat_sym =        "\u1360";
String tm_sym =         "\u2122";
String anchor_sym =     "\u2693";
String ard_sym =        "\u267E";
String sball_sym =      "\u26BD";
String bball_sym =      "\u26BE";
String sman_sym =       "\u26C4";
String psun_sym =       "\u26C5";
String golf_sym =       "\u26F3";
String sboat_sym =      "\u26F5";
String tent_sym =       "\u26FA";
String stars_sym =      "\u2728";

//  machine vars

bool SYSTEM_BUSY = false;     //  OS is busy.
bool Verbosity = true;        //  setting for communications protocol. Application can control response type ALPHA/NUMERIC
const int SDCardSelect = 4;   //  GPIO4 on T-Koala esp32
const int PWR_CTRL = 25;
const int VBAT_PIN = 35;
// change your threshold value here
const int touch_threshold = 20;
// variable for storing the touch pin temporary value
int touchValue;

const int httpPort = 80;
const int telnetPort = 23;
const int serverPort = 4080;
const char* ssidAP = "MBOSS";
const char* appwd = "nash";
const char* ntpServer = "ca.pool.ntp.org";
const int LOCAL_TIME_OFFSET = -5;
const long utcOffsetInSeconds = 3600 * LOCAL_TIME_OFFSET;
unsigned long startMillis;
unsigned long currentMillis;

// bluetooth vars
String buffer_in;
unsigned long previousMillis = 0;
byte valu;
int addr = 0;
String stream;
String temp;
String temp2;
unsigned int interval = 30000;  //  bluetooth wait time.

BluetoothSerial bt; //Object for Bluetooth
IPAddress ip;
AsyncWebServer httpServer(80);
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
MCP23017 mcp = MCP23017(MCP1);

/*
 * converts a String to Integer value, m is the base. ie. 16 = hexadecimal
 */
 int StrToInt(String cpv, int m){
  int iv;
  return iv = strtoul(cpv.c_str(), 0, m);
 }
/*
   Convert integer to hexadecimal string
*/
String IntToHexStr(int v) {
  char hex_string[20];
  sprintf(hex_string, "%2X", v); //convert number to hex
  return hex_string;
}

/*
   *************************************  TIMESTAMP for logs.
*/
String GetASCIITime() {
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime( &rawtime );
  String mt = asctime(info);
  mt.replace("\n", "");
  return mt;
}
void GreenLED(bool p) {
  if (p) {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  //  set LED ON
  } else {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);  //  set LED OFF
  }
}
void RedLED(bool p) {
  if (p) {
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);  //  set LED ON
  } else {
    pinMode(15, OUTPUT);
    digitalWrite(15, LOW);  //  set LED OFF
  }
}
void SetAuxPower(bool p) {
  if (p) {
    pinMode(PWR_CTRL, OUTPUT);
    digitalWrite(PWR_CTRL, LOW);  //  set PIN 1 ON - (3.3V switched)
  } else {
    pinMode(PWR_CTRL, OUTPUT);
    digitalWrite(PWR_CTRL, HIGH);  //  set PIN 1 OFF - (3.3V switched)
  }
}
/*
   These routines manage the MCP23017 port expander ic.(TBA)
*/
void InitPortExpander() {
  mcp.init();
  mcp.portMode(MCP23017Port::A, 0);     //  Port A as outputs
  mcp.portMode(MCP23017Port::B, 0);     //  Port B as outputs
  mcp.writeRegister(MCP23017Register::GPIO_A, 0x00);  //  Reset port A
  mcp.writeRegister(MCP23017Register::GPIO_B, 0x00);  //  Reset port B to default value()
}

void SetPortPin(int port, int pp, bool v) {


}

bool GetPortPin(int port, int pp) {
  int comval = 0;
  comval = mcp.readPort(MCP23017Port::A);

}

void SetPort(int pn, int v) {
  if (pn == 1) {
    mcp.writeRegister(MCP23017Register::GPIO_A, v);//  Set port A
  }
  else {
    mcp.writeRegister(MCP23017Register::GPIO_B, v);  //  Set port B
  }
}
/*
   Returns with the value of the port as a hexadecimal string.
*/
String GetPortString(int pn) {
  int comval;
  if (pn == 1) {
    comval = mcp.readPort(MCP23017Port::A);
  } else {
    comval = mcp.readPort(MCP23017Port::B);
  }
  return IntToHexStr(comval);
}
int GetPortInteger(int pn) {
  int comval;
  if (pn == 1) {
    comval = mcp.readPort(MCP23017Port::A);
  } else {
    comval = mcp.readPort(MCP23017Port::B);
  }
  return comval;
}


#endif
