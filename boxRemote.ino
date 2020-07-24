//SCK -- GPIO 14
//MISO -- GPIO 12
// MOSI -- GPIO 13
// CE -- GPIO 15
// CSN -- GPIO 5

// CE and CSN can be changed as per your preference just make sure to update it in the RF24 radio(15, 5); object below
//libraries
#include<Wire.h>
#include<MPU6050.h>
#include <SPI.h>
#include "RF24.h"
#define FREQUENCY    160   
#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}

//some declarations
RF24 radio(15, 5);
const uint64_t pipe = 0xE8E8F0F0E1LL; // you can change this according to your preference 
uint16_t data[3] = {0, 0, 0};
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
uint8_t scl = 4;
uint8_t sda = 0;
int btn[3]={2,1,3};//A,B,G
bool ledFlag=0;
bool mpuFlag=1;
uint8_t state=0;


void setup() {
  WiFi.forceSleepBegin();                  // turn off ESP8266 RF
  delay(1);                                // give RF section time to shutdown
  system_update_cpu_freq(FREQUENCY);       // update the system freq
  delay(10);
  
  pinMode(scl, INPUT); 
  pinMode(sda, INPUT);
  
  Wire.begin(scl, sda);
  mpu.initialize();
  
  for(int i=0; i<3; i++)
    pinMode(btn[i],INPUT_PULLUP);
  Serial.begin(115200);
  
  radio.setDataRate(RF24_250KBPS);
  
  //radio.setPALevel(RF24_PA_MAX); //Uncoment if you want more range and less battery life
  radio.begin();                    //Start the nRF24 communicate
  radio.openWritingPipe(pipe);
  delay(1);

}
//timing vars
unsigned long mainLoopT=0;

void loop() {

  //Lots of bit mapping after this
  
  if(mpuFlag)//get MPU values otherwise fill in the stopCar values
  getVal();
  else{
    data[0]=100<<8;
    data[0]+=100;
  }
  
  btnVals(); // get the button reads

  //get the RGB LED values as well as enable MPU
  if(data[1]==15 || ledFlag){
    ledFlag=1;
    if(data[1]!=15 && data[1]!=0){
    if(data[1]&B0001){
      ledFlag=0;
      mpuFlag=!mpuFlag;
    }
    else
      state=(data[1]&B1110)>>1;
    }
   if(data[1]==15)
      state=0;
    
    data[1]=15;
  }

  //These were some bits that sent status to the Car about the remote and RGB values 
  //This is totally specific to My RC car you can modify the code as your preference
  data[2]=255<<8;
  data[2]+=0B10000000|state;
  
  if(millis()-mainLoopT>50){// send data every 50 milliSconds increase or decrease depending on the performance
  radio.write(data, sizeof(data));
  
  mainLoopT=millis();
  }
  wdt_reset();// to avoid any unnecessary reboots
  delay(10);
}

void getVal() {
  //getting the MPU6050 values
  //could make use of mpu.setSleepEnabled(true); to put MPU6050 into sleep mode when not using
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  ax = map(ax, -17000, 17000, 0, 255);
  ay = map(ay, -17000, 17000, 0, 255);
  //packing mapped 8-bit values together into a 16bit value (MSB, LSB)
  data[0] = ax << 8;
  data[0] += ay;
}


void btnVals(){
  //charlieplexing buttons
  //getting data from 4 buttons using 3 GPIOs
  //delete this section and write a input code if dealing with 4 input lines for 4 buttons
  data[1]=0;
  pinMode(btn[0],INPUT_PULLUP);
  pinMode(btn[1],INPUT_PULLUP);
  pinMode(btn[2],INPUT_PULLUP);
  delayMicroseconds(100);
  data[1]+=!digitalRead(btn[0]);
  data[1]=data[1]<<1;
  data[1]+=!digitalRead(btn[1]);
  pinMode(btn[0],INPUT_PULLUP);
  pinMode(btn[2],INPUT_PULLUP);
  pinMode(btn[1],OUTPUT);
  digitalWrite(btn[1],LOW);
  delayMicroseconds(100);
  data[1]=data[1]<<1;
  data[1]+=!digitalRead(btn[2]);
  pinMode(btn[1],INPUT_PULLUP);
  pinMode(btn[2],INPUT_PULLUP);
  pinMode(btn[0],OUTPUT);
  
  digitalWrite(btn[0],LOW);
  delayMicroseconds(100);
  data[1]=data[1]<<1;
  data[1]+=!digitalRead(btn[2]);
}
