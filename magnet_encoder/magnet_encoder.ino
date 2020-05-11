#include <SPI.h>


// set pin 10 as the slave select for the encoder:
const int CS[] = {2};
word data_register = 0x3FFF;
word uncor_data_register = 0x3FFE;
word err_register = 0x0001;
word diag_register = 0x3FFC;
long value, value2;   
double avg1 = 0, avg2;
word mask_results = 0b0011111111111111; // this grabs the returned data without the read/write or parity bits.

const int size_of_stack1 = 10;
long stack1[size_of_stack1];
long stack2[7] ={0,0,0,0,0,0,0};


void setup() {
  Serial.begin(115200);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  
  SPI.setDataMode(SPI_MODE1);  
  SPI.setClockDivider(10);
  for (int i =0; i<sizeof(CS)/sizeof(CS[0]); i++){
    pinMode (CS[i], OUTPUT);  
    digitalWrite(CS[i], HIGH);
  }

  for(int i = 0; i < size_of_stack1; ++i)
  {
    stack1[i] = 0;
  }
  
}

long readRegister(int cs) {
  byte inByte = 0x00;   // incoming byte from the SPI
  byte inByte2 = 0x00;
  long result = 0;   // result to return
  digitalWrite(cs, LOW);
  delayMicroseconds(10);
  inByte = SPI.transfer(0x00);
  inByte2 = SPI.transfer(0x00);
  // combine the byte with the previous one:
  result = inByte << 8 | inByte2;
  // take the chip select high to de-select:
  digitalWrite(cs, HIGH);
  delayMicroseconds(10);
  // return the result:
  return result;
}

void loop() {


    value = readRegister(CS[0]) & mask_results; // I'm precomputing the parity
    //value2 = readRegister(CS[1]) & mask_results; // I'm precomputing the parity

    long summation = 0;
    for(int i = 0; i < size_of_stack1; ++i)
    {
      summation += stack1[i];
    }
    avg1 = long ((summation + value)/(size_of_stack1+1));
    
    Serial.println(avg1*360.0/16383.0);
        
    //Serial.println("1");

    for(int i = 0; i < size_of_stack1; ++i)
    {
      stack1[i+1] = stack1[i];
    }
    stack1[0] = value;
    
    delayMicroseconds(10);

}
