

// VOLTAGES 
const uint16_t voltagePins[] = {A0,A1,A2,A3,A4,A5};
//const float voltageScale[] = {1.2366,1.55,1.39,1.36,1.36,1.664};
const float voltageScale[] = {1.2093,1.3958,1.2984,1.2737,1.2732,1.5227};
//const float voltageScale[] = {1,1,1,1,1,1};
float voltages [] = {-1,-1,-1,-1,-1,-1};

// TEMPS
const uint16_t tempsPins[] = {A6,A7};
const float tempScale[] = {10.87,10.87};
float temps [] = {-1,-1};

const int SMAPLE_AMOUNT = 150;


float readAnalogWithSampling(int pin){
  double readSum = 0;
  for(int i =0; i<SMAPLE_AMOUNT;i++){
    readSum+=analogRead(pin);
  }
  return readSum/SMAPLE_AMOUNT;
}


void handleVoltageSensor(int sensorNum){
   float newVoltage = ((readAnalogWithSampling(voltagePins[sensorNum])*5)/1024)*voltageScale[sensorNum];
  if(int(voltages[sensorNum]*100) != int(newVoltage*100)){
    voltages[sensorNum] = newVoltage;
  }

}
void handleTempSensor(int sensorNum){
  float newTemp = ((readAnalogWithSampling(tempsPins[sensorNum])*5)/1024)*tempScale[sensorNum];
  if(int(temps[sensorNum]*100) != int(newTemp*100)){
    temps[sensorNum] = newTemp;
  }
}

void sendFloat(float data){
    unsigned char* ptr;
    ptr = (unsigned char*)(&data);
    Serial.write(ptr,4);
}

void sendData(){
  Serial.write(0xff); // transmission start flag
  for(int i=0; i<6; i++)
    sendFloat(voltages[i]);
  
  for(int i=0; i<2; i++)
    sendFloat(temps[i]);
}


void setup() {
  Serial.begin(115200);
}


void loop() {

  for(int i =0; i< 6;i++)
      handleVoltageSensor(i);
  for(int i =0; i< 2;i++)
      handleTempSensor(i);
  sendData();

//  
//  float dunnmyVoltage = 3.35;
//    unsigned char* ptr;
//    ptr = (unsigned char*)(&dunnmyVoltage);
//    Serial.write(0xff);
//    Serial.write(ptr,4);



//    for(int i =0; i< 6;i++){
//      Serial.print(voltages[i]);
//      Serial.print(" ");
//    }
//    Serial.println();
//    for(int i =0; i< 2;i++){
//      Serial.print(temps[i]);
//      Serial.print(" ");
//    }
//    Serial.println();
  
}
