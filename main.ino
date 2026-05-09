#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h> // Adafruit BMP280 Library 모듈 다운. (고도 센서)

#define SCL 8
#define SDA 9 
#define CSB 10
#define SDO 11

// LED
int green = 11;
int red = 10;
int blue = 9;

void LEDSetting(){
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
}

void LED1Play(){
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
}

void LED2Play(){
  digitalWrite(green, LOW);
  digitalWrite(red, HIGH);
  digitalWrite(blue, LOW);
}

void LED3Play(){
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);
  digitalWrite(blue, HIGH);
}

Adafruit_BMP280 bmp(CSB, SDA, SDO, SCL); // 고도센서 클래스 불러오기

void AltitudeSensorCheck(){ // 고도 센서 있는지 체크
  if (!bmp.begin()) { // 고도 센서가 인식되지 않는 경우 
      Serial.println(F("고도센서가 인식되지 않습니다. 연결 상태를 확인해주세요. ")) ;
  }
}

void AltitudeCheck(){ // 고도 센서 출력
  Serial.println("=============");
  Serial.print(F("온도 ="));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C"); //센서로 온도 측정해서 출력
  Serial.print(F("기압="));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa"); //센서로 기압 측정해서 출력
  Serial.print(F("고도 ="));
  Serial.print (bmp.readAltitude(999.2)); // 센서로 고도 측정해서 출력
  Serial.println(" m");
  Serial.println("===========\n");
}

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void SlopeCheck(){ // 기울기 센서: MPU-6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); 
  AcX=Wire.read()<<8|Wire.read();

  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read(); 
  Tmp=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read(); 
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read(); 
  Serial.println("=============");
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  Serial.println("===========\n");
}

void SlopeSetting(){ // MPU-6050 세팅
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

#include <SoftwareSerial.h>

#include <TinyGPS.h> // TinyGPS모듈 다운

TinyGPS gps;
SoftwareSerial ss(4, 3);

void GPSSetting(){ // GPS(gy-gpsv3-neo) 세팅
  ss.begin(9600);
  
  Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS::library_version());
  Serial.println();
}

void GPSCheck(){
  if (!ss.available()) {  
      return;
  }
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;) // 나중에 이거 millis 어떻게든 없애야함
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }

  Serial.println("=============");
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
  Serial.println("===========\n");
}

void setup () {
  Serial.begin(9600);
  LEDSetting();
  AltitudeSensorCheck();
  SlopeSetting();
  GPSSetting();
}

int time = millis();

void loop() {
  int Maxtime = 1000; // 최대 시간 연산
  int timechange = millis() - time;
  Serial.println(timechange);
  //LED 병렬
  if (timechange < Maxtime * 0.3) {
    LED1Play();
  }
  else if (timechange < Maxtime * 0.6) {
    LED2Play();
  }
  else if (timechange < Maxtime) {
    LED3Play();
  }
  //고도 센서 병렬
  if (timechange < Maxtime) {
    AltitudeCheck();
  }
  //기울기 센서 병렬
  if (timechange < Maxtime) {
    SlopeCheck();
  }
  //GPS 센서 병렬
  if (timechange < Maxtime) {
    GPSCheck();
  }

  if (timechange > Maxtime) {
    time = millis();
  }
}
