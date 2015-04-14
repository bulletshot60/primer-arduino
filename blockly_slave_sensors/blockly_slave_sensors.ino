/******************************************************************************/

#include <Wire.h>
#include <math.h>
#define COLOR_SENSOR_ADDR  0x39//the I2C address for the color sensor 
#define REG_CTL 0x80
#define REG_TIMING 0x81
#define REG_INT 0x82
#define REG_INT_SOURCE 0x83
#define REG_ID 0x84
#define REG_GAIN 0x87
#define REG_LOW_THRESH_LOW_BYTE 0x88
#define REG_LOW_THRESH_HIGH_BYTE 0x89
#define REG_HIGH_THRESH_LOW_BYTE 0x8A
#define REG_HIGH_THRESH_HIGH_BYTE 0x8B
#define REG_BLOCK_READ 0xCF
#define REG_GREEN_LOW 0xD0
#define REG_GREEN_HIGH 0xD1
#define REG_RED_LOW 0xD2
#define REG_RED_HIGH 0xD3
#define REG_BLUE_LOW 0xD4
#define REG_BLUE_HIGH 0xD5
#define REG_CLEAR_LOW 0xD6
#define REG_CLEAR_HIGH 0xD7
#define CTL_DAT_INIITIATE 0x03
#define CLR_INT 0xE0
//Timing Register
#define SYNC_EDGE 0x40
#define INTEG_MODE_FREE 0x00
#define INTEG_MODE_MANUAL 0x10
#define INTEG_MODE_SYN_SINGLE 0x20
#define INTEG_MODE_SYN_MULTI 0x30
 
#define INTEG_PARAM_PULSE_COUNT1 0x00
#define INTEG_PARAM_PULSE_COUNT2 0x01
#define INTEG_PARAM_PULSE_COUNT4 0x02
#define INTEG_PARAM_PULSE_COUNT8 0x03
//Interrupt Control Register 
#define INTR_STOP 40
#define INTR_DISABLE 0x00
#define INTR_LEVEL 0x10
#define INTR_PERSIST_EVERY 0x00
#define INTR_PERSIST_SINGLE 0x01
//Interrupt Souce Register
#define INT_SOURCE_GREEN 0x00
#define INT_SOURCE_RED 0x01
#define INT_SOURCE_BLUE 0x10
#define INT_SOURCE_CLEAR 0x03
//Gain Register
#define GAIN_1 0x00
#define GAIN_4 0x10
#define GAIN_16 0x20
#define GANI_64 0x30
#define PRESCALER_1 0x00
#define PRESCALER_2 0x01
#define PRESCALER_4 0x02
#define PRESCALER_8 0x03
#define PRESCALER_16 0x04
#define PRESCALER_32 0x05
#define PRESCALER_64 0x06

#define RED 104
#define GREEN 105
#define BLUE 106
#define BLACK 107
#define PURPLE 108
#define PINK 109
#define ORANGE 110
#define YELLOW 111

int reading_data[20];
int green, red, blue, clr, ctl;

void setup()
{  
  Serial.begin(9600);
  Wire.begin(3); 
  Wire.onReceive(receiveEvent);
}

int check_color = 0;

bool between(int value, int middle) {
  return middle - 1200 <= value && value <= middle + 1200;
}

bool between(int value, int middle, int spread) {
  return middle - spread <= value && value <= middle + spread;
}

void loop() {
  if(check_color != 0) {
    Serial.println("about to check color");
    setTimingReg(INTEG_MODE_FREE); //Set trigger mode.Including free mode,manually mode,single synchronizition mode or so.
    setInterruptSourceReg(INT_SOURCE_GREEN); //Set interrupt source 
    setInterruptControlReg(INTR_LEVEL|INTR_PERSIST_EVERY); //Set interrupt mode
    setGain(GAIN_1|PRESCALER_4); //Set gain value and prescaler value
    setEnableADC(); //Start ADC of the color sensor
    read_color(); //read color values
    clearInterrupt(); //clear interrupts setup by prescaler
    Wire.beginTransmission(1);
    if(between(red, 4500) && between(green, 5750) && between(blue, 5550) && between(clr, 16200, 2500)) {
      Serial.println("PURPLE"); 
      Wire.write(PURPLE);
    } else if(between(red, 2500) && between(green, 5000) && between(blue, 5250) && between(clr, 13500, 2500)) {
      Serial.println("BLUE"); 
      Wire.write(BLUE);
    } else if(between(red, 12000) && between(green, 8800) && between(blue, 3750) && between(clr, 24500, 2500)) {
      Serial.println("ORANGE"); 
      Wire.write(ORANGE);
    } else if(between(red, 11000) && between(green, 9500) && between(blue, 6500) && between(clr, 26750, 2500)) {
      Serial.println("PINK");
      Wire.write(PINK); 
    } else if(between(red, 10500) && between(green, 13500) && between(blue, 4350) && between(clr, 29000, 2500)) {
      Serial.println("YELLOW"); 
      Wire.write(YELLOW);
    } else if(between(red, 7700) && between(green, 4800) && between(blue, 3000) && between(clr, 16000, 2500)) {
      Serial.println("RED");
     Wire.write(RED); 
    } else if(between(red, 3300) && between(green, 8200) && between(blue, 3800) && between(clr, 16000, 2500)) {
      Serial.println("GREEN"); 
      Wire.write(GREEN);
    } else {
      Serial.println("BLACK");
      Wire.write(BLACK);
    }
    Wire.endTransmission();
    check_color = 0;
  }
  delay(5000);
}

/* These methods are provided by Grove Studio for interfacing with the color sensor */
void setTimingReg(int x)
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_TIMING);
   Wire.write(x);
   Wire.endTransmission();  
   delay(100); 
}
void setInterruptSourceReg(int x)
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_INT_SOURCE);
   Wire.write(x);
   Wire.endTransmission();  
   delay(100);
}
void setInterruptControlReg(int x)
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_INT);
   Wire.write(x);
   Wire.endTransmission();  
   delay(100);
}
void setGain(int x)
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_GAIN);
   Wire.write(x);
   Wire.endTransmission();
}
void setEnableADC()
{
 
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_CTL);
   Wire.write(CTL_DAT_INIITIATE);
   Wire.endTransmission();  
   delay(100);  
}
void clearInterrupt()
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(CLR_INT);
   Wire.endTransmission(); 
}
/* End Grove Methods */

void receiveEvent(int howMany) {
  int x = Wire.read(); // receive byte as an integer
  check_color = x;
}

void read_color()
{
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.write(REG_BLOCK_READ);
   Wire.endTransmission();
 
   Wire.beginTransmission(COLOR_SENSOR_ADDR);
   Wire.requestFrom(COLOR_SENSOR_ADDR,8);
   delay(500);
   if(8 <= Wire.available())    // if two bytes were received 
   { 
     for(int i = 0;i < 8; i++)
     {
       reading_data[i]=Wire.read();
     }
  }
  
  green = reading_data[1]*256+reading_data[0];
  red = reading_data[3]*256+reading_data[2];
  blue = reading_data[5]*256+reading_data[4];
  clr = reading_data[7]*256+reading_data[6];
  
  Serial.println("The RGB value and Clear channel value are");
  Serial.println(red,DEC);
  Serial.println(green,DEC);
  Serial.println(blue,DEC);
  Serial.println(clr,DEC);  
}
