#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const int analogInput = 0; // analogue pin
const int analogRefVolt = 2; // analogue pin
const int rangeButtonPin = 2; // digital pin
const int serialButtonPin = 3; //digital pin
const int ledPin = 13;

float vout = 0.0;
float vin = 0.0;
float vref = 5.0;
float R1 = 99100.0; //actual resistance of R1 (100k)
float R2 = 99250.0; // actual resistance of R2 (100k)
float R3 = 992000.0; // actual resistance of R3 (1M)

int value = 0;
int vrefin = 5;
int range = 99;

volatile unsigned long lastMillis = 0;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup(){

	// Setup button modes  
	pinMode(rangeButtonPin, INPUT);
	pinMode(serialButtonPin, INPUT);
	pinMode(ledPin, OUTPUT);
	pinMode(analogInput, INPUT);
	pinMode(analogRefVolt, INPUT);
	digitalWrite(ledPin, LOW);
	attachInterrupt(digitalPinToInterrupt(rangeButtonPin), rangeButtonPressed, RISING);

	// Initialize the LCD panel
	lcd.init();
	lcd.backlight();

	// Start Serial interface in case needed
	Serial.begin(9600);
}

void loop(){

	// Read the value at analog input
	switch (range) {
	case 5:
		value = analogRead(analogInput);
		vout = (value * vref) / 1024.0;
		vin = vout;
		break;
	case 10:
		value = analogRead(analogInput);
		vout = (value * vref) / 1024.0;
		vin = vout / (R1/(R1+R2)); 
		break;
	case 55:
		value = analogRead(analogInput);
		vout = (value * vref) / 1024.0;
		vin = vout / (R1/(R1+R2+R3)); 
		break;
	case 99: // SPECIAL case to set reference voltage based on measured VCC
		vrefin = analogRead(analogRefVolt);
		vref = (vrefin * 5.5) / 1024.0;
		break;    
   }

	// Print on LCD
	if (range != 99) {
		lcd.setCursor(0,0);
		lcd.print("RANGE DC ");
		lcd.setCursor(9,0);
		lcd.print(range);
		lcd.print(" V PC ");
		lcd.setCursor(0,1);
		lcd.print("INPUT V= ");
		lcd.print(vin);
		lcd.print("   ");
	} else {
		lcd.setCursor(0,0);
		lcd.print("SET VCC VALUE   ");
		lcd.setCursor(0,1);
		lcd.print("VCC V= ");
		lcd.print(vref);
		lcd.print("     ");
	}

	// Send via Serial
	Serial.println(vin);

	delay(50);
}

void rangeButtonPressed() {

	long currentTime = millis();  

	if ((currentTime - lastMillis) > 300) {
		lastMillis = currentTime; 
		switch (range) {
			case 99:
				range = 10;
				digitalWrite(ledPin, LOW);
				break;
			case 10:
				range = 99;
				digitalWrite(ledPin, LOW);
				break;
    	}
  	}
}

