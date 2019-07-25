#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#define RST_PIN   9                            // reset핀은 9번으로 설정
#define SS_PIN    10                           // SS핀은 10번으로 설정
                                               // SS핀은 데이터를 주고받는 역할의 핀( SS = Slave Selector )

#ifndef STASSID
#define STASSID "CECOM_2.4G"
#define STAPSK  "2579*2579*"
#endif

MFRC522 mfrc(SS_PIN, RST_PIN);                 // MFR522를 이용하기 위해 mfrc객체를 생성해 줍니다.

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);



void handleRoot() {
 
  server.send(200, "text/plain", "hello from esp8266!");
  
}

void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void setup(){
  Serial.begin(115200);                        
  SPI.begin();                                // SPI 초기화
                                              // (SPI : 하나의 마스터와 다수의 SLAVE(종속적인 역활)간의 통신 방식)
  mfrc.PCD_Init(); 
 
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
                                
}

void loop(){
  server.handleClient();
  MDNS.update();
  if ( !mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial() ) {   
                                               // 태그 접촉이 되지 않았을때 또는 ID가 읽혀지지 않았을때
    delay(500);                                // 0.5초 딜레이 
    return;                                    // return
  } 
    
  Serial.print("Card UID:");                  // 태그의 ID출력
  
  for (byte i = 0; i < 4; i++) {               // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
    Serial.print(mfrc.uid.uidByte[i]);        // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
    Serial.print(" ");                        // id 사이의 간격 출력
  }
  Serial.println(); 
  
}
