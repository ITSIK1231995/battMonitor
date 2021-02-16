//  Created by Bodmer 24th Jan 2017

// The latest Arduino IDE versions support UTF-8 encoding of Unicode characters
// within sketches:
// https://playground.arduino.cc/Code/UTF-8

/*
  The library expects strings to be in UTF-8 encoded format:
  https://www.fileformat.info/info/unicode/utf8.htm

  Creating varaibles needs to be done with care when using character arrays:
  char c = 'µ';          // Wrong
  char bad[4] = "5µA";   // Wrong
  char good[] = "5µA";   // Good
  String okay = "5µA";   // Good

  This is because UTF-8 characters outside the basic Latin set occupy more than
  1 byte per character! A 16 bit unicode character occupies 3 bytes!

*/

//  The fonts are stored in arrays within the sketch tabs.

//  A processing sketch to create new fonts can be found in the Tools folder of TFT_eSPI
//  https://github.com/Bodmer/TFT_eSPI/tree/master/Tools/Create_Smooth_Font/Create_font

#include "BluetoothSerial.h"



#include "Final_Frontier_28.h"
#include "Latin_Hiragana_24.h"
#include "Unicode_Test_72.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//====================================================================================
//                                  Libraries
//====================================================================================

#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

uint16_t bg = TFT_BLACK;
uint16_t fg = TFT_WHITE;

// VOLTAGES 
const uint16_t voltageDisplyPos[6][2] = {{0,0},{80,0},{160,0},
                                        {0,30},{80,30},{160,30}};
float voltages [] = {-1,-1,-1,-1,-1,-1};

const float highMidVoltageTransision = 3.2;
const float midLowVoltageTransision = 3.0;

// TEMPS
const uint16_t tempDisplyPos[2][2] = {{15,65},{130,65}};
float temps [] = {-1,-1};

const float highMidTempTransision = 40;
const float midLowTempTransision = 30;



BluetoothSerial SerialBT;
bool bluetoothConected = false;
bool bluetoothStateChanged = true;


//HardwareSerial Serial3(2);
#define RXD2 2 
#define TXD2 15



//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200); // Used for messages and the C array generator
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  SerialBT.begin("batteryMonitor#1"); //Bluetooth device name

  
  tft.begin();
  tft.setRotation(3); 
  fg = TFT_WHITE;
  bg = TFT_BLACK;

  tft.setTextColor(fg, bg);
  tft.fillScreen(bg);

}

void writeBluetoothState(){
  if (bluetoothStateChanged){
    Serial.print("in here");
    bluetoothStateChanged = false;
    if(bluetoothConected){
      tft.setCursor(0,100);
      tft.setTextColor(TFT_BLACK, bg);
      tft.print("awaiting connection");
      tft.setCursor(0,100);
      tft.setTextColor(TFT_BLUE, bg);
      tft.print("bluetooth Conected");
    }else{
      tft.setCursor(0,100);
      tft.setTextColor(TFT_BLACK, bg);
      tft.print("bluetooth Conected");
      tft.setCursor(0,100);
      tft.setTextColor(TFT_BLUE, bg);
      tft.print("awaiting connection");
    }
      
  }  
}


int getVoltageColor(float voltage){
  if(voltage > highMidVoltageTransision)
    return TFT_GREEN;
  else if (voltage > midLowVoltageTransision)
    return TFT_YELLOW;
  else
    return TFT_RED;
}

void deleteOldVoltage(int sensorNum){
    tft.setCursor(voltageDisplyPos[sensorNum][0],voltageDisplyPos[sensorNum][1]);
    tft.setTextColor(TFT_BLACK, bg);
    tft.print(voltages[sensorNum]);
    tft.print("v");
}
void writeNewVoltage(int sensorNum,float newVoltage){
    tft.setCursor(voltageDisplyPos[sensorNum][0],voltageDisplyPos[sensorNum][1]);
    tft.setTextColor(getVoltageColor(newVoltage), bg);
    tft.print(newVoltage);
    tft.print("v");
}

void updateVoltage(int sensorNum,float newVoltage){
  if(int(voltages[sensorNum]*100) != int(newVoltage*100)){
    deleteOldVoltage(sensorNum);
    writeNewVoltage(sensorNum,newVoltage);
    voltages[sensorNum] = newVoltage;
  }
}

int getTempColor(float Temp){
  if(Temp > highMidTempTransision)
    return TFT_RED;
  else if (Temp > midLowTempTransision)
    return TFT_YELLOW;
  else
    return TFT_CYAN;
}


void deleteOldTemp(int sensorNum){
  tft.setCursor(tempDisplyPos[sensorNum][0],tempDisplyPos[sensorNum][1]);
  tft.setTextColor(TFT_BLACK, bg);
  tft.print(temps[sensorNum]);
  tft.println("c");
}
void writeNewTemp(int sensorNum,float newTemp){
  tft.setCursor(tempDisplyPos[sensorNum][0],tempDisplyPos[sensorNum][1]);
  tft.setTextColor(getTempColor(newTemp), bg);
  tft.print(newTemp);
  tft.println("c");
}



void updateTemp(int sensorNum,float newTemp){
    if(int(temps[sensorNum]*100) != int(newTemp*100)){
      deleteOldTemp(sensorNum);
      writeNewTemp(sensorNum,newTemp);
      temps[sensorNum] = newTemp;
  }
}

void waitForStartFlag(){
    while(!Serial2.available());
    byte buff[]  = {0,0,0,0};
    bool startConditionMeat = false;
    while(!startConditionMeat){
      Serial2.readBytes(buff,1);
      if(buff[0] == 0xff)
        startConditionMeat = true;
    }
}

void getDataAndUpdate(){
  waitForStartFlag();
  byte buff[]  = {0,0,0,0};
  float *Fptr = (float*)buff;
  
  for(int i =0; i< 6;i++){
    if(Serial2.available()>3){
      Serial2.readBytes(buff,4);
      updateVoltage(i,*Fptr);
    }
  }
  
  for(int i =0; i< 2;i++){
    if(Serial2.available()>3){
      Serial2.readBytes(buff,4);
      updateTemp(i,*Fptr);
    }
  }
}


void loop()
{
  tft.loadFont(Final_Frontier_28);
  
  getDataAndUpdate();



  if (SerialBT.available()) {
    if(!bluetoothConected)
      bluetoothStateChanged = true;
    bluetoothConected = true;
      for(int i =0; i< 6;i++){
        SerialBT.print(voltages[i]);
        SerialBT.print(" ");
      }
      SerialBT.println();
      for(int i =0; i< 2;i++){
        SerialBT.print(temps[i]);
        SerialBT.print(" ");
      }
      SerialBT.println();
  }
  else{
    if(bluetoothConected)
      bluetoothStateChanged = true;
    bluetoothConected = false;
    
  }

  writeBluetoothState();
  tft.unloadFont();

}
