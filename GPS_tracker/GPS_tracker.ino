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
  long lat, lon;
  String lats, lons;
  gps.get_position(&lat, &lon);
  
  lats = convertCoordinates(lat);
  lons = convertCoordinates(lon);
}

String convertCoordinates(long coordinates){

  String tempCoords = String(coordinates);
  String degrees = tempCoords.substring(0, tempCoords.length() - 6);
  String part = String("0." + tempCoords.substring(tempCoords.length() - 6));
  part = String((part.toDouble())*60);
  return degrees + part;

}
