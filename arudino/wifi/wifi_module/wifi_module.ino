/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
//#include <MsTimer2.h>

#include <WiFi.h>

#define SerialSpeed 9600

const char* ssid     = "******";
const char* password = "******";

const char* host = "yaho.co.jp";

const char* streamId   = "....................";
const char* privateKey = "....................";
HardwareSerial mySerial(2); // UART2
int recv_data; // 受信データ

char ir0 = '9';
char ir1 = '9';
char ir2 = '9';
char ir3 = '9';

char humid0 = '9';
char humid1 = '9';

void setup()
{
    Serial.begin(SerialSpeed);
    delay(10);
    
    mySerial.begin(SerialSpeed);
    delay(10);
    
    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

int value = 0;

void read_seriral_data(){
  int counter=0;
   while (mySerial.available()) {
     char header = mySerial.read();
     
     Serial.write("rh=");
     Serial.write(header);
     Serial.write(" rv= ");
     
     if (header == '\n') {
       //Serial.write("return");
       //mySerial.flush();
       //break;
     }
     else if (header == 'A') {
       ir0 = mySerial.read();
       Serial.write(ir0);
     }
     else if (header == 'B') {
       ir1 = mySerial.read();
       Serial.write(ir1);
     }
     else if (header == 'C') {
       ir2 = mySerial.read();
       Serial.write(ir2);
     }
     else if (header == 'D') {
       ir3 = mySerial.read();
       Serial.write(ir3);
     }
     else if (header == 'X') {
       humid0 = mySerial.read();
       Serial.write(humid0);
     }
     else if (header == 'Y') {
       humid1 = mySerial.read();
       Serial.write(humid1);
     }
     /*
     if (counter > 100) {
       mySerial.flush();
       break;
    }
    */
     //counter = counter + 1;
  }
  //Serial.println("");
}

void loop()
{
    delay(300);
    ++value;

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
      
    read_seriral_data();

    String url = make_url();     
    
    request_http(client,url);
    fetch_response(client);
    
}

void request_http(WiFiClient client, String url){
    delay(3000);
    Serial.print("Requesting URL: ");
    Serial.println(url);
    // This will send the request to the server
    client.print(String("GET ") + url +" HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: Mozilla/4.0 (Compatible; MSIE 6.0; Windows NT 5.1;)\r\n" +
                 "Connection: close\r\n\r\n");  
}

void fetch_response(WiFiClient client){
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop(); 
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
}



String make_url(){
  String url = "/sensor.php";
    url += "?humid=";
    url += humid0;
    //url += "&humid1=";
    //url += humid1;
    url += "&ir0=";
    url += ir0;
    url += "&ir1=";
    url += ir1;
    url += "&ir2=";
    url += ir2;
    url += "&ir3=";
    url += ir3;
   return url;
}


