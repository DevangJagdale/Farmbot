int temp;
int moist,gas;
int motor_status=0;

int ledpin=6;
int pirpin=7;
int pirstat=0;

int buzzerpin=5;
int dcmotorpin=4;

int gaspin=A2;

String ssid     = "Simulator Wifi";  // SSID to connect to
String password = ""; //virtual wifi has no password 
String host     = "api.thingspeak.com"; // Open farmbot API
const int httpPort   = 80;
String url = "/update?api_key=xxxxxxxxxxxx";
String field1="&field1=";
String field2="&field2=";
String field3="&field3=";
String field4="&field4=";
String field5="&field5=";

//Replace XXXXXXXXXXXXXXXX by your ThingSpeak Channel API Key

void setupESP8266(void) {
  
  // Start our ESP8266 Serial Communication
  Serial.begin(115200);   // Serial connection over USB to computer
  Serial.println("AT");   // Serial connection on Tx / Rx port to ESP8266
  delay(10);        // Wait a little for the ESP to respond
  if (Serial.find("OK"))
    Serial.println("ESP8266 OK!!!");
    
  // Connect to Simulator Wifi
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(10);        // Wait a little for the ESP to respond
  if (Serial.find("OK"))
    Serial.println("Connected to WiFi!!!");
  
  // Open TCP connection to the host:
  //ESP8266 connects to the server as a TCP client. 

  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(50);        // Wait a little for the ESP to respond
  if (Serial.find("OK")) 
   Serial.println("ESP8266 successfully Connected to server!!!") ;
  else
    Serial.println("Connection failed");
  
  Serial.println();
 
}

void anydata(void) {
     
  String httpPacket= "GET "+ url + field1 +String(temp) + field2 +String(moist)+ field3 +String(pirstat)+ field4 +String(gas)+ field5 +String(motor_status)+ " HTTP/1.1\r\nHost:" + host + "\r\n\r\n";
  int length = httpPacket.length();
  
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(1); // Wait a little for the ESP to respond if (!Serial.find(">")) return -1;

  // Send our http request
  Serial.print(httpPacket);
  delay(1); // Wait a little for the ESP to respond
  if (!Serial.find("SEND OK\r\n")) 
    Serial.println("data uploaded sucessfully");
  else 
    Serial.println("failed to upload data");

}


void setup() {
  pinMode(A0, INPUT);
  setupESP8266();
  
  pinMode(gaspin, INPUT);
  
  pinMode(buzzerpin,OUTPUT);
  
  pinMode(dcmotorpin,OUTPUT);
  
  pinMode(ledpin, OUTPUT);
  pinMode(pirpin, INPUT);
  Serial.begin(9600);
               
}

void loop() {  
  
  temp=map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
  moist=map(analogRead(A1), 0, 1023, 0, 100);;
  gas=analogRead(gaspin);
  pirstat=digitalRead(pirpin);
  
  anydata();
  
  //Serial.println(pirstat);
  while(pirstat==HIGH)
  {
	digitalWrite(ledpin, HIGH);
    tone(buzzerpin,100);
    Serial.println("obstacle detected");
    pirstat=digitalRead(pirpin);
  }

  noTone(buzzerpin);
  digitalWrite(ledpin, LOW);
  
  delay(1);
  
  if(temp>30 && moist<50){
  	Serial.println("low water, turn on motor");
    digitalWrite(dcmotorpin,HIGH);
    motor_status=1;
  }
  else{
    digitalWrite(dcmotorpin,LOW);
    motor_status=0;
  }
  
  //Serial.println(gas);
  while(gas>500){
  	tone(buzzerpin,100);
    digitalWrite(ledpin, HIGH);
    gas=analogRead(gaspin);
  }
  noTone(buzzerpin);
  digitalWrite(ledpin,LOW);
  
}
