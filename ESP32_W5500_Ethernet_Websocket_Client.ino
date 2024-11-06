#include "defines.h"
#include <Ticker.h>
#include <WebSockets2_Generic.h>

using namespace websockets2_generic;

WebsocketsClient client;

////////////////////////////////////////////////////

// Select the IP address according to your local network
IPAddress serverIP(192, 168, 1, 63);
IPAddress clientIP(192, 168, 1, 220);
//IPAddress dns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char* websockets_server_host = "192.168.1.63"; //Enter server address
//const char* websockets_server_host = "serverip_or_name"; //Enter server address

#define WEBSOCKETS_PORT   8085

const uint16_t websockets_server_port = WEBSOCKETS_PORT; // Enter server port

/////////////////////////////////////////////////////////
///////////////////DINH NGHIA CAC BIEN///////////////////

//Chuoi - Strings
String msg_data = String();

String msg, code_received;
unsigned int lastStringLength = msg_data.length();  // previous length of the String

char mode = '0';
char mode_real;
unsigned int last_mode;

//LED - Duty Circle - Siren
const int PIN_LED = 32;
const int PIN_SIREN = 33;

const int resolution = 8; // Độ phân giải (bit)

const int freq1 = 3; // Tần số PWM (Hz)
const int freq2 = 20; // Tần số PWM (Hz)

/* Co tong cong 16 channel */
const int ledChannel1 = 1; // Kênh LEDC 1
const int ledChannel2 = 2; // Kênh LEDC 2
const int ledChannel3 = 3; // Kênh LEDC 3
const int ledChannel4 = 4; // Kênh LEDC 4
const int ledChannel5 = 5; // Kênh LEDC 5

bool State = 0;
#define TimeSirenDelay1 50 
#define TimeSirenDelay2 400 
#define TimeSirenDelay3 1000 

unsigned long pMillis = 0;

Ticker R_mode;
Ticker Y_mode;
Ticker G_mode;

////////////////////////////////////////////////////////

void onEventsCallback(WebsocketsEvent event, String data) 
{
  (void) data;
  
  if (event == WebsocketsEvent::ConnectionOpened) 
  {
    Serial.println("Connnection Opened");
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) 
  {
    Serial.println("Connnection Closed");
  } 
  else if (event == WebsocketsEvent::GotPing) 
  {
    Serial.println("Got a Ping!");
  } 
  else if (event == WebsocketsEvent::GotPong) 
  {
    Serial.println("Got a Pong!");
  }
}

void initEthernet()
{ 
#if (USING_SPI2)
  #if defined(CUR_PIN_MISO)
    LOGWARN(F("Default SPI pinout:"));
    LOGWARN1(F("MOSI:"), CUR_PIN_MOSI);
    LOGWARN1(F("MISO:"), CUR_PIN_MISO);
    LOGWARN1(F("SCK:"),  CUR_PIN_SCK);
    LOGWARN1(F("SS:"),   CUR_PIN_SS);
    LOGWARN(F("========================="));
  #endif
#else
  LOGWARN(F("Default SPI pinout:"));
  LOGWARN1(F("MOSI:"), MOSI);
  LOGWARN1(F("MISO:"), MISO);
  LOGWARN1(F("SCK:"),  SCK);
  LOGWARN1(F("SS:"),   SS);
  LOGWARN(F("========================="));
#endif

// You can use Ethernet.init(pin) to configure the CS pin
//Ethernet.init(10);  // Most Arduino shields
//Ethernet.init(5);   // MKR ETH shield
//Ethernet.init(0);   // Teensy 2.0
//Ethernet.init(20);  // Teensy++ 2.0
//Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
//Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

#ifndef USE_THIS_SS_PIN
  #define USE_THIS_SS_PIN   5   //22    // For ESP32
#endif

LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);

// For other boards, to change if necessary
#if ( USE_ETHERNET_GENERIC || USE_ETHERNET_ENC )
  // Must use library patch for Ethernet, EthernetLarge libraries
  // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
  // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3

  //Ethernet.setCsPin (USE_THIS_SS_PIN);
  Ethernet.init (USE_THIS_SS_PIN);

#elif USE_CUSTOM_ETHERNET

  // You have to add initialization for your Custom Ethernet here
  // This is just an example to setCSPin to USE_THIS_SS_PIN, and can be not correct and enough
  Ethernet.init(USE_THIS_SS_PIN); 

#endif  //( USE_ETHERNET_GENERIC || USE_ETHERNET_ENC )

/////////////////////////////////////

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;
  // Use Static IP
  //Ethernet.begin(mac[0], clientIP, dns, gateway, subnet);
  Ethernet.begin(mac[index]);

#if USE_ETHERNET_GENERIC

  LOGWARN(F("========================="));
  
  // Just info to know how to connect correctly
  // To change for other SPI
  LOGWARN(F("Currently Used SPI pinout:"));
  LOGWARN1(F("MOSI:"), PIN_MOSI);
  LOGWARN1(F("MISO:"), PIN_MISO);
  LOGWARN1(F("SCK:"),  PIN_SCK);
  LOGWARN1(F("SS:"),   PIN_SS);
  
  LOGWARN(F("========================="));
  
#endif

  Serial.print(F("Using mac index = "));
  Serial.println(index);
}

void setup()
{
#if (defined(ETHERNET_WITH_SD_CARD) && ETHERNET_WITH_SD_CARD)
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card
#endif

  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(500);

  ledcAttachChannel(PIN_LED, 5000, resolution, ledChannel1);
  ledcAttachChannel(PIN_SIREN, 5000, resolution, ledChannel2);

  Serial.println("\nStarting ESP32_Ethernet-Client on " + String(BOARD_NAME));
  Serial.println("Ethernet using " + String(ETHERNET_TYPE));
  Serial.println(WEBSOCKETS2_GENERIC_VERSION);

  initEthernet();

  Serial.print("WebSockets Client IP address: ");
  Serial.println(Ethernet.localIP());

  Serial.print("Connecting to WebSockets Server @");
  Serial.println(websockets_server_host);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  
  delay(1000);

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message)
  {
    //Tin nhan nhan tu server
    Serial.print("Tin nhan tu Server: ");
    //Serial.println(message.data());
    msg_data = message.data();
    unsigned int lastStringLength = msg_data.length();  // previous length of the String
    Serial.println(msg_data);
    //Ma code nhan duoc tu tin nhan server
    code_received = tach_chuoi(msg_data, lastStringLength);
    Serial.println();
    Serial.print("Code da tach: ");
    Serial.println(code_received);
    //Kiem tra mode tu code nhan duoc
    mode_real = check_mode(code_received);
    Serial.print("Thuc thi Mode: ");
    Serial.println(mode_real);
    // Thuc thi Event
    chooose_zone(mode_real);
    // Kiem tra thay doi trang thai
    State = check_change_state(mode_real, last_mode);
    last_mode = mode_real;
    

  });

  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  // try to connect to Websockets server

  client.connect("ws://192.168.1.222:8085/ws"); 
  bool connected = client.connect("ws://192.168.1.222:8085/ws");
  put_msg_to_sever(connected);
  
}

void loop() 
{
  // let the websockets client check for incoming messages
  if (client.available()) 
  {
    client.poll();
  }
  
  
  
  

}

void put_msg_to_sever(bool connected){
  if (connected){
    Serial.println("Connected!");
    String WS_msg = "get_warning";
    client.send(WS_msg);
    Serial.print("Tin nhan da gui cho Server: ");
    Serial.println(WS_msg);
  }
}

String tach_chuoi(String txtMsg, unsigned int lastStringLength){
  char Significant;
  String msg = String();
  for(int j=4; j>1; j--){ // chuoi 3 ki tu cuoi
    Significant = txtMsg.charAt(lastStringLength-j);
    Serial.print(Significant);
    msg += Significant;
  }
  return msg;
}

char check_mode(String data_msg){
  char mode;
  // CHECK MODE                  //
  // '0' - NO WARNING            //
  // '1' = RED - HIGH WARNING    //
  // '2' = YELLOW - WARNING      //
  // '3' = GREEN - LOW WARNING   //
  if(data_msg == "256"){
      Serial.println("Style 1");
      mode = '1';
    }
    else if(data_msg == "257"){
      Serial.println("Style 2");
      mode = '2';
    }
    else if(data_msg == "258"){
      Serial.println("Style 3");
      mode = '3';
    }
    else{
      Serial.println("Style 0");
      mode = '0';
    }

  return mode;
}

unsigned int check_change_state(char mode, unsigned int last_mode){
  unsigned int state_mode = mode - '0'; //chuyen char sang int
  if(state_mode != last_mode){       //xac dinh co su thay doi giua cac zone
    State = 1;
  }
  return State;
}

void high_warning(){
  if(State == 1){
    analogWrite(PIN_LED, 255);
    ledcWriteNote(PIN_SIREN, NOTE_D, 4);
    State = 0;
  }
  else{
    analogWrite(PIN_LED, 0);
    ledcWrite(PIN_SIREN, 0);
    State = 1;
  }
}

void warning(){
  if(State == 1){
    ledcWriteNote(PIN_SIREN, NOTE_E, 3);
    analogWrite(PIN_LED, 191);
    State = 0;
  }
  else{
    analogWrite(PIN_LED, 0);
    ledcWrite(PIN_SIREN, 0);
    State = 1;
  }
}

void low_warning(){
  if(State == 1){
    ledcWriteNote(PIN_SIREN, NOTE_D, 2);
    analogWrite(PIN_LED, 128);
    State = 0;
  }
  else{
    analogWrite(PIN_LED, 0);
    ledcWrite(PIN_SIREN, 0);
    State = 1;
  }
}


void zone0() {
  Serial.println("WHITE");
  ledcWrite(PIN_SIREN, 0);
  analogWrite(PIN_LED, 0);
}

void chooose_zone(char mode) {
  int choose = mode - '0';
  switch (choose) {
    case 0:
      zone0();
      R_mode.detach();
      Y_mode.detach();
      G_mode.detach();
      break;
    case 1:
      G_mode.attach(0.6, low_warning); // 1s
      Y_mode.detach();
      R_mode.detach();
      break;
    case 2:
      Y_mode.attach(0.4, warning); // 400ms
      G_mode.detach();
      R_mode.detach();
      break;
    case 3:
      R_mode.attach(0.1, high_warning); //100ms
      G_mode.detach();
      Y_mode.detach();
      break;
  }
  
}

