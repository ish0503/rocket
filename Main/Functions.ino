bool Save_Mode = false; // EEPROM은 사용 제한이 있기 때문에 mode를 true로 할때만 저장.

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

void Humidity_Temperature_Check(){
  float humidity = dht.readHumidity();       // 습도 값 읽기
  float temperature = dht.readTemperature(); // 온도 값 읽기
 
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!"); // 읽기 실패 시 에러 메시지 출력
    return;
  }

  // 온도와 습도 값을 시리얼 모니터에 출력
  Serial.print("Temperature: ");
  Serial.print((int)temperature);
  Serial.print(" *C, ");
  Data_Save((int)temperature);

  Serial.print("Humidity: ");
  Serial.print((int)humidity);
  Serial.println(" %");
  Data_Save((int)humidity);
}

void AltitudeSensorCheck(){ // 고도 센서 있는지 체크
  if (!bmp.begin()) { // 고도 센서가 인식되지 않는 경우 
      Serial.println(F("고도센서가 인식되지 않습니다. 연결 상태를 확인해주세요. ")) ;
  }
}

void AltitudeCheck(){ // 고도 센서 출력
  int tmp = bmp.readTemperature();
  int press = bmp.readPressure();
  int alti = bmp.readAltitude(999.2);
  Serial.println("=============");
  Serial.print(F("온도 ="));
  Serial.print(tmp);
  Serial.println(" *C"); //센서로 온도 측정해서 출력
  Data_Save(tmp);

  Serial.print(F("기압="));
  Serial.print(press);
  Data_Save(press);

  Serial.println(" Pa"); //센서로 기압 측정해서 출력
  Serial.print(F("고도 ="));
  Serial.print (alti); // 센서로 고도 측정해서 출력
  Serial.println(" m");
  Data_Save(alti);
  Serial.println("===========\n");
}

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

int data_location = 0;

void Data_Save(int Data){
  if (Save_Mode){
      Serial.println("데이터 저장을 실시합니다.");

      EEPROM.write(data_location, Data); // write(주소, 값)
      Serial.println("EEPROM Address : " + String(data_location) + "\t Value : " + Data);
      Serial.println();

      data_location += 1;
  }
}

void Data_Read(){
  Serial.println("Reading EEPROM...");
  for(int i = 0; i < 1024; i=i+1) {               // EEPROM에 데이터 읽기
    byte data = EEPROM.read(i);
    Serial.println("EEPROM Address : " + String(i) + "\t Value : " + data);
  }
}