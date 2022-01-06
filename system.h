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




//***********************************************************************
//  DEFINES
//***********************************************************************
#define OFF 0
#define ON 1
#define MCP1 0x20
#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

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

//MCP23017 - MOTOR_CONTROLLER @ 0x20



//  MACHINE GLOBAL VARIABLES  ********************************************************

String OSNAME = "MicroBOS V2.0";



/*
   ANSI Terminal ESC sequences
*/
String ADarkGrey ="\u001b[1;30m";
String ABlack =  "\u001b[30m";
String ARed =  "\u001b[31m";
String AGreen =  "\u001b[32m";
String AYellow =  "\u001b[33m";
String ABlue =  "\u001b[34m";
String AMagenta =  "\u001b[35m";
String ACyan =  "\u001b[36m";
String AWhite =  "\u001b[37m";
String AReset =  "\u001b[0m";
String ABrightBlack = "\u001b[30;1m";
String ABrightRed = "\u001b[31;1m";
String ABrightGreen = "\u001b[32;1m";
String ABrightYellow = "\u001b[33;1m";
String ABrightBlue = "\u001b[34;1m";
String ABrightMagenta = "\u001b[35;1m";
String ABrightCyan = "\u001b[36;1m";
String ABrightWhite = "\u001b[37;1m";

String ABold = "\u001b[1m";
String AClearScreen = "\u001b[2J";
String AHomeCursor = "\u001b[;H";
String ASlowBlink = "\u001b[5m";
String AReverse = "\u001b[7m";
String AUnderline = "\u001b[4m";
String AUpaline = "\u001b[1A";
String tab = "\u0009";
String nl = "\n";
String cr = "\r";


// unicode special symbols
String revso_sym = "\u005c";
String cr_sym =  "\u00A9";
String pat_sym =  "\u1360";
String tm_sym =  "\u2122";
String anchor_sym =  "\u2693";
String ard_sym =  "\u267E";
String sball_sym =  "\u26BD";
String bball_sym =  "\u26BE";
String sman_sym =  "\u26C4";
String psun_sym =  "\u26C5";
String golf_sym =  "\u26F3";
String sboat_sym =  "\u26F5";
String tent_sym =  "\u26FA";
String stars_sym =  "\u2728";

//  machine vars

int XORG, YORG, XPOS, YPOS, XDEST, YDEST, XOFFSET, YOFFSET, SXHOME, SYHOME;
bool SYSTEM_BUSY = false;     //  OS is busy.
bool MachineRun = false;      //  a motor is cycling (X-Y-Z).
bool LIMITS_FLAG = false;    //  a boundary has been breached.
bool NeedleUp = false;        //  logic to manage the needle (Z) to keep it out of
bool NeedleDown = false;      //  harms way when the fabric gantry is in motion.
bool Forward = true;          //  direction logic for DRV8845
bool CalGood = false;         //  calibration has been completed sucessfully.

bool Verbosity = true;        //  setting for communications protocol. Application can control response type ALPHA/NUMERIC


const int SDCardSelect = 5;   //  GPIO5 on esp32
const int WorkLights = 34;    //  GPIO34 - work lights via the ULN2803A
int MWAIT = 10;               //  motor wait timing

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
unsigned int interval = 30000;

BluetoothSerial bt; //Object for Bluetooth
IPAddress ip;
AsyncWebServer httpServer(80);
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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


#endif
