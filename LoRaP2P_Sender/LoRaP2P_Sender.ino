/********************************************************
 * This demo is tested with RUI firmware version 3.0.0.14.X on RAK811
 * Master Board Uart Receive buffer size at least 128 bytes. 
 ********************************************************/

#include "SoftwareSerial.h"
#define P2P_TRANFER_MODE_RECEIVER 1
#define P2P_TRANFER_MODE_SENDER 2
#define LORA_WORKING_MODE_P2P 1

int RESET_PIN = 12;
#define DEFAULT_SERIAL_TIMEOUT 1000
#define TXpin 11  // Set the virtual serial port pins
#define RXpin 10
#define DebugSerial Serial
SoftwareSerial ATSerial(RXpin, TXpin);  // Declare a virtual serial port
int msg_count;

void board_reset(bool showWelcomeMsg) {
  DebugSerial.println("Now Reset RAK811 Module...");
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);  // turn the pin low to Reset
  delay(400);
  digitalWrite(RESET_PIN, HIGH);  // then high to enable
  DebugSerial.println("Reset Done");

  // read welcome message
  String s;
  ATSerial.setTimeout(3000);
  s = ATSerial.readString();
  ATSerial.setTimeout(DEFAULT_SERIAL_TIMEOUT);
  if (showWelcomeMsg)
    DebugSerial.print(s);
}

void init_debug_serial(void) {
  DebugSerial.begin(9600);
  while (DebugSerial.available()) {
    DebugSerial.read();
  }
}

void init_at_serial(void) {
  ATSerial.begin(9600);
  while (ATSerial.available()) {
    ATSerial.read();
  }
}

bool get_firmware_version(void) {
  DebugSerial.println("Now read firmware version...");
  String s;
  ATSerial.println("at+version");
  s = ATSerial.readString();
  DebugSerial.println(s);
  if (s.indexOf("OK") != -1) {
    return true;
  } else {
    return false;
  }
}

bool set_p2p_configuration(void) {
  DebugSerial.println("Now set p2p configuration...");
  String s;
  ATSerial.println("at+set_config=lorap2p:869525000:12:0:1:8:20");
  s = ATSerial.readString();
  DebugSerial.println(s);
  if (s.indexOf("OK") != -1) {
    return true;
  } else {
    return false;
  }
}

bool set_lora_working_mode(int mode) {
  DebugSerial.println("Now set lora working mode...");
  String s;
  if (mode == 1)
    ATSerial.println("at+set_config=lora:work_mode:1");
  else
    ATSerial.println("at+set_config=lora:work_mode:0");
  s = ATSerial.readString();
  DebugSerial.println(s);
  if (s.indexOf("OK") != -1) {
    return true;
  } else {
    return false;
  }
}

bool set_p2p_transfer_mode(int mode) {
  DebugSerial.println("Now set p2p transfer mode...");
  String s;
  if (mode == 1)
    ATSerial.println("at+set_config=lorap2p:transfer_mode:1");
  else
    ATSerial.println("at+set_config=lorap2p:transfer_mode:2");
  s = ATSerial.readString();
  DebugSerial.println(s);
  if (s.indexOf("OK") != -1) {
    return true;
  } else {
    return false;
  }
}

/**
 * Converte una stringa in un vettore di coppie di caratteri esadecimali.
 * 
 * @param inputString La stringa da convertire.
 * @param hexVector Il vettore in cui memorizzare i valori esadecimali.
 */
void stringToHexVector(String inputString, char *hexVector) {
  int index = 0;
  for (int i = 0; i < inputString.length(); i++) {
    char c = inputString.charAt(i);
    sprintf(&hexVector[index], "%02X", c);  // Converte il carattere in esadecimale
    index += 2;                             // Avanza di 2 posizioni nel vettore
  }
}

bool send_p2p_data(String msg) {

  // Calcola la lunghezza necessaria per il vettore esadecimale
  int hexVectorLength = msg.length() * 2 + 1;  // 2 caratteri per byte + terminatore

  // Crea un vettore per contenere i valori esadecimali
  char hexVector[hexVectorLength];
  memset(hexVector, 0, hexVectorLength);  // Inizializza il vettore con zeri

  // Converti la stringa in esadecimale
  stringToHexVector(msg, hexVector);

  String command = "at+send=lorap2p:";
  DebugSerial.print(command);
  DebugSerial.println(hexVector);
  ATSerial.print(command);
  ATSerial.println(hexVector);
  String s = ATSerial.readString();
  DebugSerial.println(s);
  if (s.indexOf("OK") != -1) {
    return true;
  } else {
    return false;
  }
}

void generate_p2p_message(void) {
  String s;
  s = "IIS Lonigo message number ";
  s.concat(msg_count++);
  if (!send_p2p_data(s))
    DebugSerial.println("Error: unable to send p2p data !! ");

  DebugSerial.println(s);
}

void setup() {

  // init serials
  init_debug_serial();
  init_at_serial();

  // check if rak811 module is connected
  board_reset(false);
  get_firmware_version();  // dummy request to flush rak811 buffer
  if (!get_firmware_version()) {
    DebugSerial.println("Fatal: unable to get firmware version !! ");
    while (1)
      ;
  }

  // since set working mode command will cause restart module
  // it is not possible to check it
  set_lora_working_mode(LORA_WORKING_MODE_P2P);
  delay(2000);
  board_reset(true);

  // dummy request to flush rak811 buffer
  get_firmware_version();
  if (!get_firmware_version()) {
    DebugSerial.println("Fatal: unable to get firmware version !! ");
    while (1)
      ;
  }

  if (!set_p2p_configuration()) {
    DebugSerial.println("Fatal: unable to set p2p configuration !! ");
    while (1)
      ;
  }

  if (!set_p2p_transfer_mode(P2P_TRANFER_MODE_SENDER)) {
    DebugSerial.println("Fatal: unable to set p2p transfer mode !! ");
    while (1)
      ;
  }
}

void loop() {
  generate_p2p_message();
  delay(5000);
}