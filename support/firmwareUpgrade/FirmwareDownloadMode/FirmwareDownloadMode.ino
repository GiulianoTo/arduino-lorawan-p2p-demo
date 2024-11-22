//This will set the Arduino Nano to reset the module and don't touch the serial pins

#define RESETpin 12
#define TXpin 11 
#define RXpin 10
#define DebugSerial Serial

void board_reset(void) {
  pinMode(RESETpin, OUTPUT);
  digitalWrite(RESETpin, LOW);  // turn the pin low to Reset
  delay(400);
  digitalWrite(RESETpin, HIGH);  // then high to enable
  DebugSerial.println("Now Reset RAK");
  delay(400);
}

void setup() {
  DebugSerial.begin(9600);
  board_reset();
  pinMode(TXpin, INPUT);
  pinMode(RXpin, INPUT);
}

void loop() {
  DebugSerial.println("Ok now you can update firmware by external serial interface!");
  delay(5000);
}
