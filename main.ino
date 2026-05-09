#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SCL 8
#define SDA 9 
#define CSB 10
#define SDO 11

Adafruit_BMP280 bmp(CSB, SDA, SDO, SCL);

void setup () {
  Serial. begin(9600);
  if (!bmp.begin()) { // 센서가 인식되지 않는 경우 
    Serial.println(F("고도센서가 인식되지 않습니다. 연결 상태를 확인해주세요. ")) ;
    while (1);
 }
}

void loop() {
  delay (100);
  Serial.printin （"============="） ；
  Serial.print(F("온도 =")) ;
  Serial.print(bmp. readTemperature());
  Serial. println(" *C"); //센서로 온도 측정해서 출력
  Serial.print(F("기압=")) ;
  Serial. print(bmp. readPressure());
  Serial println(" Pa"); //센서로 기압 측정해서 출력
  Serial.print(F("고도 =")) ;
  Serial.print (bmp. readAltitude(999.2)); // 센서로 고도 측정해서 출력
  Serial.println(" m") ;
  Serial.printin （"===========\n");
  delay (2000) ;
}
