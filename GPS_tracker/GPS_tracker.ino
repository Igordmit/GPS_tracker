#include <TinyGPS.h>
TinyGPS gps;
bool newData;
unsigned long start;
int currentData;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  Serial.println(TinyGPS::library_version());
  newData = false;
  start = millis();
}

void loop() {

  if ((millis() - start > 5000) && (Serial1.available())) {
    currentData = Serial1.read();
    Serial.println(currentData);
    if (('\r' != currentData) && gps.encode(currentData)) {
      Serial.println("debug");
      newData = true;
    }

    if(newData){
        gpsdump(gps);
    }

    start = millis();
  }
}

void gpsdump(TinyGPS &gps){
  long lat, lon;
  gps.get_position(&lat, &lon);
  Serial.print(lat); Serial.print(", "); Serial.print(lon); 
}
