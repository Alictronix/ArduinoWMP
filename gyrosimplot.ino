#include <Wire.h>
byte data[6]; //six data bytes
int yaw, pitch, roll; //three axes
int yaw0, pitch0, roll0; //calibration zeroes

void wmpOn(){
Wire.beginTransmission(0x53); //WM+ starts out deactivated at address 0x53
Wire.write(0xfe); //send 0x04 to address 0xFE to activate WM+
Wire.write(0x04);
Wire.endTransmission(); //WM+ jumps to address 0x52 and is now active
}

void wmpSendZero(){
Wire.beginTransmission(0x52); //now at address 0x52
Wire.write(0x00); //send zero to signal we want info
Wire.endTransmission();
}

void calibrateZeroes(){
for (int i=0;i<10;i++){
wmpSendZero();
Wire.requestFrom(0x52,6);
for (int i=0;i<6;i++){
data[i]=Wire.read();
}
yaw0+=(((data[3]>>2)<<8)+data[0])/10; //average 10 readings
pitch0+=(((data[4]>>2)<<8)+data[1])/10;
roll0+=(((data[5]>>2)<<8)+data[2])/10;
}
/*Serial.print("Yaw0:");
Serial.print(yaw0);
Serial.print(" Pitch0:");
Serial.print(pitch0);
Serial.print(" Roll0:");
Serial.println(roll0);
*/
}

void receiveData(){
wmpSendZero(); //send zero before each request 
Wire.requestFrom(0x52,6); //request the six bytes from the WM+
for (int i=0;i<6;i++){
data[i]=Wire.read();
}
yaw=((data[3]>>2)<<8)+data[0]-yaw0;
pitch=((data[4]>>2)<<8)+data[1]-pitch0;
roll=((data[5]>>2)<<8)+data[2]-roll0;
}

void setup(){
Serial.begin(57600); 
Wire.begin();
wmpOn(); //turn WM+ on
calibrateZeroes(); //calibrate zeroes
delay(1000);
}
int buffer[20]; //Buffer needed to store data packet for transmission
//int data1;
//int data2;
//int data3;
//int data4;

void loop(){
receiveData(); //receive data and calculate yaw pitch and roll

/*Serial.print("yaw:");
Serial.print(yaw); //for info on which axis is which
Serial.print(" pitch:");
Serial.print(pitch);
Serial.print(" roll:");
Serial.println(roll);
*/
plot(yaw,pitch,roll); 

delay(100);
}

void plot(int yaw, int pitch, int roll)
{
  int pktSize;
 
  buffer[0] = 0xCDAB;             //SimPlot packet header. Indicates start of data packet
  buffer[1] = 3*sizeof(int);      //Size of data in bytes. Does not include the header and size fields
  buffer[2] = yaw;
  buffer[3] = pitch;
  buffer[4] = roll;
   
  pktSize = 2 + 2 + (3*sizeof(int)); //Header bytes + size field bytes + data
 
  //IMPORTANT: Change to serial port that is connected to PC
  Serial.write((uint8_t * )buffer, pktSize);
}
