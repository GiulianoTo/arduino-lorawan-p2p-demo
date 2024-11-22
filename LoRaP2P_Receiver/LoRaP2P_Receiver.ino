/********************************************************
 * This demo is tested with RUI firmware version 3.0.0.14.X on RAK811
 * Master Board Uart Receive buffer size at least 128 bytes. 
 ********************************************************/

#include "SoftwareSerial.h"

#define P2P_TRANFER_MODE_RECEIVER 1
#define P2P_TRANFER_MODE_SENDER 2
#define LORA_WORKING_MODE_P2P 1

#define TXpin 11  // Set the virtual serial port pins
#define RXpin 10
#define RESET_PIN 12

#define DEFAULT_SERIAL_TIMEOUT 1000

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

/**
 * Converte una stringa di valori esadecimali in una stringa originale.
 * 
 * @param hexString La stringa esadecimale da convertire.
 * @param originalString Il vettore in cui memorizzare la stringa originale.
 */
void hexToString(String hexString, char *originalString) {
  int index = 0;
  for (int i = 0; i < hexString.length(); i += 2) {
    // Estrai due caratteri esadecimali
    String hexPair = hexString.substring(i, i + 2);

    // Converte i due caratteri esadecimali in un byte
    char c = (char)strtol(hexPair.c_str(), NULL, 16);

    // Memorizza il byte nella stringa originale
    originalString[index++] = c;
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
  //DebugSerial.print(command);
  //DebugSerial.println(hexVector);
  ATSerial.print(command);
  ATSerial.println(hexVector);
  String s = ATSerial.readString();
  //DebugSerial.println(s);
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

void receive_p2p_message(void) {
  ATSerial.setTimeout(3000);
  String s = ATSerial.readStringUntil('\l');
  s.trim();
  int i = s.indexOf(":");
  if (i > 0) {
    //DebugSerial.println(s);
    s.remove(0, i + 1);
    //DebugSerial.println(s);

    // Calcola la lunghezza necessaria per la stringa originale
    int originalLength = s.length() / 2 + 1;  // Ogni 2 caratteri esadecimali corrispondono a 1 byte

    // Crea un array per contenere la stringa originale
    char originalString[originalLength];
    memset(originalString, 0, originalLength);  // Inizializza il vettore con zeri

    // Converti la stringa esadecimale in stringa originale
    hexToString(s, originalString);

    DebugSerial.println(originalString);
  }
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

  if (!set_p2p_transfer_mode(P2P_TRANFER_MODE_RECEIVER)) {
    DebugSerial.println("Fatal: unable to set p2p transfer mode !! ");
    while (1)
      ;
  }
}

void loop() {
  receive_p2p_message();
}