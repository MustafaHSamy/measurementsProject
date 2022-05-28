#include <Math.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <LiquidCrystal.h>
#define btn 2
#define bzr 3
#define ldr A0
#define ntc A1
const int rs = 12;
const int en = 13;
const int D7 = 11;
const int D6 = 10;
const int D5 = 9;
const int D4 = 8;

LiquidCrystal lcd(rs, en, D4, D5, D6, D7);
#define P0 101325
double T, P, A;
float alarmT = 50000;
int flag = 5;
int flag2 = 0;
unsigned long timer = 0;
unsigned long alarmTimer = 0;
float R1 = 10000;
float I, logR2, R2, TEMP;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
Adafruit_BMP085 bmp;

void clearLcd()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
}
void printSI()
{
    flag2 += 1;
    lcd.setCursor(0, 0);
    lcd.print("T=");
    lcd.print(T + 273.15, 1);
    lcd.print("K");

    lcd.setCursor(9, 0);
    lcd.print("A=");
    lcd.print(A, 1);
    lcd.print("m");

    lcd.setCursor(0, 1);
    lcd.print("P=");
    lcd.print(P, 3);
    lcd.print("pa");

    while (flag2 == 8)
    {
        clearLcd();
        lcd.setCursor(0, 0);
        lcd.print("Light Intensity ");
        lcd.setCursor(0, 1);
        lcd.print(I, 2);
        lcd.print("%");
        if ((millis() - timer) >= 2000)
        {
            flag2 = 0;
        }
    }
}

void printImperial()
{
    flag2 += 1;
    lcd.setCursor(0, 0);
    lcd.print("T=");
    lcd.print(int((T * 9 / 5) + 32));
    lcd.print("F");

    lcd.setCursor(7, 0);
    lcd.print("A=");
    lcd.print(A * 3.28084, 2);
    lcd.print("ft");

    lcd.setCursor(0, 1);
    lcd.print("P=");
    lcd.print(P * 0.000145037738, 4);
    lcd.print("psi");

    while (flag2 == 8)
    {
        clearLcd();
        lcd.setCursor(0, 0);
        lcd.print("Light Intensity ");
        lcd.setCursor(0, 1);
        lcd.print(I, 2);
        lcd.print("%");
        if ((millis() - timer) >= 2000)
        {
            flag2 = 0;
        }
    }
}

void displayAlt()
{
    lcd.setCursor(0, 0);
    lcd.print("A=");
    lcd.print(A, 3);
    lcd.print("m");

    lcd.setCursor(0, 1);
    lcd.print("A=");
    lcd.print(A * 3.28084, 3);
    lcd.print("ft");
}

void displayTemp()
{
    lcd.setCursor(0, 0);
    lcd.print("T=");
    lcd.print(T, 2);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print(T + 273.15, 1);
    lcd.print("K");

    lcd.setCursor(10, 1);
    lcd.print((T * 9 / 5) + 32, 2);
    lcd.print("F");
}

void displayPressure()
{
    lcd.setCursor(0, 0);
    lcd.print("P=");
    lcd.print(P * 9.8792e-6, 3);
    lcd.print("atm");

    lcd.setCursor(0, 1);
    lcd.print(P * 0.00750062, 3);
    lcd.print("mmHg");
}

void displayNames()
{
    clearLcd();
    lcd.setCursor(0, 0);
    lcd.print("Dr Refaat Khater");
    delay(2500);
    clearLcd();
    lcd.setCursor(0, 0);
    lcd.print("Eng. Mahmoud");
    lcd.setCursor(0, 1);
    lcd.print("Abdel Aziz");
    delay(2500);
    clearLcd();
    lcd.setCursor(0, 0);
    lcd.print("Ahmed Hussein");
    lcd.setCursor(0, 1);
    lcd.print("Kareem Magdy");
    delay(2000);
    clearLcd();
    lcd.setCursor(0, 0);
    lcd.print("Mustafa Hesham");
    lcd.setCursor(0, 1);
    lcd.print("Ahmed Yasser");
    delay(2000);
    clearLcd();
    lcd.setCursor(0, 0);
    lcd.print("Ahmed Alaa Eldin");
    delay(1000);
}

double getTemp()
{
    int ntcValue = analogRead(ntc);
    R2 = R1 * (1023.0 / (float)ntcValue - 1.0);
    logR2 = log(R2);
    TEMP = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    TEMP = TEMP - 273.15;
    Serial.println(TEMP);
    return TEMP;
}

float getLightIntensity()
{
    float x = map(analogRead(ldr), 0, 1023, 0, 100);
    return x;
}

void alarm()
{
    digitalWrite(bzr, HIGH);
    delay(500);
    digitalWrite(bzr, LOW);
    delay(500);
}

void setup()
{
    pinMode(btn, INPUT);
    pinMode(bzr, OUTPUT);
    Serial.begin(9600);
    lcd.begin(16, 2);
    for (int i = 0;; i++)
    {
        if (bmp.begin())
        {
            Serial.println("Initialized bmp");
            break;
        }
        else
        {
            Serial.println("Couldn't find a bmp sensor");
        }
    }
}

void loop()
{
    if (digitalRead(btn))
    {
        while (digitalRead(btn))
            ;
        flag += 1;
        flag2 = 0;
    }
    T = getTemp();                                                      // Temperature in C
    P = bmp.readPressure();                                             // Pressure in pascal
    A = ((1 - (pow((P0 / P), (1 / 5.257)))) * (T + 273.15)) / (0.0065); // Altitude in meters
    I = getLightIntensity();                                            // light intesnity

    if (((T - alarmT) >= 8.5) && ((millis() - alarmTimer) <= 60000))
    {
        alarm();
    }
    else if ((millis() - alarmTimer) >= 60000)
    {
        alarmTimer = millis();
        alarmT = T;
    }
    if (T >= 58)
    {
        alarm();
    }

    if ((flag == 0) && ((millis() - timer) >= 1000))
    {
        // SI
        timer = millis();
        clearLcd();
        printSI();
    }
    else if ((flag == 1) && ((millis() - timer) >= 1000))
    {
        // Imperial
        timer = millis();
        clearLcd();
        printImperial();
    }
    else if ((flag == 2) && ((millis() - timer) >= 1000))
    {
        // Pressure Values
        timer = millis();
        clearLcd();
        displayPressure();
    }
    else if ((flag == 3) && ((millis() - timer) >= 1000))
    {
        // Temperature Values
        timer = millis();
        clearLcd();
        displayTemp();
    }
    else if ((flag == 4) && ((millis() - timer) >= 1000))
    {
        // Altitude Values
        timer = millis();
        clearLcd();
        displayAlt();
    }
    else if (flag == 5)
    {
        // Team Names
        clearLcd();
        displayNames();
        flag = 0;
    }
    //
    //
    //  Serial.print("Altitude = ");
    //  Serial.print(A / 100);
    //  Serial.println(" meters");
    //
    //  Serial.print("Temperature = ");
    //  Serial.print(T);
    //  Serial.print(" C           |  ");
    //  Serial.print(T + 273.15);
    //  Serial.println(" K");
    //
    //  Serial.print("Pressure = ");
    //  Serial.print(P * 9.8692E-6);
    //  Serial.print(" atm           |  ");
    //  Serial.print(P);
    //  Serial.print(" Pa           |  ");
    //  Serial.print(P * 0.00750062);
    //  Serial.println("mmhg");
}