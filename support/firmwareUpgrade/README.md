# firmwareUpgrade

## firmwareUpgrade from v2

Unfortunately the purchased shields are equipped with firmware version 2.0.3. This firmware is obsolete. It must be updated to version 3.

Below are the instructions for the update from firmware 2 to 3:

Since the serial of the RAK811 module is not connected directly to the PC, I decided to install a sketct in Arduino that leaves pins 10 and 11 as input. Then I connect a serial converter to pins 10 and 11 of the shield connector (the connector for Arduino UNO) and so the update is performed via PC.

- Download the sketch "FirmwareDownloadMode" to Arduino nano Every
- Install Java Runtime if needed "jre-8u431-windows-x64.exe"
- Install SetupSTM32CubeProgrammer-2.1.0.exe
- Power up the shield via Arduino while a red wire put high level to RAK811 pin 17 BOOT0
- Start STM32CubeProgrammer connect (see smt.png)
- Press Erase button
- Open RAK811_v3.0.0.14.H_20200810.hex , then press Download
- Restart shield
- Open RAK_SERIAL_PORT_TOOL_V1.2.1 , at 115200 test "at+version" command. 

Now RAK811 default baud rate is 115200. if you use a Nano every Arduino board there is a problem communicating at speed 115200
on softwareSerial port. So you must change baud rate with PC serial com. 

Send the follow command with 115200 baud and check if after a reboot baud is 9600.

at+set_config=device:uart:1:9600

All is done

## firmwareUpgrade from v3

If you have to upgrade a sheld that has been a V3 firmware you can use the RAK_Device_Firmware_Upgrade_Tool_v1.4. 

Below are the instructions for the update from firmware:

Since the serial of the RAK811 module is not connected directly to the PC, I decided to install a sketct in Arduino that leaves pins 10 and 11 as input. Then I connect a serial converter to pins 10 and 11 of the shield connector (the connector for Arduino UNO) and so the update is performed via PC.

- Download the sketch "FirmwareDownloadMode" to Arduino nano Every
- Power up the shield via Arduino 
- Start RAK_Device_Firmware_Upgrade_Tool
- Try two times because the first connection set the module in boot mode and so it communicate at 115200 and not to 9600. 
- Upgrade the firmware
- Restart shield
- In my case the module keep the original baudrate (9600)
- Open RAK_SERIAL_PORT_TOOL_V1.2.1 , at 115200 test "at+version" command. 

 




