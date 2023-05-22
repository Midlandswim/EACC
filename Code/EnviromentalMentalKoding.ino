/* 
  Name: Wilson Dhalwani
  Advisor: Mr. Joseph Barbetta 
  Date: 2/9/23
  Project: Engineers Against Climate Change 
*/

  /* 
  PLANNING PARKING LOT: 
    IDEA: Have two pumps per tube, with holes in the tube. One pump will go in reverse to pull liquid, one will go forwad to push liquid. Will help with pressure and speed.
    >> It is important to calibrate and set for each plant. I know it is dumb to have to do that, but sadly rn that is the best way. 
    >> EVentually I will look into taking and saving human input (egad, that means object orientedt programming; the bane of my existance). 
    >> This is an actual pain the in beeehinddddddd. 
  */

//libraries
#include <Wire.h>
#include <math.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_seesaw.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

//pins
#define swch 13
#define pump1 3
#define pump2 4
#define pump3 5
#define pump4 6
#define DS1 7
#define soilS 8

//iniates
OneWire OneWire(DS1);
DallasTemperature sensors(&OneWire);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

//otherStuff
Adafruit_seesaw ss;
int lux;    //lux
int prcnt;  //batterylife percent
int raw;    //raw read value from A0 or 17
int volt;   //inital volt conversion with decent degree of error
int volt2;  //secondary volt conversion with minimal degree of error


void setup() {
  //initialize for Arduino Uno R3
  Serial.begin(115200);   ///wait wait I think this has to be chnaged to 115200 if we go with feather (pls go with feather)
  sensors.begin();
  //pinMode Set
  //Output
  pinMode(pump1, OUTPUT);
  pinMode(pump2, OUTPUT);
  pinMode(pump3, OUTPUT);
  pinMode(pump4, OUTPUT);
  //Input
  pinMode(swch, INPUT);
  pinMode(DS1, INPUT);
  pinMode(soilS, INPUT);
  //SoilSensor
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! NoSeeSaw");
    while (1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  //TSL2591
  TSLconfig();
}

//BatteryLife for 3.7V 500 mAh Lipo 
void battRead(void) {
  //add calculations for battery life
  int raw = analogRead(A0);
  //Serial.println(raw);
  float volt = 0.0042 * raw - 0.0022;  //found using excell graphing; trendline
  //Serial.println(volt);
  float volt2 = 1.0007 * volt - 0.0802;  //improves calculation accuracy; used excel graphing; trendline
  //Serial.println(volt2);
  int prcnt = map(raw, 0, 1024, 0, 100);
  int battery = prcnt;
  if (volt2 < 3.0 || volt2 > 4.2) {  //debug check for volatge check
    Serial.println("DANGER! INSUFFICENT VOLTAGE!");
  } else {
    Serial.print("Battery Life: ");
    Serial.print(prcnt);
    Serial.println("%");
  }
}

//TSL2591 Horrendous Coding
void displaySensorDetails(void) {
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print(F("Sensor:       "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:   "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:    "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:    "));
  Serial.print(sensor.max_value);
  Serial.println(F(" lux"));
  Serial.print(F("Min Value:    "));
  Serial.print(sensor.min_value);
  Serial.println(F(" lux"));
  Serial.print(F("Resolution:   "));
  Serial.print(sensor.resolution, 4);
  Serial.println(F(" lux"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}
void configureSensor(void) {
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);  // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)Serial.println(F("------------------------------------"));
  Serial.print(F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch (gain) {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print(F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC);
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}
void TSLconfig(void) {
  Serial.println(F("Starting Adafruit TSL2591 Test!"));

  if (tsl.begin()) {
    Serial.println(F("Found a TSL2591 sensor"));
  } else {

    Serial.println(F("No sensor found ... check your wiring?"));
    while (1)
      ;
  }
  /* Display some basic information on this sensor */
  displaySensorDetails();
  /* Configure the sensor */
  configureSensor();
}
//Shows how to perform a basic read on visible, full spectrum or infrared light (returns raw 16-bit ADC values)
void simpleRead(void) {
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  Serial.print(F("[ "));
  Serial.print(millis());
  Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
}
//Show how to read IR and Full Spectrum at once and convert to lux
void advancedRead(void) {
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  // Serial.print(F("[ "));
  // Serial.print(millis());
  // Serial.print(F(" ms ] "));
  // Serial.print(F("IR: "));
  // Serial.print(ir);
  // Serial.print(F("  "));
  // Serial.print(F("Full: "));
  // Serial.print(full);
  // Serial.print(F("  "));
  // Serial.print(F("Visible: "));
  // Serial.print(full - ir);
  // Serial.print(F("  "));
  // Serial.print(F("Lux: "));
  // Serial.println(tsl.calculateLux(full, ir), 6);
  lux = tsl.calculateLux(full, ir);
}
//Performs a read using the Adafruit Unified Sensor API.
void unifiedSensorAPIRead(void) {
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);
  /* Display the results (light is measured in lux) */
  Serial.print(F("[ "));
  Serial.print(event.timestamp);
  Serial.print(F(" ms ] "));
  if ((event.light == 0) | (event.light > 4294966000.0) | (event.light < -4294966000.0)) {

    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    Serial.println(F("Invalid data (adjust gain or timing)"));
  } else {
    Serial.print(event.light);
    Serial.println(F(" lux"));
  }
}
void lightRead(void) {
  //simpleRead();
  advancedRead();
  //unifiedSensorAPIRead();
  Serial.print("Lux: ");
  Serial.println(lux);  //atleast 3K Lux, reccomended is 10k-15k (based off of 2 sec google search)
}

//PumpFunctions
void pumpOn() {
  digitalWrite(pump1, HIGH);
  digitalWrite(pump2, HIGH);
  digitalWrite(pump3, HIGH);
  digitalWrite(pump4, HIGH);
}
void pumpOFF() {
  digitalWrite(pump1, LOW);
  digitalWrite(pump2, LOW);
  digitalWrite(pump3, LOW);
  digitalWrite(pump4, LOW);
}

//Nonsense
void otherNonsense() {
  //tempSensor
  sensors.requestTemperatures();
  Serial.println("F Temp: ");
  Serial.print(sensors.getTempFByIndex(0));
  //TSL2591
  lightRead();
  //SoilSensor
  uint16_t capRead = ss.touchRead(0);
  int capReadMax = 0;  //TBD Calibration
  int capReadMin = 0;  //TBD Calibration
                       //Debug
  //Serial.print("Capacitive: ");
  //Serial.println(capRead);
  //Basic elif Funtion for human read moisture
  if (capRead < capReadMin) {
    Serial.println("Soil Moisture Insufficient");
  } else if (capRead > capReadMax) {
    Serial.println("Soil Mositure Excessive");
  } else {
    Serial.println("Soil Mositure Adequate");
  }
  //BattLife 
  battRead(); 
}

void loop() {
  //GateSwitchCode (idk what this called, I just wanted to sound cool)
  int swchState = digitalRead(swch);
  switch (swchState) {
    case LOW:
      //power off
      pumpOFF();
      Serial.println("DEVICE OFF");
      break;
    case HIGH:
      //Normal Code here
      pumpOn();
      otherNonsense();  //had variable and function issues in past, make sure not now
      break;
  }
  delay(2500);  //2.5 second delay (can increase or decrease; after testing about 60000 delay)
}
