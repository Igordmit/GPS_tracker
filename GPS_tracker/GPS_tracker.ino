#include <TinyGPS.h>
#include <EEPROM.h>
TinyGPS gps;
bool newData;
unsigned long start;
int currentData;
char target[] = "hello";
String dates, times, lats, lons, speed, sats, course, height = "0";
String password = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.setTimeout(50);
  newData = false;
  start = millis();
  password =  getPassword();
}

void loop() {
 //easySerial();
  getCommand();
  //mainFunction();
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
  unsigned long date, time;
  lats =""; 
  lons =""; 
  speed =""; 
  sats =""; 
  course ="";
  gps.f_get_position(&lat, &lon);
  gps.get_datetime(&date, &time);
 
  lats = convertCoordinates(lat);
  lons = convertCoordinates(lon);
  speed = (String)gps.speed();
  sats = (String)gps.satellites();
  course = (String)gps.course();
  dates = String(date);
  times = String(time).substring(0,6);

  Serial.println(shortPacket());
  Serial.println(pingPacket());
}

String convertCoordinates(float coordinates){

  String result = "";
  int part;
  float temp = ((int)coordinates*100 + (coordinates - (int)coordinates) * 60);
  result = String((int)temp);
  temp = (temp - (int)temp) * 100000;
  result = result + "." + String(temp);
  result = result.substring(0,result.length()-3);

  return result;

}

String shortPacket(){
  return "#SD#"+dates+";"+times+";"+lats+";N;"+lons+";E;"+speed+";"+course+";"+height+";"+sats+(char)13+(char)10;
}

// String loginPacket(){
//   String packet = "#L#";
// }

String pingPacket(){
  return "#P#"+(char)13+(char)10;
}

void  getCommand(){
  String fromPort = "";
  while (Serial.available()) {
    fromPort = Serial.readString();
     if(fromPort.indexOf("AT+PASS=") != -1){
      setPassword(fromPort.substring(8, fromPort.length()));
      getPassword();
    }
    
  }
  
}

void setSetting(String newPassword){
  int i ;
  for(i = 0; i<newPassword.length()-1; i++){
    EEPROM.write(i, newPassword[i]);
  }
  EEPROM.write(i, (char)10);
}

String setSetting(){
  int count = 0;
  password = "";
  char symbol = (char)EEPROM.read(count);
  while((symbol != (char)10) && (count <= 10)){
    password += symbol;
    Serial.println(symbol);
    count++;
    symbol = (char)EEPROM.read(count);
  }
  
  return password;
}