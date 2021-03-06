#include <SPI.h>

const int CS[] = {2,3,4,5};
const int num_data = sizeof(CS)/sizeof(int);

word data_register = 0x3FFF;
word uncor_data_register = 0x3FFE;
word err_register = 0x0001;
word diag_register = 0x3FFC;
word mask_results = 0b0011111111111111;

long value[num_data];   
long avg[num_data];
const int size_of_stack = 7;

long stack[num_data][size_of_stack];



void setup() {
  Serial.begin(115200);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  
  SPI.setDataMode(SPI_MODE1);  
  SPI.setClockDivider(10);
  
  for (int i = 0; i<sizeof(CS)/sizeof(CS[0]); i++){
    pinMode (CS[i], OUTPUT);  
    digitalWrite(CS[i], HIGH);
  }

  for(int i = 0; i < num_data; ++i)
  {
    for(int j = 0; j < size_of_stack; ++j)
    {
      stack[i][j] = 0;
    }
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
  
    // Data read
    for(int i = 0; i < num_data; ++i)
    {
    value[i] = readRegister(CS[i]) & mask_results;
    }


    // Averaging
    long sum;
    
    for(int i = 0; i < num_data; ++i)
    {
      sum = 0;
      for(int j = 0; j < size_of_stack-1; ++j)
      {
        sum += (long)stack[i][j];
      }
      sum += value[i];
      avg[i] = (long) sum / size_of_stack;
    }

    for(int i = 0; i < num_data; ++i)
    {
      Serial.print((avg[i])*360.0/16363.0);
      Serial.print("\t");
    }
    Serial.print("\n");
    
    if(Serial.read() == 'a'){

    //Serial.println(String(value)+","+String(value2));
    //byte low = lowByte(value2);
    //Serial.print(highByte(value2),HEX);
    //Serial.print((avg1| 0b1000000000000000  ),HEX);//| 0b1000000000000000 
    //Serial.print((avg2| 0b1000000000000000  ), HEX);//| 0b1000000000000000 
      
    }
    
    //Serial.println("1");

  // Stack(Deque)
  for(int i = 0; i < num_data; ++i)
  {
    for(int j = 1; j < size_of_stack; ++j)
    {
      stack[i][j] = stack[i][j-1];
    }
    stack[i][0] = value[i];
  }
    
    delayMicroseconds(10);

}
