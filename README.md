


![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/EasyPIR-blackCr.png?raw=true)
![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/PIRpcb3cr.jpg?raw=true)
![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/EasyPIR-nrf-1cr.jpg?raw=true)


**The easyPIRmultisensorsBox is a low cost wireless Arduino IDE compatible (the Atmel ATMega328P 8MHz) microcontroller with RFM 69 CW or Nordic Semiconductor’s NRF 24L01+ radio on board and few other nice additions.** 
------------------------------------------------------------------------

Best sutable for Home Automation, IOT.  You may think of it as Arduino Pro Mini plus all the items in the picture below:

![](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/replceA.jpg?raw=true)

## Specification: ##

 - Board dimensions 31mm x 41mm.
 - Enclosure dimensions mm x mm
 - Powered by two CR2032 batteries in series with very high efficient power converter (3.5-10V). JST connector for external battery.
 - Wide temperature range. Tested -20 +40
 - PIR sensor S16-L201D 
 - Temperature and humidity sensor Si7021 
 - High Accuracy Temperature Sensor ±0.4 °C (max), –10 to 85 °C
 - Precision Relative Humidity Sensor ± 3% RH (max), 0–80% RH
 - Light sensor BH1750,  spectral responsibility is approximately human eye response.
 - Authentication security - Atmel ATSHA204A Crypto Authentication Chip
 - External JDEC EPROM
 - Dualoptiboot bootloader. Implements over the air (OTA) firmware update ability
 - Multiradio board. Rfm69cw and nrf24l01+ are supported.
 - FTDI  header for programming
 - Reverse polarity protection.


**Arduino IDE Settings**

![Arduino IDE Settings](https://github.com/EasySensors/ButtonSizeNode/blob/master/pics/IDEsettings.jpg?raw=true)


**programming FTDI adapter connection**

![enter image description here](https://github.com/EasySensors/ButtonSizeNode/blob/master/pics/FTDIvcc5-3.jpg?raw=true)


How to use it as home automation (IOT) node controller
------------------------------------------------------


easyPIRmultisensorsBox.ino is the Arduino example sketch using [MySensors](https://www.mysensors.org/) API. 

- **Controller Setup.**  
Burn the easyPIRmultisensorsBox.ino sketch into the board and it will became  one of the MySensors home automation network Sensor's Node reporting Motion  (PIR), Temperature , Humidity and Vsial light in Luxes to a smart-home controller. 
To create the home automation network you need smart-home controller and at least two Nodes one as a Sensor node and the other one as the “Gateway Serial” connected to a smart-home controller. I personally love [Domoticz](https://domoticz.com/) as a smarthome controller. Please check this [HowTo](https://github.com/EasySensors/ButtonSizeNode/blob/master/DomoticzInstallMySensors.md) to install Domoticz.

- **No Controller setup.** 
However, for no-controller setup, as example, you can use 3 nodes - first node as the “Gateway Serial”, second node as a Relay and last one as a PIR  sensor for that a Relay. No controller needed then, keep the Switch and the Relay on the same address and the switch will operate the relay. 


Things worth mentioning about the  [MySensors](https://www.mysensors.org/) Arduino sketch: 


Code |	Description
------------|--------------
Light meter BH1750 Library by Christopher Laws | [MySensorsArduinoExamples BH1750](https://github.com/mysensors/MySensorsArduinoExamples/tree/master/libraries/BH1750)
Temperature and humidity sensor  Si7021 SparkFun Library | [SparkFun_Si7021_Breakout_Library](https://github.com/sparkfun/Si7021_Breakout/tree/master/Libraries/Arduino/Si7021/src)
#define MY_RADIO_RFM69<br>#define MY_RFM69_FREQUENCY   RF69_433MHZ| The easyPIRmultisensorsBox can use Hope RF RFM69CW or Nordic Semiconductor's NRF 24L01+ radios. <br> Define which radio we use – here is RFM 69<br>with frequency 433 MHZ and it is HW<br>type – one of the most powerful RFM 69 radios.<br> 
#define MY_RADIO_NRF24|	Define which radio we use – here is Nordic Semiconductor's NRF 24L01+ <br> 
#define MY_NODE_ID 0xE0 | Define Node address (0xE0 here). I prefer to use static addresses<br> and in Hexadecimal since it is easier to identify the node<br> address in  [Domoticz](https://domoticz.com/) devices list after it<br> will be discovered by controller ( [Domoticz](https://domoticz.com/)).<br> However, you can use AUTO instead of the hardcoded number<br> (like 0xE0) though.  [Domoticz](https://domoticz.com/) will automatically assign node ID then.
#define MY_OTA_FIRMWARE_FEATURE<br>#define MY_OTA_FLASH_JDECID 0x2020 | Define OTA feature. OTA stands for “Over The Air firmware updates”.<br> If your node does not utilize Sleep mode you can send new “firmware”<br> (compiled sketch binary) by air. [Here is the link on how to do it.](https://www.mysensors.org/about/ota)<br> Skip to the step "How to upload a new sketch just with OTA" as all initial <br> steps have been completed in the easyPIRmultisensorsBox. <br>For OTA we use JDEC Flash chip where the node stores<br> new firmware and once it has been  received and checksum (CRC)<br> is correct it reboots and flashes your new<br> code into the node controller.  0x2020 "Erase type"  <br>defined here for JDEC Flash chip . 
#define MY_SIGNING_ATSHA204 <br>#define  MY_SIGNING_REQUEST_SIGNATURES | Define if you like to use Crypto Authentication to secure your nodes<br> from intruders or interference. After that, you have to “personalize”<br> all the nodes, which have those, defines enabled.<br> [**How to “personalize” nodes with encryption key**](https://github.com/EasySensors/ButtonSizeNode/blob/master/SecurityPersonalizationHowTo.md).<br> You need both defines in the nodes you need to protect.<br> The Gateway Serial could be with only one of those<br> defines enabled - #define MY_SIGNING_ATSHA204

Connect the Node to FTDI USB adaptor, Select Pro Mini 8MHz board in Arduino IDE and upload the easyPIRmultisensorsBox.ino sketch.

**Done**


The board designed by  [Koresh](https://www.openhardware.io/user/143/projects/Koresh)

![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/EasyPIR-black.png?raw=true)
![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/EasyPIR-nrf-1.jpg?raw=true)
![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/PIRpcb3.jpg?raw=true)
![enter image description here](https://github.com/EasySensors/easyPIRmultisensorsBox/blob/master/pics/pirRFM69cw.jpg?raw=true)

