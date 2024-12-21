 
#define SHIFT_DATA 2 //serial data
#define SHIFT_CLK 3 //è il clock interno dello shift register
#define SHIFT_LATCH 4 //questo è quello che manda tutti dati contemporanenamente allo store register
#define WRITE_EN 13 //questo sarà l'equivalente di schiacciare il tasto write

//---------------------------------------------------------------------------------------------

//byte data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//dafult program with all "NOP 0"

//---------------------------------------------------------------------------------------------

//struct usefull for the following dictionary
struct KeyValue {
  const char* key;
  byte value;
};

// Dizionario con 16 voci
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

//Questa funzione permette di ricercare il valore nella struttura appena creata, di modo che 
//quando viene chiamato una qualsiasi istruzione trova il valore corrispondente
byte getValueForKey(const char* key) {
  // Cerca la chiave nel dizionario
  for (int i = 0; i < 16; i++) {
    //Verifica se le chiavi sono uguali
    if (strcmp(dictionary[i].key, key) == 0) {
      //Se lo sono ritorna il valore binario
      return dictionary[i].value; 
    }
  }
}

//Function to output the byte (8bit) ready to be stored into the RAM (take as input mnemonics + operands)
byte ConvertSAPLine(const char *Instr, int Value) {
  // Combina i 4 bit dell'istruzione con i 4 bit del dato
  byte result = (getValueForKey(Instr) << 4) | (Value & 0b1111);  // Shift a sinistra di 4 bit + aggiungi i 4 bit del dato
  return result;  // Ritorna il numero binario a 8 bit
}

//---------------------------------------------------------------------------------------------

//PROGRAM TO IMPLEMENT
//This program does 6+10-5
byte data[16] = {
  data[0] = ConvertSAPLine("LDA", 15),
  data[1] = ConvertSAPLine("ADD", 14),
  data[2] = ConvertSAPLine("SUB", 13),
  data[3] = ConvertSAPLine("OUT", 0),
  data[4] = ConvertSAPLine("HLT", 0),
  data[5]  = ConvertSAPLine("NOP", 0),
  data[6]  = ConvertSAPLine("NOP", 0),
  data[7]  = ConvertSAPLine("NOP", 0),
  data[8]  = ConvertSAPLine("NOP", 0),
  data[9]  = ConvertSAPLine("NOP", 0),
  data[10]  = ConvertSAPLine("NOP", 0),
  data[11]  = ConvertSAPLine("NOP", 0),
  data[12]  = ConvertSAPLine("NOP", 0),
  data[13] = ConvertSAPLine("NOP", 5),
  data[14] = ConvertSAPLine("NOP", 10),
  data[15] = ConvertSAPLine("NOP", 6),
};

//---------------------------------------------------------------------------------------------

void setLine(int Line) {
 // shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (Line >> 8)); //previuous file version
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, Line);
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

//---------------------------------------------------------------------------------------------

void setup() {
//Serial.begin(57600);
//Serial.print(ConvertSAPLine("LDA", 15), BIN);
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH); //all'inizio meglio mettere a HIGH il pulsante che schiacciamo per scrivere nella RAM, se guardi nel datasheet dovrebbe funzionare che scrivi quando lo metti a low (perchè è un active low, di quelli negati in partenza)
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  for (int command = 0; command < 15; command += 1) {
      setLine(data[command]); //set the RAM data/instruction
      
      //set the RAM address using arduino pins from 9 (LSB) to 12 (MSB)

      int tempCommand = command;

      for (int pin = 9; pin <= 12; pin += 1) {
        digitalWrite(pin, tempCommand & 1); //MAYBE WE NEED TO CONVERT INT INTO BYTE
      tempCommand = tempCommand >> 1;
      }

      //to click the write button of the RAM
      digitalWrite(WRITE_EN, LOW);
      delay(1000); //500 milliseconds, to click on 'write' of RAM
      digitalWrite(WRITE_EN, HIGH);
      delay(2000); //2 seconds to wait for next instructions
  }
}

//---------------------------------------------------------------------------------------------

void loop() {
}