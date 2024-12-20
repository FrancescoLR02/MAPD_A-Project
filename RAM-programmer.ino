 




//---------------------------------------------------------------------------------------------

//SEZIONE COMMENTI E VARIE MODIFICHE FATTE
/*
 * Output the address bits and outputEnable signal using shift registers.
*/

//Line = addressRAM + datoRAM concettualmente
//dove addressRAM è sempre della forma 0000abcd, ossia in esadecimale 0x0A (con A=abcd)
//dove datoRAM è fatto da 8 bit 0xMN con M=b1b2b3b4 e N=b5b6b7b8, M sarebbe l'istruzione del SAP, mentre N è il dato/indirizzo dopo l'istruzione

// Il programma sarà contenuto qui dentro, dove ogni dato è del tipo Line definito sopra
/* svarione di come avevo impostato prima le cose (leggilo comunque che ti chiarisce)
  tipo array di word perchè sono 8+8 bit, ossia 2 byte
  word data[] = { 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN, 0x0AMN };
  tutti gli address della RAM non specificati (ossia gli A) sarà meglio mettegli come datoRAM il NOP:0000 (no operation), quindi datoRAM=0 x 0000 0000 (i secondi 4 bit li mettiamo comunque a 0)
  word data[] = { 0x0000, 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800, 0x0900, 0x0a00, 0x0b00, 0x0c00, 0x0d00, 0x0e00, 0x0f00 }
*/

/* avevo provato a ragionare con le struct ma per ora non imprementato, non cacellare comunque
struct SAPline{
   byte RAMaddress;
   *char RAMdata; // non se arduino abbia le string, e se magari va definito con char[]
	
};/


#####! SECONDO ME QUESTA è E' LA SOLUZIONE MIGLIORE (QUELLA SCRITTA SOTTO)

/*l'alternativa che sarebbe anche migliore è usare un solo shift register per il dato ram (8 bit)
e usare 4 pin dell'arduino per dirgli l'address che nella funzione voide setup() sarebbe il command
for (int pin = 5; pin <= 8; pin += 1) {
    digitalWrite(pin, command & 1);
    command = command >> 1;
  }
*/


//---------------------------------------------------------------------------------------------

//byte data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//questo contiene solo l'istruzione, quella che sopra è chiamata MN, l'informazione dell'address della RAM e' già contenuta nella posizione in cui si trova nell'array l'istruzione (e.g. posizione 0 corrisponde all'addressRAM=0, posizione 15 è l'addressRAM=15 -i.e. l'ultimo-)

//---------------------------------------------------------------------------------------------


//Creo un dizionario a mano usando le struct:
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
  {"JC", 0b0111},
  {"JZ", 0b1000},
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


//Questa funzione prende in input l'istruzione (LDA, ADD, ...) e il dato (che esso sia l'address del registro o il dato in sè)
// e sputa fuori un byte che identifica, una volta settato l'address corretto della pcb nella quale mettere l'istruzione, 
// quello che dobbiamo mettere nel pezzetto a 8 bit (non ricordo come si chiama)
byte ConvertSAPLine(const char *Instr, int Value) {
  
  // Combina i 4 bit dell'istruzione con i 4 bit del dato
  byte result = (getValueForKey(Instr) << 4) | (Value & 0b1111);  // Shift a sinistra di 4 bit + aggiungi i 4 bit del dato
  
  return result;  // Ritorna il numero binario a 8 bit
}


/* DA IMPLEMENTARE
funzione ConvertSAPLine() che prende le stringhe che contengolo le istruzioni o i dati e questa funzione deve schiaffare
fuori in output un tipo byte (quindi 8 bit) in cui sono convertite le operazioni+address(o dato nel caso LDI) oppure i dati
quindi deve riconoscere se quello che si sta dando è un'operazione o un mero dato, saper convertire i dati
occhio che quando hai tipo "LDA 15" devi fare i primi 4 bit << 4 e poi | (or) con 15 in binario
  sarebbe da mettere degli if di controllo (o exception) se viene inserito un idirizzoRAM maggione di 15
  viene scritto un comando che non e' tra quelli contemplati
  se il numero dopo un'istruzione è maggione di 15
  se un dato (quelli alla fine del programma è superiore a 255)
*/


//---------------------------------------------------------------------------------------------

//This program does 6+10-5 (wooooooooow)
char data[16] = {
  data[0] = ConvertSAPLine("LDA", 15),
  data[1] = ConvertSAPLine("ADD", 14),
  data[2] = ConvertSAPLine("SUB", 13),
  data[3] = ConvertSAPLine("OUT", 0),
  data[4] = ConvertSAPLine("HLT", 0),
  //data[ 5 ]  = ConvertSAPLine(   "NOP 0" )
  //data[ 6 ]  = ConvertSAPLine(   "NOP 0" )
  //data[ 7 ]  = ConvertSAPLine(   "NOP 0" )
  //data[ 8 ]  = ConvertSAPLine(   "NOP 0" )
  //data[ 9 ]  = ConvertSAPLine(   "NOP 0" )
  //data[ 10 ]  = ConvertSAPLine(  "NOP 0" )
  //data[ 11 ]  = ConvertSAPLine(  "NOP 0" )
  //data[ 12 ]  = ConvertSAPLine(  "NOP 0" )
  data[13] = ConvertSAPLine("NOP", 5),
  data[14] = ConvertSAPLine("NOP", 10),
  data[15] = ConvertSAPLine("NOP", 6),
};

//---------------------------------------------------------------------------------------------

void setLine(int Line) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (Line >> 8)); //butta dentro nel shift register i primi 8 bit, ossia i quattro 0 e poi l'inidirizzo nella ram quello sopra chiamato A=abcd
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, Line); //Butta dentro 0xMN (perchè di default shiftOut prende gli ultimi 8 bit)
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}
// quanto sarebbe bella la vita se ci fosse il dictionary su arduino, ma invece pare proprio di no.


//---------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(57600);

  Serial.print(ConvertSAPLine("LDA", 15), BIN);

 /*
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH); //all'inizio meglio mettere a HIGH il pulsante che schiacciamo per scrivere nella RAM, se guardi nel datasheet dovrebbe funzionare che scrivi quando lo metti a low (perchè è un active low, di quelli negati in partenza)
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  */ 

 /*
  for (int command = 0; command < 15; command += 1) {
      setLine((command << 8) | data[command])
  //ho i miei seri dubbi che funzioni, perché prima dentro data[command] c'era sia l'addressRAM che datoRAM poi mi sono accorto che command non è altro che addressRAM, però setLine lavora con 8+8 bit
      digitalWrite(WRITE_EN, LOW); //qui è come quando schiacciavamo il pulsante 'write'
      delayMicroseconds(1000); //sarebbe come tenere premuto il tasto 'write' per 0.1 secondi, questo bisogna sperimentare
      digitalWrite(WRITE_EN, HIGH);
      delay(2); //meglio apsettare 2 secondi prima di scrivere la prossima riga del programma, almeno all'inizio sennò crasha tutto
  }
  */

}


//---------------------------------------------------------------------------------------------

void loop() {
  // put your main code here, to run repeatedly:
}
