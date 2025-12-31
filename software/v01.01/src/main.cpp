/*
*     Hardware: WROOM32, ADS1015, SSD1306 OLED
*     
*     !!! Connect 12V power supply to input !!!
*     
*     - calculates & adjusts PWM Dutycycle to reach the output voltage
*     - Press DOWN Button for Buck Converter (5V Output)
*     - Press UP Button for Boost Converter (24V Output)
*     - Press no button for no output voltage
*     - OLED displays values
*
*     - Current sense factor = 0,625 OR 1,6
*     - Voltage factor = 0,0248659191 OR 40,2156863
*/

#include <Arduino.h>
#include <Wire.h>
//#include <SPI.h>
#include "ADS1X15.h"
#include <U8g2lib.h>
#include "bitmaps.h"

// Pin Definitions
#define GPIO_BTN_SEL      17          // Buttons
#define GPIO_BTN_BACK     5
#define GPIO_BTN_DOWN     18
#define GPIO_BTN_UP       19
#define GPIO_PWM_BUCK     12          // MOSFET Driver
#define GPIO_EN_BUCK      18
#define GPIO_PWM_BOOST    26
#define GPIO_EN_BOOST     27
#define GPIO_I2C_ADC_CLK  22          // I2C ADC
#define GPIO_I2C_ADC_SDA  23
#define I2C_ADC_FREQ      100000
#define GPIO_I2C_OLED_CLK 16          // I2C OLED
#define GPIO_I2C_OLED_SDA 4
#define I2C_OLED_FREQ     400000

ADS1015 ADS(0x48, &Wire);                                                         // initialize ADS1115 on I2C bus 1 with I2C address 0x48
U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C u8g2(U8G2_R0, /*reset=*/ U8X8_PIN_NONE);  // initialize SSD1306 on I2C bus 2

// User Parameters
int
PWM_FREQ_a              = 39062,      // Active switching PWM Frequency
PWM_RES_a               = 11,         // Active switching PWM Resolution (in bits)
PWM_FREQ_b              = 1000,       // Always-on PWM Frequency
PWM_RES_b               = 12,         // Always-on PWM Resolution (in bits)
V_BUCK                  = 5.0000,     // Target Output Voltage - Step Up Converter
V_BOOST                 = 24.0000;    // Target Output Voltage - Step Down Converter
// Calibration Parameters
float
A0DivRatio              = 1.6000,     // voltage divider sensor ratio (change this value to calibrate voltage sensor)
A1DivRatio              = 1.6000,
A2DivRatio              = 40.2157,
A3DivRatio              = 40.2157,
PWM_DutyCycle_b         = 99.5000,     // Always-on PWM Dutycycle (in percent)
PWM_max                 = 98.0000,    // Maximum PWM Dutycycle (in %)
PWM_min                 = 2.0000;     // Minimum PWM Dutycycle (in %)
// System Parameters   (upper case = converted to decimal)
int
SWITCH_MODE             = 0,          // undefined=0, buck=2, buk-boost=4, boost=6
PWM_CH_BUCK             = 0,          // PWM Channel Buck-stage
PWM_CH_BOOST            = 2,          // PWM Channel Buck-stage
PWM_RES_A               = 0,          // Active switching PWM Resolution (in decimal)
PWM_DutyCycle_A         = 0,          // Active switching PWM Dutycycle (in decimal)
PWM_RES_B               = 0,          // Always-on PWM Resolution (in decimal)
PWM_DutyCycle_B         = 0,          // Always-on PWM Dutycycle (in decimal)
PWM_MAX                 = 0,          // Maximum PWM Dutycycle (in decimal)
PWM_MIN                 = 0;          // Minimum PWM Dutycycle (in decimal)
float
PWM_DutyCycle_a         = 0.0000,     // Active switching PWM Dutycycle (in percent)   !NO LONGER NEEDED!
A0_RAW                  = 0.0000,     // Raw ADC voltages
A1_RAW                  = 0.0000,
A2_RAW                  = 0.0000,
A3_RAW                  = 0.0000,
I_IN                    = 0.0000,     // Actual measured Input Current
I_OUT                   = 0.0000,     // Actual measured Output Current
V_OUT                   = 0.0000,     // Actual measured Output Voltage
V_IN                    = 0.0000;     // Actual measured Input Voltage
char 
text_string[30];                      // to string text & variables together & later print them in one sentence

void PWM_calculations();
void read_ADC_values();
void buck_mode();
void buck_mode_configure();
void boost_mode();
void boost_mode_configure();
void print_serial();
void print_display();



void setup() {
  // Button Pins
  pinMode(GPIO_BTN_SEL, INPUT);
  pinMode(GPIO_BTN_SEL, INPUT_PULLDOWN);
  pinMode(GPIO_BTN_BACK, INPUT);
  pinMode(GPIO_BTN_BACK, INPUT_PULLDOWN);
  pinMode(GPIO_BTN_UP, INPUT);
  pinMode(GPIO_BTN_UP, INPUT_PULLDOWN);
  pinMode(GPIO_BTN_DOWN, INPUT);
  pinMode(GPIO_BTN_DOWN, INPUT_PULLDOWN);

  // MOSFET Driver Pins
  pinMode(GPIO_EN_BUCK, OUTPUT);                // Enable Pins
  digitalWrite(GPIO_EN_BUCK, LOW);
  pinMode(GPIO_EN_BOOST, OUTPUT);
  digitalWrite(GPIO_EN_BOOST, LOW);
  pinMode(GPIO_PWM_BUCK, OUTPUT);               // PWM Pins
  ledcAttachPin(GPIO_PWM_BUCK, PWM_CH_BUCK);
  pinMode(GPIO_PWM_BOOST, OUTPUT);
  ledcAttachPin(GPIO_PWM_BOOST, PWM_CH_BOOST);

  // Initialize serial interface
  Serial.begin(115200);
  Serial.println("\nSerial Conection established");
  Serial.println(__FILE__);
  Serial.print("ADS1X15_LIB_VERSION: ");
  Serial.println(ADS1X15_LIB_VERSION);

  // Initialize I2C bus
  Wire.begin(GPIO_I2C_ADC_SDA, GPIO_I2C_ADC_CLK, I2C_ADC_FREQ);
  Serial.println("\nI2C-1 initialized");
  Wire1.begin(GPIO_I2C_OLED_SDA, GPIO_I2C_OLED_CLK, I2C_OLED_FREQ);
  Serial.println("\nI2C-2 initialized");
  
  // Initialize ADS1X15 library
  ADS.begin();
  ADS.setGain(1);         // ±4.096V
  ADS.setMode(1);         // continuous
  ADS.setDataRate(4);     // 1600 samples/s
  
  if (!ADS.isConnected()) {
    Serial.println("\nADS1x15 not connected :c");
  }
  else{
    Serial.println("\nADS1x15 connected :D");
  }

  // Initialize u8g2 display
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);    // 10px height font
  u8g2.setBitmapMode(1);              // makes background transparent
  u8g2.setDrawColor(1);
  u8g2.clearDisplay();
  Serial.println("\nU8G2 initialized");

  delay(1000);
}


void loop() {
  PWM_calculations();
  read_ADC_values();
  
  if(digitalRead(GPIO_BTN_DOWN) == 1){
    Serial.println("Configuring Buck Mode... ");
    buck_mode_configure();

    while(1){
      read_ADC_values();
      buck_mode();
      print_display();
    }
  }

  else if(digitalRead(GPIO_BTN_UP) == 1){
    Serial.println("Configuring Boost Mode... ");
    boost_mode_configure();

    while(1){
      read_ADC_values();
      boost_mode();
      print_display();
    }
  }

  else{
    u8g2.clearDisplay();
    u8g2.drawXBM( 0, 0, 128, 64, bitmap_boykisser);
    u8g2.sendBuffer();
    vTaskDelay(1000);
    u8g2.clearDisplay();
    vTaskDelay(1000);

    while(1){
      read_ADC_values();
      print_serial();
      print_display();
      vTaskDelay(100);
    }
  }
}



void PWM_calculations(){
  // Calculate Dutycycle steps (2 by the power of resolution-in-bits)
  PWM_RES_A = pow(2, PWM_RES_a) - 1;
  PWM_RES_B = pow(2, PWM_RES_b) - 1;
  // Calculate Min/Max Dutycycle
  PWM_MIN = PWM_RES_A * PWM_min / 100;
  PWM_MAX = PWM_RES_A * PWM_max / 100;
}

void read_ADC_values(){
  // Read voltages
  A0_RAW = ADS.readADC(0);
  A1_RAW = ADS.readADC(1);
  A2_RAW = ADS.readADC(2);
  A3_RAW = ADS.readADC(3);

  // Convert voltages
  I_IN = ADS.toVoltage(A0_RAW) * A0DivRatio;
  I_OUT = ADS.toVoltage(A1_RAW) * A1DivRatio;
  V_OUT = ADS.toVoltage(A2_RAW) * A2DivRatio;
  V_IN = ADS.toVoltage(A3_RAW) * A3DivRatio;
}

void buck_mode_configure(){
  SWITCH_MODE = 2;
  // Configre PWM for buck-stage: PWM switching
  ledcSetup(PWM_CH_BUCK, PWM_FREQ_a, PWM_RES_a);
  ledcWrite(PWM_CH_BUCK, 0);
  digitalWrite(GPIO_EN_BUCK, HIGH);

  // Configure PWM for buck-stage: Always on
  ledcSetup(PWM_CH_BOOST, PWM_FREQ_b, PWM_RES_b);
  PWM_DutyCycle_B = PWM_RES_B * PWM_DutyCycle_b / 100;
  ledcWrite(PWM_CH_BOOST, PWM_DutyCycle_B);
  digitalWrite(GPIO_EN_BOOST, HIGH);

  // calculate first PWM dutycycle   (Dutycycle% = TargetOutputVoltage / InputVoltage)
  PWM_DutyCycle_A = PWM_RES_A * ( V_BUCK / V_IN );
  PWM_DutyCycle_A = constrain(PWM_DutyCycle_A , PWM_MIN, PWM_MAX);
  ledcWrite(PWM_CH_BUCK, PWM_DutyCycle_A);
}

void buck_mode(){
  // calculate PWM dutycycle   (NewDutycycle% = LastDutycycle% * ( TargetOutputVoltage / ActualOutputVoltage) )
  PWM_DutyCycle_A = PWM_DutyCycle_A * ( V_BUCK / V_OUT );
  PWM_DutyCycle_A = constrain(PWM_DutyCycle_A , PWM_MIN, PWM_MAX);
  ledcWrite(PWM_CH_BUCK, PWM_DutyCycle_A);
}

void boost_mode_configure(){
  SWITCH_MODE = 6;
  // Configre PWM for buck-stage: PWM switching
  ledcSetup(PWM_CH_BOOST, PWM_FREQ_a, PWM_RES_a);
  ledcWrite(PWM_CH_BOOST, 0);
  digitalWrite(GPIO_EN_BOOST, HIGH);

  // Configure PWM for buck-stage: Always on
  ledcSetup(PWM_CH_BUCK, PWM_FREQ_b, PWM_RES_b);
  PWM_DutyCycle_B = PWM_RES_B * PWM_DutyCycle_b / 100;
  ledcWrite(PWM_CH_BUCK, PWM_DutyCycle_B);
  digitalWrite(GPIO_EN_BUCK, HIGH);

  // calculate first PWM dutycycle   (Dutycycle% = 1 - ( TargetOutputVoltage - InputVoltage ) / TargetOutputVoltage )
  PWM_DutyCycle_A = PWM_RES_A * ( 1 - ( ( V_BOOST - V_IN ) / V_BOOST ) );
  PWM_DutyCycle_A = constrain(PWM_DutyCycle_A , PWM_MIN, PWM_MAX);
  ledcWrite(PWM_CH_BOOST, PWM_DutyCycle_A);
}

void boost_mode(){
  // calculate PWM dutycycle   (NewDutycycle% = LastDutycycle% * ( ( TargetOutputVoltage / ActualOutputVoltage ) ^ 2 )
  //PWM_DutyCycle_A = PWM_DutyCycle_A * pow(V_BOOST / V_OUT, 2);

  // calculate PWM dutycycle   (NewDutycycle% = LastDutycycle% * ( ( ActualOutputVoltage / TargetOutputVoltage ) ^ 2 )
  //PWM_DutyCycle_A = PWM_DutyCycle_A * pow(V_OUT / V_BOOST, 2);
  
  // calculate PWM dutycycle   (NewDutycycle% = LastDutycycle% * ( ActualOutputVoltage / TargetOutputVoltage )
  PWM_DutyCycle_A = PWM_DutyCycle_A * (V_OUT / V_BOOST);

  PWM_DutyCycle_A = constrain(PWM_DutyCycle_A , PWM_MIN, PWM_MAX);
  ledcWrite(PWM_CH_BOOST, PWM_DutyCycle_A);
}

void print_serial(){
      Serial.print("\nInput Voltage:   ");
      Serial.println(V_IN);
      Serial.print("Output Voltage:   ");
      Serial.println(V_OUT);
      Serial.print("Dutycycle %:   ");
      Serial.println((PWM_DutyCycle_A * 100 / PWM_RES_A), 1);
}

void print_display(){
    //u8g2.clearDisplay();
    
    //dtostrf();
    //dtostrf(V, 7, 3, voltString); // AVR convenience
    sprintf(text_string, "In: %.2fV %.2fA", V_IN, I_IN);
    u8g2.drawStr(18, 12, text_string);
    sprintf(text_string, "Out: %.2fV %.2fA", V_OUT, I_OUT);
    u8g2.drawStr(18, 28, text_string);
    sprintf(text_string, "PWM: %d %", (PWM_DutyCycle_A * 100 / PWM_RES_A));
    u8g2.drawStr(18, 44, text_string);
    sprintf(text_string, "Switchmode: %d", SWITCH_MODE);
    u8g2.drawStr(18, 60, text_string);
    u8g2.sendBuffer();
}


