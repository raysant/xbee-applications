
  o-----------------------o
  | Serial Bypass Example |
  o-----------------------o

  Compatible platforms
  --------------------
  * XBee ZB (S2B) 32KB
  * XBee ZB (S2B) 128KB
  * XBee ZB (S2C) 32KB
  * XBee 865/868LP DigiMesh (S8) 32KB
  * XBee-PRO 900HP DigiMesh (S3B) 32KB
  * XBee ZB (S2CTH) 32KB (current)

  Introduction
  ------------
  Example that implements a bypass of the Freescale CPU inside the programmable
  XBee module. All serial data received on the serial port connected to
  the PC is forwarded to the serial port connected to the radio and vice versa.
  This way the user can easily access the radio from the PC using tools like 
  X-CTU as if the Programmable XBee module were a non programmable XBee module.

  Requirements
  ------------
  To run this example you will need:
    * One programmable XBee module to host the application.
    * One PE-Micro debugger to debug the application.
    * One XBee Interface Board.
    * A serial console terminal to view the application wellcome and interact
      with the radio. It's recommended to use X-CTU software application as 
      terminal so radio AT parameters and firmware can easily be read and 
      updated.

  Example setup
  -------------
  1) Make sure the hardware is set up correctly:
       a) The XBee module into the headers on the XBee USB interface board.
       b) The debugger is connected to the interface board by its ribbon 
          cable.
       c) Both the XBIB device and the debugger are connected to your PC via 
          USB.
     
  2) This demo requires a serial console terminal in order to interact with 
     the XBee radio chip. You can use the embedded terminal included with the 
     XBee extensions or any other serial terminal application.
     
     Configure the terminal and open a serial connection with the XBee module.
     
     The baud rate of the serial console must be 115200, as the application 
     will configure the UART of the XBee module with that baud rate.
     
     Refer to the topic "Terminal" of the "XBee Extensions User's Guide" for 
     more information.
     
     It's recommended to use X-CTU software application as terminal so radio 
     AT parameters and firmware can easily be read and updated.
	 
  3) When this example is used to setup an auxiliary Programmable XBee module 
     so applications running on the PC can access the DigiMesh networks,
     it may be necessary to configure 'Network ID (ID)' in the "XBee Configuration" 
     component in the Smart Editor.

  Running the example
  -------------------
  The example is already configured, so all you need to do is to compile and 
  launch the application.
  
  As soon as the application starts, it prints some welcome and usage 
  messages and then enters in bypass mode.
  Any character typed will be forwarded to the radio.
  - If using an standard serial terminal you can put the radio in command mode
    by sending three '+' characters: '+++'. The radio should answer with an 'OK'.
    Then you can communicate with the radio using AT commands. You can check the 
    radio Firmware version by typing 'atVR'. The radio should answer with four 
    characters that should match the 'FW version' printed in the welcome message.
	This method doesn't work for a S2B module running radio API mode. So you need 
	to use next method.
  - If using X-CTU terminal, after selecting the correct baudrate (115200), you 
    can verify the connection by pressing the "Test / Query" button.
    Then you can read/write radio configuration and Firmware.
