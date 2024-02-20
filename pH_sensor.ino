#include <LiquidCrystal_I2C.h> // Library for LCD
#define TdsSensorPin A1
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
const int sensorPin = A0;

void setup() {
  // Begin serial communication
  Serial.begin(9600);
  pinMode(TdsSensorPin,INPUT);
  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void loop() {
  // Read the analog value from the pH sensor
  ph();
  tds();
}
void ph()
{
  int sensorValue = analogRead(sensorPin);
  // Serial.print(sensorValue);

  // Convert the analog value to voltage
  float voltage = sensorValue * (5.0 / 1023.0);

  // Convert voltage to pH value (this conversion formula may vary depending on the sensor)
  // Serial.print(voltage);
  float pHValue = 5.5  - (voltage - 2.5) * 3.5;

  // Print the pH value to the serial monitor
  Serial.print("pH Value: ");
  Serial.println(pHValue);
  lcd.setCursor(0, 0);
  lcd.print("pH Value : "); 
  lcd.print(pHValue);
  //lcd.setCursor(1, 1);
  //lcd.print("AMITY"); 
  delay(1000); // Delay for readability, adjust as needed
}
void tds()
{
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");   
      
      Serial.print("TDS Value:");
      Serial.print(tdsValue + 100,0);
      Serial.println("ppm");
  

  lcd.setCursor(0, 1);
  lcd.print("TDS Value:"); 
  lcd.print(tdsValue + 100, 0);
  lcd.println("ppm");
  
  
}
}
int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}