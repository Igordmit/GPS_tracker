#include <TinyGPS.h>
#include <EEPROM.h>

#define SERVER_SETTING_POSITION 0
#define SERVER_SETTING_MAX_LENGTH 15

#define PORT_SETTING_POSITION 16
#define PORT_SETTING_MAX_LENGTH 5

#define ID_SETTING_POSITION 22
#define ID_SETTING_MAX_LENGTH 15

#define PASSWORD_SETTING_POSITION 38
#define PASSWORD_SETTING_MAX_LENGTH 10

TinyGPS gps;
bool newData;
unsigned long start;
int currentData;
char target[] = "hello";
String dates, times, lats, lons, speed, sats, course, height = "0";
String password = "";
String server = "";
String port = "";
String id = "";
bool serialWork = false;
bool logged = false;
bool waitAnswer = false;
String waitedAnswer = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.setTimeout(50);
  newData = false;
  start = millis();
  password = getPassword();
  server = getServer();
  port = getPort();
  id = getId();
}

void loop() {
  getCommand();
  if(serialWork){
    easySerial();
  } else {
    mainFunction();
  }
  
}

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

String loginPacket(){
  String packet = "#L#"+id+";"+password+(char)13+(char)10;
}

String pingPacket(){
  return "#P#"+(char)13+(char)10;
}

void  getCommand(){
  String fromPort = "";
  while (Serial.available()) {
    fromPort = Serial.readString();
    
    if(fromPort.indexOf("AT+PASS=") != -1){
      setPassword(fromPort.substring(8, fromPort.length()));
      password = getPassword();
    }

    else if(fromPort.indexOf("AT+SERVER=") != -1){
      setServer(fromPort.substring(10, fromPort.length()));
      server = getServer();
    }

     else if(fromPort.indexOf("AT+PORT=") != -1){
      setServer(fromPort.substring(8, fromPort.length()));
      server = getServer();
    }

    else if(fromPort.indexOf("AT+ID=") != -1){
      setId(fromPort.substring(6, fromPort.length()));
      id = getId();
    }

    else if(fromPort.indexOf("AT+SERIAL") != -1){
      serialWork = true;
    }

    else if(fromPort.indexOf("AT+TRACKER") != -1){
      serialWork = false;
    }
    
    else{
      Serial1.println(fromPort);
    }
  }
  
}

void setSetting(String newSetting, int seek, int countSymbols){
  int i ;
  for(i = 0; i<newSetting.length()-1; i++){
    EEPROM.write(i + seek, newSetting[i]);
  }
  EEPROM.write(i + seek, (char)10);
}

String getSetting(int seek, int countSymbols){
  int count = seek;
  String setting = "";
  char symbol = (char)EEPROM.read(count);
  while((symbol != (char)10) && (count <= seek + countSymbols)){
    setting += symbol;
    count++;
    symbol = (char)EEPROM.read(count);
  } 
  return setting;
}

void setPassword(String newPassword){
  setSetting(newPassword, PASSWORD_SETTING_POSITION, PASSWORD_SETTING_MAX_LENGTH);
}

String getPassword(){
  return getSetting(PASSWORD_SETTING_POSITION, PASSWORD_SETTING_MAX_LENGTH);
}

void setServer(String serverPort){
  setSetting(serverPort, SERVER_SETTING_POSITION, SERVER_SETTING_MAX_LENGTH);
}

String getServer(){
  return getSetting(SERVER_SETTING_POSITION, SERVER_SETTING_MAX_LENGTH);
}

void setPort(String port){
  setSetting(id, PORT_SETTING_POSITION, PORT_SETTING_MAX_LENGTH);
}

String getPort(){
  return getSetting(PORT_SETTING_POSITION, PORT_SETTING_MAX_LENGTH);
}

void setId(String id){
  setSetting(id, ID_SETTING_POSITION, ID_SETTING_MAX_LENGTH);
}

String getId(){
  return getSetting(ID_SETTING_POSITION, ID_SETTING_MAX_LENGTH);
}

void initializeGSM(){
  sendATGSM("AT");
  delay(100);
  sendATGSM("AT+CSTT=\"internet\"");
  delay(100);
  sendATGSM("AT+CIICR");  
  delay(100);
}

void connectToServer(){
  sendATGSM("AT+CIPSTART=\"TCP\",\""+server+"\","+port+"");
  waitAnswer = true;
  waitedAnswer = "CONNECT OK";
}

void sendATGSM(String command){
  Serial2.println(command);
}

void sendData(String package){
  sendATGSM("AT+CIPSEND");
  delay(100);
  sendATGSM(package);
  delay(100);
  Serial2.println((char)26); 
  delay(100);
  Serial2.println();
}