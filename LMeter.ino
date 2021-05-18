/*
  LMeter.

  To do - write stuff here.
*/
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //on 328p, OC1A function is tied to pin PB1 (D9)
  pinMode( D9, OUTPUT );

  analogReference(INTERNAL4V096);

  Serial.begin(115200);
  lcd.begin(16, 2);
  delay(100); // wait for the LCD to init
  lcd.setCursor(0, 0); lcd.print("uH: ");
  lcd.setCursor(0, 1); lcd.print("mV: ");

  //TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 65kHz @ 32MHz on Tim2 channels

  //Speed up ADC. On AVR, comment out the following, or adjust the prescaler value to something higher. (64?)
  //ADCSRA = ((0 << ADPS0) | (0 << ADPS1) | (1 << ADPS2));   // ADC prescaler to 16, ADC Clk to 2MHz / 133MSpS
  
  setFrequency(200000);
}

// Set TIMER1 to the frequency that we will get on pin OCR1A
void setFrequency(uint32_t freq)
{
  TCCR1A &= ~(1 << COM1A0); //turn off output
  if (freq == 0) return;

  uint32_t requiredDivisor = (F_CPU / 2) / freq;

  uint16_t prescalerVal;
  uint8_t prescalerBits;
  if (requiredDivisor < 65536UL) {
    prescalerVal = 1;
    prescalerBits = 1;
  }
  else if (requiredDivisor < 8 * 65536UL) {
    prescalerVal = 8;
    prescalerBits = 2;
  }
  else if (requiredDivisor < 64 * 65536UL) {
    prescalerVal = 64;
    prescalerBits = 3;
  }
  else if (requiredDivisor < 256 * 65536UL) {
    prescalerVal = 256;
    prescalerBits = 4;
  }
  else {
    prescalerVal = 1024;
    prescalerBits = 5;
  }

  uint16_t top = ((requiredDivisor + (prescalerVal / 2)) / prescalerVal) - 1;
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | prescalerBits;
  TCCR1C = 0;
  OCR1A = (top & 0xFF);

  //turn on output OCR1 (PB1/D9)
  TCNT1H = 0;
  TCNT1L = 0;
  TCCR1A |= (1 << COM1A0);
}

// the loop function runs over and over again forever
void loop() {


  //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(900);                       // wait for a second
  //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

  delay(300);                       // wait for the LCD

  uint16_t voltage = ReadInductor(); //== reading * vref / 2^12-bit == 4096mV/4096 == vref/1
  lcd.setCursor(5, 0);
  lcd.print("        ");
  lcd.setCursor(5, 0);
  lcd.print(curveFit(voltage));
  lcd.setCursor(5, 1);
  lcd.print("        ");
  lcd.setCursor(5, 1);
  lcd.print(voltage);
}

uint16_t ReadInductor(void) {
  uint32_t runningtotal = 0;
  for (int i = 0; i < 30; i++) runningtotal += analogRead(A0);
  return (uint16_t)(runningtotal / 30);
}

/*curve-fitting analysis from several hundred inductors tested shows that
   y = 21337840*e^(-1*(x+12785.47)^2/(2*2927.016^2))
   for all values of x from 10 to 470, give or take a tiny bit

   linear curve from 0 to 10uH:
   y = 123.531 + -0.04*voltage

   https://elsenaju.eu/Calculator/online-curve-fit.htm
*/
float curveFit(uint16_t voltage) {

  //curve 1 - valid from voltage = 2850 to 3100
  if (voltage > 2850) return (123.531 + (-0.04*voltage));
  //curve 2 - valid from voltage = 650 to 2850
  if (voltage > 650)  return 21337840 * pow(M_E, (-1 * pow((voltage + 12785.47), 2) / (2 * pow(2927.016, 2))));
  //curve 3 - valid from voltage = 0 to 650mV
  return 999.99;
}
