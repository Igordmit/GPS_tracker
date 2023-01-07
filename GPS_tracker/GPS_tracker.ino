#include <TinyGPS.h>
TinyGPS gps;
bool newData;
unsigned long start;
int currentData;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  newData = false;
  start = millis();
}

void loop() {
 // easySerial();

  mainFunction();
}

//Функция для проверки модуля через u-center
void easySerial(){
  if(Serial1.available()){
    Serial.write(Serial1.read());
  }
}

void mainFunction(){
  if (millis() - start > 5000) {

    newData = readgps();
    if (newData) {
      start = millis();
      gpsdump(gps);
    }
  }  
}

bool readgps() {
  while (Serial1.available()) {
    int b = Serial1.read();
    if ('\r' != b) {
      if (gps.encode(b))
        return true;
    }
  }
  return false;
}

void gpsdump(TinyGPS &gps) {
  float lat, lon;
  String lats, lons;
  gps.f_get_position(&lat, &lon);
 
  lats = convertCoordinates(lat);
  lons = convertCoordinates(lon);
}

String convertCoordinates(float coordinates){

  float temp = (coordinates - int(coordinates)) * 60;
  String result = String(int(coordinates)) + String(temp);
  return result;

}
