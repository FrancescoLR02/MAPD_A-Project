#define SHIFT_DATA 2 // Serial data pin
#define SHIFT_CLK 3  // Clock pin for the shift register
#define SHIFT_LATCH 4 // Latch pin to send data simultaneously to the storage register
#define WRITE_EN 13  // Simulates pressing the write button

//---------------------------------------------------------------------------------------------

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
  {"JC",  0b0111},
  {"JZ",  0b1000},
  {"OUT", 0b1110},
  {"HLT", 0b1111}
};

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
byte ConvertSAPLine(const char *Instr = nullptr, int Value) {
  if (Instr != nullptr) {
    byte result = (getValueForKey(Instr) << 4) | (Value & 0b1111); // Combine instruction and operand
    return result; // Return the 8-bit binary value
  } 
  else {
    byte result = Value & 0b11111111
    return result;
  }
  return 0; // Default return value
}
/*
// Function to combine the instruction's 4 bits with the operand's 4 bits
byte ConvertSAPLine(const char *Instr, int Value) {
  byte result = (getValueForKey(Instr) << 4) | (Value & 0b1111); // Combine instruction and operand
  return result; // Return the 8-bit binary value
}
*/

//---------------------------------------------------------------------------------------------

// This program performs the calculation 6 + 10 - 5
byte data[16] = {
  data[0] = ConvertSAPLine(   "LDA", 15 ), // Load 6 (stored in RAM address 15)
  data[1] = ConvertSAPLine(   "ADD", 14 ), // Add 10 (stored in RAM address 14)
  data[2] = ConvertSAPLine(   "SUB", 13 ), // Subtract 5 (stored in RAM address 13)
  data[3] = ConvertSAPLine(   "OUT", 0  ), // Output the result
  data[4] = ConvertSAPLine(   "HLT", 0  ), // Halt the program
  data[5]  = ConvertSAPLine(  "NOP", 0  ), // No operation (remaining instructions are NOPs)
  data[6]  = ConvertSAPLine(   255 ),
  data[7]  = ConvertSAPLine(  "NOP", 0  ),
  data[8]  = ConvertSAPLine(  "NOP", 0  ),
  data[9]  = ConvertSAPLine(  "NOP", 0  ),
  data[10]  = ConvertSAPLine( "NOP", 0  ),
  data[11]  = ConvertSAPLine( "NOP", 0  ),
  data[12]  = ConvertSAPLine( "NOP", 0  ),
  data[13] = ConvertSAPLine(  "NOP", 5  ),
  data[14] = ConvertSAPLine(  "NOP", 10 ),
  data[15] = ConvertSAPLine(  "NOP", 6  ),
};

//---------------------------------------------------------------------------------------------

// Function to set a line on the shift register
void setLine(int Line) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, Line); // Output the data
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

//---------------------------------------------------------------------------------------------

void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);

  digitalWrite(WRITE_EN, HIGH); // Set write button to HIGH initially (active low)
  Serial.begin(57600);

  for (int command = 0; command <= 15; command++) {
    setLine(data[command]); // Set the RAM data/instruction

    int tempCommand = command;

    // Set the RAM address using Arduino pins from 9 (LSB) to 12 (MSB)
    for (int pin = 9; pin <= 12; pin++) {
      digitalWrite(pin, tempCommand & 1); // Write each bit of the address
      tempCommand = tempCommand >> 1;
    }

    // Simulate pressing the write button on the RAM
    digitalWrite(WRITE_EN, LOW);
    delay(100); // Wait for 1 second
    digitalWrite(WRITE_EN, HIGH);
    delay(1000); // Wait for 2 seconds
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
