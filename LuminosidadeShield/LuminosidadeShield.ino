
#include <EtherCard.h>

#define APIKEY "QILUO7XCXI6BGLKT" // put your key here
#define ethCSpin 10 // put your CS/SS pin here.

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
const char website[] PROGMEM = "api.thingspeak.com";
byte Ethernet::buffer[700];
uint32_t timer;
Stash stash;
byte session;

int res = 100; // was 0
int sensor = 0;

void setup () {
  Serial.begin(9600);
  Serial.println("\n[ThingSpeak example]");

  //Initialize Ethernet
  initialize_ethernet();
}

void loop () { 
  //if correct answer is not received then re-initialize ethernet module
  if (res > 220){
    initialize_ethernet(); 
  }
  res = res + 1;
  ether.packetLoop(ether.packetReceive());
  //200 res = 10 seconds (50ms each res)
  if (res == 200) {
    //Generate random info
    int demo = analogRead(sensor);
    delay(1000);
    String msje;

    if (demo < 250){
      msje = "low";
    }
    else{
      msje = "high";
    }

    byte sd = stash.create();
    stash.print("field1=");
    stash.print(demo);
    //stash.print("&field2=");
    //stash.print(one);
    stash.save();

    //Display data to be sent
    Serial.println(demo);


    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("POST /update HTTP/1.0" "\r\n"
      "Host: $F" "\r\n"
      "Connection: close" "\r\n"
      "X-THINGSPEAKAPIKEY: $F" "\r\n"
      "Content-Type: application/x-www-form-urlencoded" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    session = ether.tcpSend(); 

 // added from: http://jeelabs.net/boards/7/topics/2241
 int freeCount = stash.freeCount();
    if (freeCount <= 3) {   Stash::initMap(56); } 
  }
  
   const char* reply = ether.tcpReply(session);
   
   if (reply != 0) {
     res = 0;
     Serial.println(F(" >>>REPLY recieved...."));
     Serial.println(reply);
   }
   delay(300);
}

void initialize_ethernet(void){  
  for(;;){ // keep trying until you succeed 

    if (ether.begin(sizeof Ethernet::buffer, mymac, ethCSpin) == 0){ 
      Serial.println( "Failed to access Ethernet controller");
      continue;
    }
    
    if (!ether.dhcpSetup()){
      Serial.println("DHCP failed");
      continue;
    }

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);  
    ether.printIp("DNS: ", ether.dnsip);  

    if (!ether.dnsLookup(website))
      Serial.println("DNS failed");

    ether.printIp("SRV: ", ether.hisip);

    //reset init value
    res = 180;
    break;
  }
}
