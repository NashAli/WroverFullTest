#ifndef _TELNET_SUPPORT_H
#define _TELNET_SUPPORT_H
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

  This project uses the Devkit esp32 WROVER B module. TTGO
  See docs for more info.
  I2C Device Listing
  0x20 - MCP23017 Port Expander
  0x3C - OLED - used
  ULN2803 buffer driver

  This unit's primary role is to provide direct builder diagnostics to the machine
  from the PC utilizing Putty to communicate.

  Available System Commands:

  netscan
  status
  log
  files
  sdcardinfo
  clr
  mkdir
  rmdir
  rename
  voltage
  getpin
  setpin
  getport
  setport
  i2cdetect

  Subject to change, without notice!
*/
#include "ESPTelnet.h"            //  Telnet

ESPTelnet telnet;       //  setup telnet

//  SYSTEM ***************************************************************************************************
/*
   System prompt
   rr - response required if '1'
   rt - 0 "OK", 1 "error", 2 "busy", 3 "Access Denied", 4 "Not Found", 5 "unknown".
   pt - prompt type (symbol) 0 - 4. none,top,machine,file,misc.
*/
void ResponsePrompt(int rr, int rt, int ptr) {

  //  outcomes for system response. eg. OK, error, busy, Access denied!, Not Found!
  if (rr == 1) {
    switch (rt) {
      case 0:
        if (Verbosity) {
          telnet.println(ABrightGreen + "OK");
        } else {
          telnet.println("0");
        }
        break;
      case 1:
        if (Verbosity) {
          telnet.println(ABrightRed + ASlowBlink + "error" + AReset);
        } else {
          telnet.println("1");
        }
        break;
      case 2:
        if (Verbosity) {
          telnet.println(ABrightBlue + "busy" + AReset);
        } else {
          telnet.println("2");
        }
        break;
      case 3:
        if (Verbosity) {
          telnet.println(ABrightMagenta  + ASlowBlink + "Access Denied!" + AReset);
        } else {
          telnet.println("3");
        }
        break;
      case 4:
        if (Verbosity) {
          telnet.println(ABrightYellow + "Not Found!" + AReset);
        } else {
          telnet.println("4");
        }
        break;
      default:
        if (Verbosity) {
          telnet.println(ABrightYellow + "unknown" + AReset);
        } else {
          telnet.println("5");
        }
        break;
    }
  }
  switch (ptr) {
    case 0:
      telnet.println(AReset);                     //  no prompt
      break;
    case 1:
      telnet.print(ABrightYellow + "$" + AReset); //  main prompt - top level
      break;
    case 2:
      telnet.print(ABrightCyan + "~" + AReset);   //  machine related
      break;
    case 3:
      telnet.print(ABrightRed + ">" + AReset);    //  used by filesystem
      break;
    case 4:
      telnet.print(ABrightYellow + "-" + AReset); //  misc.
      break;
    default:
      // if nothing else matches, do this
      telnet.print(ABrightYellow + "$" + AReset); //  default prompt
      break;
  }
}
//  NETWORK *********************************************************************************************************
/*

  Net scan - needs work !!!
  now if you are connected via telnet via the AP then the "No networks found" might make sense
  but on the other hand, if connected via a Router then No Networks found, doesn't get there!!!
*/
void TelNetScan() {
  ResponsePrompt(0, 0, 4); //subsystem prompt
  telnet.println( ACyan + "Scanning the Local Area Network..." + AReset);
  int nn = WiFi.scanNetworks();
  ResponsePrompt(0, 0, 4); //subsystem prompt
  telnet.println( ABrightWhite + "Currently there are " + ABrightGreen + String(nn) + ABrightWhite + " networks in the neighbourhood" + AReset);

  if (nn == 0) {
    //no networks found go AP
    telnet.println(ABrightRed + "No Local Networks Found? - Connect to AP address: 192.168.5.1");
    telnet.println(ABrightGreen + "Open your wifi settings and find the address above." + AReset);
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- no networks found.");
  } else {
    telnet.println(ABrightWhite + "List of available wifi connections:" + AReset);
    for (int i = 0; i < nn; ++i) {
      // Print SSID and RSSI for each network found
      String ss = String( WiFi.RSSI(i));
      String sc = String(i + 1);
      String sn = String(WiFi.SSID(i));
      ResponsePrompt(0, 0, 3); //system prompt
      if (i < 9) {
        telnet.print("0");
      }
      telnet.print(sc);
      telnet.print(ABrightBlue + ":" + AReset);
      if (sn == " " || sn == "") {
        sn = "unknown";
        telnet.print(ABrightYellow + sn + AReset);
      } else {
        telnet.print(ABrightCyan + sn + AReset);
      }
      telnet.print(ABrightYellow + "(" + AReset);
      telnet.print(ABrightBlue + ss);
      telnet.print(ABrightYellow + ")" + AReset);
      if (sn.length() < 7 ) {
        telnet.print(tab);
      }
      if (sn.length() < 15 ) {
        telnet.print(tab);
      }
      if (sn.length() < 23 ) {
        telnet.print(tab);
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
        telnet.println(ABrightGreen + "Open");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WEP) {
        telnet.println(AMagenta + "WEP");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_PSK) {
        telnet.println(ARed + "WPA-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK) {
        telnet.println(ABrightRed + "WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) {
        telnet.println(ARed + "WPA-WPA2-PSK");
      }
      if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_ENTERPRISE) {
        telnet.println(ABrightMagenta + "WPA2-ENTERPRISE");
      }
    }
  }
  telnet.println("");
  ResponsePrompt(1, 0, 1); //OK,system prompt
}
/*
   Displays system network status.
*/

void TelSysStat() {
  ResponsePrompt(0, 0, 2); //subsystem prompt
  telnet.println( ACyan + "Status:" + AReset);
  telnet.println(AYellow + "Current Local Time : " + ABrightWhite + GetASCIITime());
  telnet.print(ABrightBlue + "Local IP: " + ABrightYellow);
  telnet.println(WiFi.localIP().toString());
  telnet.println(AGreen + "Connected to IP Address: " + ABrightRed + telnet.getIP());
  telnet.println(AReset + "Access Point Address: 192.168.5.1");
  ResponsePrompt(1, 0, 1); //OK,system prompt
}
//  FILESYSTEM  *******************************************************************************************
/*
   *************************************************************************************** FILES
   Resursive function to retreive sd card file listing
*/
void TelFileDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      telnet.print(tab);
    }
    String en = String(entry.name());
    if (en.startsWith("/System Volume Information")) {
      en.replace("/System Volume Information", ".");
      en.replace("/WPSettings.dat", ".");
      en.replace("/IndexerVolumeGuid", ".");
      en.replace("/System Volume Information\\", ".");
      //telnet.print(ABrightRed + revso_sym);
      telnet.print(ABrightWhite + en + AReset);
    } else {
      en.replace("/", "");
      /*
        if (en.length() < 4) {
        telnet.print(tab);
        }
      */
      if (entry.isDirectory()) {
        String enu = String(entry.name());
        enu.toUpperCase();
        telnet.print(ABrightYellow + enu + AReset);
      } else {
        telnet.print(ABrightWhite + en + AReset);
      }
    }
    if (entry.isDirectory()) {
      telnet.println(ABrightGreen + tab + tab + "<DIR>" + AReset);
      TelFileDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      telnet.print(tab + tab);
      if (en.length() < 8) {
        telnet.print(tab);
      }
      String es = String(entry.size());
      telnet.println(AYellow + es + ACyan + " bytes" + AReset);
    }
    entry.close();
  }
}
/*
  ********************************************* List the files on SD card.
*/
void TelSysFiles(String dir) {

  telnet.println(ABrightGreen + "SD Card File Listings..." + AReset);
  //SD.begin(5);
  File root = SD.open(dir);
  TelFileDirectory(root, 0);
  ResponsePrompt(1, 0, 1); //OK,system prompt
}
/*
   ********************************************** Make Dir on SD card
*/
void TelMakeDir(String param) {
  telnet.println("Creating Dir: " + param);
  if (SD.mkdir("/" + param)) {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir created!");
    LogToSD(AWhite + GetASCIITime() + ABrightGreen + " +++ new dir created --> " + param);
  } else {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("mkdir failed");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- make dir fail --> " + param);
  }
  ResponsePrompt(1, 0, 1); //ok, top prompt
}
/*
  **************************************************  Delete Dir on SD card
*/
void TelRemoveDir(String param) {
  ResponsePrompt(0, 0, 3); //file prompt
  telnet.println("Removing Dir: " + param);
  if (SD.rmdir("/" + param)) {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("Dir removed!");
    LogToSD(AWhite + GetASCIITime() + ABrightGreen + " +++ dir removed --> " + param);
  } else {
    ResponsePrompt(0, 0, 3); //file prompt
    telnet.println("rmdir failed");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- remove dir fail --> " + param);
  }
  ResponsePrompt(1, 0, 1); //ok, top prompt
}
/*
  ****************************************************  List a file's contents to the terminal(dump)
*/
void TelListFile(String param) {
  String fn =   param ;
  File myfile = SD.open(fn, FILE_READ);
  // if the file is available, read it:
  if (myfile) {
    while (myfile.available()) {
      telnet.print(myfile.read());
    }
    myfile.close();
    LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ file looked at--> " + param);
    ResponsePrompt(1, 0, 1); //OK, system prompt
  } else {
    ResponsePrompt(0, 0, 2);
    telnet.println("----fail");
    LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- list file - fail.");
    ResponsePrompt(0, 0, 1); // just system prompt
  }
}
void TelRenameFile(String oldname, String newname) {

  ResponsePrompt(0, 0, 1); // just system prompt
}
/*
  ********************************************************* Show the system log
*/
void TelShowSysLogs() {
  telnet.println(ACyan + "Logs..." + AReset);
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  if (SDCARD_READY) {
    if (! SD.exists("/syslog.txt")) {
      telnet.println(ABrightBlue + "creating new logfile..." + AReset);
      // only open a new file if it doesn't exist
      File myfile = SD.open("/syslog.txt", FILE_WRITE);
      String header = ABrightMagenta + "eMBOS -" + ABrightRed + " SYSTEM LOG - " + ABrightCyan + "MASTER";
      myfile.println(header);
      myfile.println(ABrightCyan + "System Log Started@:" + AWhite + GetASCIITime());
      String user = AWhite + GetASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
      myfile.println(user);
      myfile.close();
    }
    telnet.println(ABrightBlue + "-->log found!" + AGreen + " dumping log..." + AReset);
    String fn = String("/syslog.txt" );
    File myfile = SD.open(fn, FILE_READ);
    // if the file is available, read it:
    if (myfile) {
      while (myfile.available()) {
        telnet.print(myfile.read());
      }
      myfile.close();
    } else {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightRed + "unknown error opening --> syslog.txt" + AReset);
      ResponsePrompt(1, 3, 1);  //  access denied, system prompt.
    }
  }
  ResponsePrompt(1, 0, 1); //OK, system prompt
}

/*
   *********************************************  Delete System logs
*/
void TelDeleteSysLogs() {
  ResponsePrompt(0, 0, 2);// subsystem prompt only
  telnet.println(ABrightRed + "Deleted, cannot be undone! --> syslog.txt" + AReset);
  SD.remove("/syslog.txt");
  ResponsePrompt(1, 0, 1);//  OK, system prompt
}
/*
  ************************************************  SD Card Info
*/
void  TelSDCardInfo() {
  ResponsePrompt(0, 0, 2); // just sub-system prompt
  telnet.println(ABrightGreen + "SD Card Information" + AReset);
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_MMC) {
    telnet.println(ABrightBlue + "CardType - MMC" + AReset);
  }
  else if (cardType == CARD_SD) {
    telnet.println(ABrightBlue + "CardType - SD" + AReset);
  }
  else if (cardType == CARD_SDHC) {
    telnet.println(ABrightBlue + "CardType - SDHC" + AReset);
  }
  else {
    telnet.println(ABrightRed + "CardType - unknown" + AReset);
  }
  int cardSize = SD.cardSize() / (1024 * 1024);
  int tbu = SD.totalBytes() / (1024 * 1024);
  int ubu = SD.usedBytes() / (1024 * 1024);
  String sdcs = String(cardSize);
  telnet.println(AWhite + "SD Card Size: " + sdcs + " MB");
  String tb = String(tbu);
  String ub = String(ubu);
  telnet.println("Total space: " + tb + " MB");
  telnet.println("Used space: " + ub + " MB" );
  ResponsePrompt(1, 0, 1); // OK, system prompt
}
//  HARDWARE  ********************************************************************************************************
/*
   ***********************************************************  ScanI2CBus is implemented - 9/21/2020
  this has to be done before init sensor group sets the sensor availability so the system can ignore offline sensors
  to manage performance and no bad data woes.  The i2c_scanner uses the return value of the Write.endTransmisstion to
  see if a device did acknowledge to the address. This version is the Telnet version.
*/

void TelI2CBusScan() {
  ResponsePrompt(1, 0, 2); //OK, subsystem prompt only
  telnet.println(ABrightYellow + "Running i2c bus scan!.. Standby." + AReset);
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --> i2c test started.");

  int lastRow = 50;
  byte error, address;
  int nDevices;
  Wire.begin();
  DrawBanner();
  display1.setCursor(10, 15);
  display1.print("I2C Bus Scan....");
  display1.display();

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      DrawBanner();
      display1.setCursor(10, 25);
      display1.print("I2C at 0x");
      telnet.print(ABrightGreen + "I2C device at 0x");
      if (address < 0x10) {
        display1.print("0");
        telnet.print("0");
      }
      char charVal[4];
      sprintf(charVal, "%04X", address);
      telnet.println(charVal + AReset);
      display1.print(address, HEX);
      display1.print(" hex!");
      display1.display();

      if (address == 0x20) {
        telnet.println(ABrightYellow + "PortExpander @" + ABrightBlue + " address" + ABrightCyan + " 0x20" + AReset);
        display1.setCursor(10, 35);
        display1.print("PortExpander");
        display1.display();
      }

      else if (address == 0x3C) {
        telnet.println(ABrightYellow + "OLED @" + ABrightBlue + " address" + ABrightCyan + " 0x3C" + AReset);
        display1.setCursor(10, 35);
        display1.print("OLED - Me!");
        display1.display();
      }

      nDevices++;
      delay(500);
    } else if (error == 4) {
      display1.setCursor(5, lastRow);
      display1.print("Error at address 0x");
      telnet.println(ABrightRed + "Error at address 0x");
      if (address < 16) {
        display1.print("0");
        telnet.print("0");

      }
      char charVal[4];
      sprintf(charVal, "%04X", address);
      telnet.println(charVal + AReset);
      display1.println(address, HEX);
      display1.display();
    }
  }
  if (nDevices == 0) {
    DrawBanner();
    display1.setCursor(10, lastRow);
    display1.print("No I2C devices found");
    telnet.print( ABrightRed + "No I2C devices found" + AReset);
    display1.display();
  } else {
    display1.setCursor(10, lastRow);
    display1.print("That's all. Done.");
    telnet.println( ABrightWhite + "That's all. Done." + AReset);
    display1.display();
  }
  ResponsePrompt(1, 0, 1); //OK!, system prompt
}

/*
 ********************************** Telnet Battery Info
*/
void TelBatteryInfo() {
  ResponsePrompt(0, 0, 2); //subsystem prompt
  float battery_voltage = ((float)analogRead(VBAT_PIN) / 4095.0) * 2.0 * 3.3 * (1100 / 1000.0);
  String voltage =  String(battery_voltage) + ABrightYellow + " Volts";
  telnet.println(ABrightBlue + "Battery Voltage :" + ABrightGreen + voltage + AReset);
  ResponsePrompt(1, 0, 1); //OK,system prompt
}
/*
  Gets the hexadecimal value of the specified port.
*/
void TelGetPort(String p) {
  String pf = "";
  if (p == "A" || p == "a") {
    pf = GetPortString(1);
    telnet.println( ABrightCyan + "Port A:" + ABrightWhite + " 0x" + pf + AReset);
  }
  else {
    pf = GetPortString(2);
    telnet.println( ABrightCyan + "Port B:"  + ABrightWhite + " 0x" + pf + AReset);
  }
  ResponsePrompt(1, 0, 1); //OK,system prompt
}
/*
  Sets the hexadecimal value of a port.
*/
void TelSetPort(String p, int pv) {
  if (p == "A" || p == "a") {
    SetPort(1, pv);
  } else {
    SetPort(2, pv);
  }
  ResponsePrompt(0, 0, 1); // just system prompt
}
void TelGetPin(String port, String pin) {
  int iport,ipin;
  bool pv = GetPortPin(iport, ipin);
  if(pv){
    ResponsePrompt(0, 0, 3); //cursor only
    telnet.println(ABrightBlue + "H" + AReset);
  }else{
    ResponsePrompt(0, 0, 3); //cursor only
    telnet.println(ABrightRed + "L" + AReset);
  }
  ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
}

void TelSetPin(String port, String pin, String pval) {

  ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
}


/*
********************************************  cancel
*/
void TelSysCancel(String param) {
  ResponsePrompt(0, 0, 2); //subsystem prompt only
  telnet.println("nothing to cancel");
  ResponsePrompt(1, 0, 1); //system prompt
}

/*
**********************************  HELP SYSTEM ************************************************
*/
void TelSysHelp() {
  telnet.println(AClearScreen + AHomeCursor);
  telnet.print(ABrightCyan + ABold + " " + sboat_sym);
  telnet.print( ABrightRed + ABold + "          " + pat_sym + " " + ASlowBlink + "microBOS" + AReset);
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + "   V1.0" + AReset);
  telnet.println(tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2022");
  telnet.println(ABrightYellow + tab + tab + "General Help system!" + AReset);
  telnet.println();
  telnet.println(tab + ABrightMagenta + "Commands:");
  telnet.println();
  telnet.println(AGreen + " netscan" + AWhite + "         - returns scan of network.");
  telnet.println(AGreen + " status" + AWhite + "          - returns complete status on wifi system.");
  telnet.println(AGreen + " log" + AWhite + "             - system log.");
  telnet.println(AGreen + " clr/clear" + AWhite + "       - clears the screen.");
  telnet.println(AGreen + " sdcardinfo" + AWhite + "      - returns SD card info.");
  telnet.println(AGreen + " files" + AWhite + "           - list file system on machine.");
  telnet.println(AGreen + " list" + AWhite + "            - dumps file to screen.");
  telnet.println(AGreen + " mkdir" + AWhite + "           - creates a new directory.");
  telnet.println(AGreen + " rmdir" + AWhite + "           - deletes named directory.");
  telnet.println(AGreen + " rename" + AWhite + "          - renames a file.");
  telnet.println(ARed +   " setpin" + AWhite + "          - sets a pin HIGH or LOW.");
  telnet.println(AGreen + " setport" + AWhite + "         - sets a port value of the port expander(A/B port).");
  telnet.println(ARed +   " getpin" + AWhite + "          - gets the value of a pin (H/L).");
  telnet.println(AGreen + " getport" + AWhite + "         - gets the port value of the port expander (A/B port).");
  telnet.println(AGreen + " battery/volts" + AWhite + "   - reports the status of the battery.");
  telnet.println(AGreen + " i2cdetect" + AWhite + "       - scans the i2c bus for devices.");
  telnet.println();
  telnet.println(tab + ABrightWhite + "Type" + ACyan + " [command]" + AYellow + "-" + ABrightGreen + "help" + AYellow + " / -" + ABrightGreen + "?" + ABrightBlue + " for more details." + AReset);
  telnet.println();
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelSetPortHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "setport" + ABrightWhite + tab + "- sets a port value of the port expander(A/B port).");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " setport-" + AMagenta + "<a:b>,<0-FF>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " nothing." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelGetPortHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "getport" + ABrightWhite + tab + "- gets a port value of the port expander(A/B port).");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " getport-" + AMagenta + "<a:b>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " port value in hexadecimal." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelSetPinHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "setpin" + ABrightWhite + tab + "- sets a pin value of the port expander(A/B port).");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " setpin-" + AMagenta + "<a:b>,<pin#>,<H:L>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " nothing." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelGetPinHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "getpin" + ABrightWhite + tab + "- gets a pin value of the port expander(A/B port).");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " getpin-" + AMagenta + "<a:b>,<pin#>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " a pin value (<H>:<L>)." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelRemoveDirHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "rmdir" + ABrightWhite + tab + "- deletes named directory.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " rmdir-" + AMagenta + "<dirname>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " nothing." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelMakeDirHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "mkdir" + ABrightWhite + tab + "- makes named directory.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " mkdir-" + AMagenta + "<dirname>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " nothing." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelLogHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "log" + ABrightWhite + tab + "- displays log information.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen  + "usage:" + ABrightCyan + " log-" + AMagenta + "<del:list>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " log info." + AReset);
  ResponsePrompt( 1, 0, 1); //system prompt
}
void TelListFileHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "list" + ABrightWhite + tab + "- dumps file contents to screen.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " list-" + AMagenta + "<dir><filename>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " contents of a file." + AReset);
  ResponsePrompt(1, 0, 1);  //system prompt
}
void TelI2CHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "i2cdetect" + ABrightWhite + tab + "- scans the i2c bus for devices.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " i2c...-i2cdetect" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " a list of i2c devices." + AReset);
  ResponsePrompt(1, 0, 1);  //system prompt
}
void TelBatteryHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "battery" + ABrightWhite + tab + "- reports the status of the battery.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " battery/volts/bat/voltage" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " battery voltage" + AReset);
  ResponsePrompt(1, 0, 1);  //system prompt
}
void TelRenameFileHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");;
  telnet.println(tab + ABrightYellow + "rename" + ABrightWhite + tab + "- renames a file.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen  + "usage:" + ABrightCyan + " rename-" + AMagenta + "<old filename>,<new filename>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " nothing." + AReset);
  ResponsePrompt(1, 0, 1);  //system prompt
}
void TelFilesHelp() {
  telnet.println(tab + tab + ABrightBlue + "COMMAND HELP");
  telnet.println(tab + ABrightYellow + "files" + ABrightWhite + tab + "- list files to screen.");
  telnet.println();
  telnet.println(tab + tab + ABrightGreen + "usage:" + ABrightCyan + " files-" + AMagenta + "<dir>" + AReset);
  telnet.println(tab + tab + ABrightBlue + "returns:" + ABrightRed + " a list of files in the specified dir." + AReset);
  ResponsePrompt(1, 0, 1);  //system prompt
}
/*
  ***************************************************** MISC  ******************************************
*/
void TelQuit() {
  telnet.println("Sorry there is no quitting!");
  ResponsePrompt(1, 0, 0); //no system prompt
}


/* -------------------------- COMMAND PARSER ------------------------------------------------------- */

void ParseCommand(String command) {
  //parses the telnet entry and locates instructions and completes the tasks

  if (command.length() == 0) {
    ResponsePrompt(1, 5, 1); //unknown - system prompt
  }

  else if (command == "sdcardinfo" || command == "sdc" || command == "sdcard" || command == "sdinfo") {
    TelSDCardInfo();
  }

  else if (command == "netscan" || command == "net" || command == "nets" || command == "netsc") {
    TelNetScan();
  }

  else if (command == "status" || command == "sta" || command == "stat" || command == "statu") {
    TelSysStat();
  }

  else if (command == "clr" || command == "clear") {
    telnet.println(AClearScreen + AHomeCursor);
    ResponsePrompt(0, 0, 1);
  }

  else if (command.startsWith("log")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    telnet.println(param);
    if (param == "del" || param == "DEL" || param == "delete" || param == "DELETE") {
      TelDeleteSysLogs();
    }
    else if (param == "l" || param == "L" || param == "List" || param == "list") {
      TelShowSysLogs();
    }
    else if (param == command || param == "?" || param == "help") {
      TelLogHelp();
    }
  }

  else if (command.startsWith("list")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelListFileHelp();
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Screen Dump FileName: " + ABrightCyan + param + AReset);
      TelListFile(param);
    }
  }

  else if (command.startsWith("mkdir")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelMakeDirHelp();
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Create Directory: " + ABrightCyan + param + AReset);
      TelMakeDir(param);
    }
  }

  else if (command.startsWith("rmdir")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelRemoveDirHelp();
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Remove Directory: " + ABrightCyan + param + AReset);
      TelRemoveDir(param);
    }
  }

  else if (command.startsWith("rename")) {
    ResponsePrompt(1, 0, 1);
    String param = command.substring(command.indexOf(' ') + 1, command.indexOf(',') - 1);
    String newname = command.substring(command.indexOf(',') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      TelRenameFileHelp();
    } else {
      ResponsePrompt(0, 0, 3);
      telnet.println(ABrightYellow + "Renamed: " + ABrightCyan + param + " to " + newname + AReset);
      TelRenameFile(param, newname);
      LogToSD(AWhite + GetASCIITime() + ABrightRed + " --> file rename - " + param + " to " + newname);
    }
  }

  else if (command.startsWith("files")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelFilesHelp();
    } else {
      TelSysFiles(param);
    }
  }

  else if (command.startsWith("cancel")) {
    ResponsePrompt(1, 0, 1);//OK, top level prompt
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if  (param == command || param == "?" || param == "help") {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightYellow + "usage: cancel -<jobname>");
      ResponsePrompt(1, 0, 1);
    } else {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightYellow + "Cancel Job: " + ABrightCyan + param + AReset);
      TelSysCancel(param);
      LogToSD(AWhite + GetASCIITime() + ABrightBlue + " --- cancel job --> " + param);
    }
  }

  else if (command == "help" || command == "?" || command == "help?" || command == "Help" || command == "HELP" || command == "HELP!") {
    TelSysHelp();
  }

  else if (command.startsWith("#")) {
    ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
  }
  else if (command == "battery" || command == "bat" || command == "batt") {
    TelBatteryInfo();
  }
  
else if (command == "volt" || command == "volts" || command == "voltage") {
    TelBatteryInfo();
  }
  else if (command.startsWith("i2c")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelGetPortHelp();
    } else {
      TelI2CBusScan();
    }
  }

  else if (command.startsWith("getport")) {
    String param = command.substring(command.indexOf('-') + 1, command.length());
    if (param == "?" || param == "help") {
      TelGetPortHelp();
      //ResponsePrompt(1, 0, 1); //OK, system prompt - normal completion.
    } else {
      TelGetPort(param);
    }
  }

  else if (command.startsWith("setport")) {
    String param = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String apv = command.substring(command.indexOf(',') + 1, command.length());
    //  converts hex string to integer value
    int pv = StrToInt(apv, 16);
    if (pv > 255) {
      ResponsePrompt(0, 0, 2);
      telnet.println(ABrightMagenta +  ABold + "Input value error - too large!" + AReset);
    } else {
      TelSetPort(param, pv);
    }
    if (param == "?" || param == "help") {
      TelSetPortHelp();
    }
  }

  else if (command.startsWith("getpin")) {
    String port = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String pin = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    if (port == "?" || port == "help") {
      TelGetPinHelp();
    } else {
      TelGetPin(port, pin);
    }
  }

  else if (command.startsWith("setpin")) {
    String port = command.substring(command.indexOf('-') + 1, command.indexOf(',') - 1);
    String pin = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    String pval = command.substring(command.indexOf(',') + 1, command.indexOf(',') - 1);
    if (port == "?" || port == "help") {
      TelSetPinHelp();
    } else {
      TelSetPin(port, pin, pval);
    }
  }
  // more commands goes here...
}
/*
 * *********************************  TELNET SETUP & BASE FUNCTIONS ***********************************************
*/
// callback functions for telnet events
void OnTelnetConnect(String ip) {
  telnet.println(AClearScreen);
  telnet.println(AHomeCursor);
  telnet.print(ABrightCyan + ABold + sboat_sym);
  telnet.print(tab + ABrightRed + ABold + pat_sym + tab + ASlowBlink + "microBOS");
  telnet.println(ABrightYellow + ABold + tm_sym + AReset + ABrightGreen + " V1.0" + AReset);
  telnet.println(tab + ABrightYellow + cr_sym + ABrightCyan + "Graffiti Softwerks" + ABrightGreen + " All Rights Reserved 2022");
  telnet.print(ABrightCyan + "System Local Time: ");
  String nt = String(GetASCIITime());
  telnet.println(AWhite + nt );
  telnet.println(ABrightMagenta +  ABold + "Telnet Service:");
  telnet.println(ACyan + "Welcome user -> " + AYellow + telnet.getIP());
  String loguser = AWhite + GetASCIITime() + tab + ABrightYellow + "Telnet User@" + telnet.getIP() + tab + ABrightGreen + "logged in sucessfully!";
  LogToSD(loguser);
  telnet.println(AGreen + "Type '?' or 'help' for commands" + AReset);
  ResponsePrompt(1, 0, 1);
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(25, 25);
  display1.print("connected to");
  display1.setCursor(30, 40);
  display1.print(telnet.getIP());
  display1.display();
}

void OnTelnetDisconnect(String ip) {
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("disconnected from");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- user disconnected@" + telnet.getIP() + nl);
}

void OnTelnetReconnect(String ip) {
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("reconnected to");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightBlue + " +++ user re-connected@" + telnet.getIP() + nl);
}

void OnTelnetConnectionAttempt(String ip) {
  DrawBanner();
  display1.setCursor(50, 15);
  display1.print("Telnet:");
  display1.setCursor(15, 25);
  display1.print("rejected");
  display1.setCursor(30, 40);
  display1.print("IP Address:");
  display1.setCursor(27, 50);
  display1.print(telnet.getIP());
  display1.display();
  LogToSD(AWhite + GetASCIITime() + ABrightRed + " --- user rejected@" + telnet.getIP() + nl);
}

void SetupTelnet() {
  // passing on functions for various telnet events
  telnet.onConnect(OnTelnetConnect);
  telnet.onConnectionAttempt(OnTelnetConnectionAttempt);
  telnet.onReconnect(OnTelnetReconnect);
  telnet.onDisconnect(OnTelnetDisconnect);
  // passing a lambda function
  telnet.onInputReceived([](String str) {
    // checks for a command entered by user at terminal.
    ParseCommand(str);
  });
  DrawBanner();
  display1.setCursor(45, 15);
  display1.print("Telnet:");
  if (telnet.begin()) {
    display1.setCursor(20, 25);
    display1.print("service running");
    display1.display();
  } else {
    ShowMessage("Will reboot...");
  }
}

#endif
