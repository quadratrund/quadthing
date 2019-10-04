# QuadThing
This program allows to control the GPIO pins of a NodeMCU via HTTPS requests. It also hosts a simple web interface.

## Features
* **Configuration via web inerface**
* **Simple passowrd check**: You can optionally set a password for access via HTTPS. **Note:** The username can be any string. It is not checked.
* **Feedback Mode:** Switch an output depending on an input state. When you request to swith an output on, it will be toggled, if the corresponding input is low. Nothing will happen, if the input already is high. The same for switching off: If the input already is low, nothing will happen. The output is toggled otherwise. **Note:** If you are not using the feedback mode, on means low and off means high.
* **Bundle Mode:** Keep two outputs at the same state.
* **Auto-Off after timeout:** Switch off all outputs after up to 255 minutes after the last user input. (Can be disabled by setting the timeout to 0)

## Required Hardware
This software is designed to run on an ESP8266 microcontroller. It was developed and tested with a NodeMCU v3.

You may also want use this together with some Relais. An DC 5V Relay Module with Optocoupler should work in most cases.

## How To Start
* Clone or download the repository (duh)
* Install the Arduino IDE from arduino.cc
* Install the ESP8266 software as described in https://github.com/esp8266/Arduino#installing-with-boards-manager
* In the IDE go to Tools > Manage Libraries... and install the librariy base64 (Version 1.1.1) by Densaugeo
* Open and compile the program and upload it to your microcontroller
* Connect to the WiFi QuadThing, which should be served by the ESP8266.
* Go to https://192.168.4.1 in a web browser

## Api
The following paths are designed for automated HTTPS GET requests:
### /api/states
Returns information about the states of all inputs and outputs.
### /api/switch
Changes the state of one or more outputs.
**Parameters:** id0, id1, id2, ... and set0, set1, set2, ... (If you use id and set with some number, all parameters with lower numbers must also be given.)
* **idX** is the zero-based output pin id (0..7)
* **setX** can be one of the following values:
  * **high** to set the output to high. This bypasses the feedback mode.
  * **low** to set the output to low. This bypasses the feedback mode.
  * **toggle** to toggle the output.
  * **on** same as low, if feedback mode is off. See Description of Feedback Mode in [Features](#features) otherwise.
  * **off** same as high, if feedback mode is off. See Description of Feedback Mode in [Features](#features) otherwise.
