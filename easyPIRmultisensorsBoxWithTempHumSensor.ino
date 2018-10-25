/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
**/

// Enable debug prints to serial monitor
#define MY_DEBUG

#include <avr/wdt.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Enable and select radio type attached
#define MY_RADIO_RFM69

// if you use MySensors 2.0 use this style 
//#define MY_RFM69_FREQUENCY   RF69_433MHZ
//#define MY_RFM69_FREQUENCY   RF69_868MHZ
//#define MY_RFM69_FREQUENCY   RF69_915MHZ


#define MY_RFM69_FREQUENCY   RFM69_433MHZ
//#define MY_RFM69_FREQUENCY   RFM69_868MHZ

//#define MY_RADIO_NRF24

// Comment it out for Auto Node ID #
#define MY_NODE_ID 0x90


// Avoid battery drain if Gateway disconnected and the node sends more than MY_TRANSPORT_STATE_RETRIES times message.
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#define MY_PARENT_NODE_ID 0


//Enable OTA feature
#define MY_OTA_FIRMWARE_FEATURE
#define MY_OTA_FLASH_JDECID 0x0//0x2020

//Enable Crypto Authentication to secure the node
//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES

#include <Wire.h>

// Written by Christopher Laws, March, 2013.
// https://github.com/claws/BH1750
#include <BH1750.h>
BH1750 lightMeter;

#include "SparkFun_Si7021_Breakout_Library.h"
//Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barrometric sensor
Weather sensor;

#include <MySensors.h>

// Redefining write codes for JDEC FLASH used in the node
// These two defines should always be after #include <MySensors.h> declaration
#define SPIFLASH_BLOCKERASE_32K   0xD8
#define SPIFLASH_CHIPERASE        0x60

#include <stdlib.h>


#define BUTTONS_INTERUPT_PIN 3
#define RED_LED_PIN 6
#define YELLOW_LED_PIN 5



// Assign numbers for all sensors we will report to gateway\controller (they will be created as child devices)
#define SW_sensor 1
#define HUM_sensor 2
#define TEMP_sensor 3
#define VIS_sensor 4


// Create MyMessage Instance for sending readins from sensors to gateway\controller (they will be created as child devices)

MyMessage msg_sw(SW_sensor, V_LIGHT);
MyMessage msg_hum(HUM_sensor, V_HUM);
MyMessage msg_temp(TEMP_sensor, V_TEMP);
MyMessage msg_vis(VIS_sensor, V_LIGHT_LEVEL);

unsigned long wdiDelay2  = 0;

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point

static int32_t oldLux = 0, lux;
static int16_t oldHumdty = 0, humdty;
static int16_t oldTemp = 0, temp;

void swarm_report()
{
  static int oldBatteryPcnt = 0;
  char humiditySi7021[10];
  char tempSi7021[10];
  char visualLight[10];


  lightMeter.begin(BH1750::ONE_TIME_LOW_RES_MODE); // need for correct wake up
  lux = lightMeter.readLightLevel();// Get Lux value
  // dtostrf(); converts float into string
  long d = (long)(lux - oldLux);
  Serial.print("abs(lux - oldLux)="); Serial.print(abs(d)); Serial.print(" lux ="); Serial.print(lux); Serial.print(" oldLux ="); Serial.println(oldLux); 
  dtostrf(lux,5,0,visualLight);
  if ( abs(d) > 50 ) {
    // this wait(); is 2.0 and up RFM69 specific. Hope to get rid of it soon
    wait(100);
    send(msg_vis.set(visualLight), true);  // Send LIGHT BH1750     sensor readings
    oldLux = lux;
  }

   
  // Measure Relative Humidity from the Si7021
  humdty = sensor.getRH();
  dtostrf(humdty,0,2,humiditySi7021);  
  if (humdty != oldHumdty) {
    wait(100);
    send(msg_hum.set(humiditySi7021), true); // Send humiditySi7021     sensor readings
    oldHumdty = humdty; 
  }

  
  // Measure Temperature from the Si7021
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTemp() instead with readTemp()
  temp = sensor.getTemp();
  dtostrf(temp,0,2,tempSi7021);
  if (temp != oldTemp) {
    wait(100);
    send(msg_temp.set(tempSi7021), true); // Send tempSi7021 temp sensor readings
    oldTemp = temp;
  }

  // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  /* 1M, 470K divider across batteries
   * 610 ~ 100 % is close to 6.1 V
   * 400 ~ 0 % is close to 4V
   */
  int batteryPcnt = (sensorValue - 400)  / 2;
  
  batteryPcnt = batteryPcnt > 0 ? batteryPcnt:0; // Cut down negative values. Just in case the battery goes below 4V and the node still working. 
  batteryPcnt = batteryPcnt < 100 ? batteryPcnt:100; // Cut down more than "100%" values. In case of ADC fluctuations. 

  if (oldBatteryPcnt != batteryPcnt ) {
    wait(100);
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }
}

void before() {
    //No need watch dog enabled in case of battery power.
    //wdt_enable(WDTO_4S);
    wdt_disable();
  
    
    #ifdef  MY_RADIO_RFM69
      /*  RFM reset pin is 9
       *  A manual reset of the RFM69HCW\CW is possible even for applications in which VDD cannot be physically disconnected.
       *  Pin RESET should be pulled high for a hundred microseconds, and then released. The user should then wait for 5 ms
       *  before using the module.
       */
      pinMode(9, OUTPUT);
      //reset RFM module
      digitalWrite(9, 1);
      delay(1);
      // set Pin 9 to high impedance
      pinMode(9, INPUT);
      delay(10);
    #endif
    
    
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN,0);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    digitalWrite(YELLOW_LED_PIN,0);
}

void setup() {
}

void presentation() 
{  
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("PIR node", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  present(SW_sensor, V_LIGHT);
  present(HUM_sensor, S_HUM);
  present(TEMP_sensor, S_TEMP);
  present(VIS_sensor, S_LIGHT_LEVEL);
}

unsigned long wdiDelay  = 0;

void loop()
{
  digitalWrite(RED_LED_PIN,1);
  wait(100);
  digitalWrite(RED_LED_PIN,0);
  
  noInterrupts();
  _flash.initialize(); 
  _flash.wakeup();
  interrupts();
  
  //No need watch dog in case of battery power.
  //wdt_reset();


  //If NACK received retry to send as many as uint8_t rty; times
  uint8_t retry = 5, waitTime = 50;
  static boolean  value = false;
  if ( digitalRead(BUTTONS_INTERUPT_PIN) == HIGH )  {
    while (!send(msg_sw.set(!value), true) && (retry > 0))  {
      // send did not go through, try  "uint8_t retry = 5" more times
      wait(waitTime); retry--; waitTime+=10;
    }
  }

  if (!retry) {
    for  (int i = 5;i;i--){
      // failure to get ACK from controller - 4 Blinks in Yellow
      digitalWrite(YELLOW_LED_PIN,1);
      wait(30);
      digitalWrite(YELLOW_LED_PIN,0);
      wait(30);
      }
    }
  
  value = !value;

  swarm_report();      

 // Go sleep for some milliseconds
  noInterrupts();
  _flash.sleep();
  interrupts();
  
  sleep(BUTTONS_INTERUPT_PIN - 2, RISING,0);  // 300000
}

