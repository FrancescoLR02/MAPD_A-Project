#define SHIFT_DATA 2 // Serial data pin
#define SHIFT_CLK 3  // Clock pin for the shift register
#define SHIFT_LATCH 4 // Latch pin to send data simultaneously to the storage register
#define WRITE_EN 13  // Simulates pressing the write button
//---------------------------------------------------------------------------------------------

// Global variable to be written inside an array which will have 
// the info if the instuctions need to be written in the RAM address
bool RAMwrite = true;
bool writeFlags[16];

// Struct useful for the dictionary
struct KeyValue {
  const char* key; // Mnemonic instruction
  byte value;      // Corresponding binary value
};

// Dictionary with 16 entries
KeyValue dictionary[16] = {
  {"NOP", 0b0000},
  {"LDA", 0b0001},
  {"ADD", 0b0010},
  {"SUB", 0b0011},
  {"STA", 0b0100},
  {"LDI", 0b0101},
  {"JMP", 0b0110},
  {"JC" , 0b0111},
  {"JZ" , 0b1000},
  {"OUT", 0b1110},
  {"HLT", 0b1111}};

//This function is needed in order to map the binary number into the instruction and viceversa
// Function to find the value corresponding to a mnemonic in the dictionary
byte getValueForKey(const char* key) {
  for (int i = 0; i < 16; i++) {
    if (strcmp(dictionary[i].key, key) == 0) {
      return dictionary[i].value; // Return binary value if the key matches
    }
  }
  return 0; // Default return value if key is not found
}

// Function to combine the instruction's 4 bits with the operand's 4 bits
// or directly take an 8-bit integer input
byte ConvertSAPLine(const char *Instr = "", int Value = -1) {
  if (Instr != "" && Value != -1) {
      byte result = (getValueForKey(Instr) << 4) | (Value & 0b1111); // Combine instruction and operand
      RAMwrite = true;
      return result; // Return the 8-bit binary value
  } else {
      if (Value >= 0 && Value <= 255) {
        byte result = Value & 0b11111111;
        RAMwrite = true;
        return result; // Return the byte directly if within range
      }
      else {
        RAMwrite = false;
        return 0; // Default return value
      }
  }
  RAMwrite = false;
  return 0; // Default return value
}

//---------------------------------------------------------------------------------------------

// Declare and initialize the array data[] (for SAP program) with default values
/*byte data[16] = {
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1),
  ConvertSAPLine("", -1)
};*/

byte data[16];

// This program performs the calculation 6+5-7
void SumAndSub() {
  data[0]  = ConvertSAPLine("LDA", 15);    writeFlags[0]  = RAMwrite;
  data[1]  = ConvertSAPLine("ADD", 14);    writeFlags[1]  = RAMwrite;
  data[2]  = ConvertSAPLine("SUB", 13);    writeFlags[2]  = RAMwrite;
  data[3]  = ConvertSAPLine("OUT", 0);     writeFlags[3]  = RAMwrite;
  data[4]  = ConvertSAPLine("HLT", 0);     writeFlags[4]  = RAMwrite;
  data[5]  = ConvertSAPLine("", -1);       writeFlags[5]  = RAMwrite;
  data[6]  = ConvertSAPLine("", -1);       writeFlags[6]  = RAMwrite;
  data[7]  = ConvertSAPLine("", -1);       writeFlags[7]  = RAMwrite;
  data[8]  = ConvertSAPLine("", -1);       writeFlags[8]  = RAMwrite;
  data[9]  = ConvertSAPLine("", -1);       writeFlags[9]  = RAMwrite;
  data[10] = ConvertSAPLine("", -1);       writeFlags[10] = RAMwrite;
  data[11] = ConvertSAPLine("", -1);       writeFlags[11] = RAMwrite;
  data[12] = ConvertSAPLine("", -1);       writeFlags[12] = RAMwrite;
  data[13] = ConvertSAPLine("", 7);        writeFlags[13] = RAMwrite;
  data[14] = ConvertSAPLine("", 6);        writeFlags[14] = RAMwrite;
  data[15] = ConvertSAPLine("", 5);        writeFlags[15] = RAMwrite;
  return;
}

//This program counts from o to 255 with steps of 1
//and then it decrements from 255 to 0, and it restarts the loop again
//(essentialy tests carry flag and zero flag)
void Counting() {
  data[0]  = ConvertSAPLine("OUT", 0);      writeFlags[0]  = RAMwrite;
  data[1]  = ConvertSAPLine("ADD", 15);     writeFlags[1]  = RAMwrite;
  data[2]  = ConvertSAPLine("JC", 4);       writeFlags[2]  = RAMwrite;
  data[3]  = ConvertSAPLine("JMP", 0);      writeFlags[3]  = RAMwrite;
  data[4]  = ConvertSAPLine("SUB", 15);     writeFlags[4]  = RAMwrite;
  data[5]  = ConvertSAPLine("OUT", 0);      writeFlags[5]  = RAMwrite;
  data[6]  = ConvertSAPLine("JZ", 0);       writeFlags[6]  = RAMwrite;
  data[7]  = ConvertSAPLine("JMP", 4);      writeFlags[7]  = RAMwrite;
  data[8]  = ConvertSAPLine("", -1);        writeFlags[8]  = RAMwrite;
  data[9]  = ConvertSAPLine("", -1);        writeFlags[9]  = RAMwrite;
  data[10] = ConvertSAPLine("", -1);        writeFlags[10] = RAMwrite;
  data[11] = ConvertSAPLine("", -1);        writeFlags[11] = RAMwrite;
  data[12] = ConvertSAPLine("", -1);        writeFlags[12] = RAMwrite;
  data[13] = ConvertSAPLine("", -1);        writeFlags[13] = RAMwrite;
  data[14] = ConvertSAPLine("", -1);        writeFlags[14] = RAMwrite;
  data[15] = ConvertSAPLine("", 1);         writeFlags[15] = RAMwrite;
  return;
}

//---------------------------------------------------------------------------------------------

// Function to set a line on the shift register
void setLine(int Line) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, Line); // Output the data
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

//---------------------------------------------------------------------------------------------

//Setting the pins connected to the board as output pins. The first three are for the shift register
void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  //This pin is the writing enable pin setted as output
  pinMode(WRITE_EN, OUTPUT);
  //This pins are the one for the address line (4 bits address line)
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  //The write enable is setted initially on LOW for semplicity:
  //it is not controlled by the program, since it is not welded to the board (write enable activated by hand) 
  digitalWrite(WRITE_EN, LOW); 
  //digitalWrite(WRITE_EN, HIGH); //in case of welded write on the PCB, the 'write' works as an active low on the PCB  
  //Serial.begin(57600);

  //Two programs implemented
  SumAndSub();
  //Counting();

  for (int command = 0; command <= 15; command++) {
    if (writeFlags[command]) { // Only write if the flag is true
      int tempCommand = command;
      // Set the RAM address using Arduino pins from 9 (LSB) to 12 (MSB)
      for (int pin = 9; pin <= 12; pin++) {
        digitalWrite(pin, tempCommand & 1); // Write each bit of the address
        tempCommand = tempCommand >> 1;
      }

      delay(100); // Wait for 0.1 second
      setLine(data[command]); // Set the RAM data/instruction
      // Simulate pressing the write button on the RAM
      delay(700); // Wait for 0.7 second
      //When the write enable is HIGH we can press physically the button on the board to write the instruction in the RAM 
      //(if the cable was welded into the board we would have inverted the setting for the write enable:
      //First HIGH, then LOW and finally again LOW)
      digitalWrite(WRITE_EN, HIGH); // By hand
      //digitalWrite(WRITE_EN, LOW); // Write welded on the pcb
      delay(500); // Wait for 0.5 second
      digitalWrite(WRITE_EN, LOW); //By hand
      //digitalWrite(WRITE_EN, HIGH); // Write welded on the pcb
      delay(700); // Wait for 0.7 second
    }
  }
  // Set everything to 0 at the end of the cycle
  setLine(0);
  for (int pin = 9; pin <= 12; pin++) {
      digitalWrite(pin,LOW);
    }
}

//---------------------------------------------------------------------------------------------

void loop() {
  // No operation in the loop
}