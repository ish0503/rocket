#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h> // Adafruit BMP280 Library 모듈 다운. (고도 센서)
#include <EEPROM.h>
#include "DHT.h"

#define SCL 8
#define SDA 9 
#define CSB 10
#define SDO 11

#define DHTPIN 5       // DHT 센서가 연결된 핀 번호
#define DHTTYPE DHT11  // 사용 중인 DHT 센서의 종류(DHT11)

DHT dht(DHTPIN, DHTTYPE); // DHT 센서 객체 생성

// LED
int green = 11;
int red = 10;
int blue = 9;

Adafruit_BMP280 bmp(CSB, SDA, SDO, SCL); // 고도센서 클래스 불러오기

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

#include <SoftwareSerial.h>

#include <TinyGPS.h> // TinyGPS모듈 다운

TinyGPS gps;
SoftwareSerial ss(4, 3);

uint32_t start_time = 0; // 병렬 처리를 위한 시간

void setup () {
  start_time = millis();// 병렬 처리를 위한 시간 설정
  
  Serial.begin(9600);
  dht.begin();
  LEDSetting();
  AltitudeSensorCheck();
  SlopeSetting();
  GPSSetting();
}

int time = millis();
bool loadData = true;

void loop() {
  if (loadData){
    Data_Read();
    while (true){

    }
  }else{
    int Maxtime = 1000; // 최대 시간 연산
    int timechange = millis() - time;
    Serial.println(timechange);
    //LED 병렬 예제
    if (timechange < Maxtime * 0.3) {
      LED1Play();
    }
    else if (timechange < Maxtime * 0.6) {
      LED2Play();
    }
    else if (timechange < Maxtime) {
      LED3Play();
    }
    Humidity_Temperature_Check();
    AltitudeCheck();
    SlopeCheck();
    GPSCheck();

    delay(100);

    if (timechange > Maxtime) {
      time = millis();
    }
  }
}