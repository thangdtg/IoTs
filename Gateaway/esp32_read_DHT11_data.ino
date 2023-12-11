#include "DHT.h"
#include "PubSubClient.h" 

#include "WiFi.h" 
#define DHTPIN 4 

#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

// WiFi
const char* ssid = "Thang ";                 // SSID
const char* wifi_password = "12345678"; // password

// MQTT
const char* mqtt_server = "192.168.1.12";  // IP cua MQTT broker
const char* humidity_topic = "home/livingroom/humidity";
const char* temperature_topic = "home/livingroom/temperature";
const char* mqtt_username = "vthang"; // MQTT username
const char* mqtt_password = "vthang"; // MQTT password
const char* clientID = "client_livingroom"; // MQTT client ID


WiFiClient wifiClient;
// port de ket noi voi broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Ham chon Wifi de ket noi MQTT broker thong qua Wifi 
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Ket noi wifi
  WiFi.begin(ssid, wifi_password);

  // Doi ket noi wifi thanh cong va tiep tuc
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Ket noi MQTT Broker
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  // MQTT chi co the truyen chuoi, nen phai chuyen doi gia tri sang chuoi
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" C ";

  // PUBLISH den MQTT Broker (topic = Temperature)
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }

  // Kiem tra neu khong the gui nhiet do, kiem tra ket noi MQTT Broker va thu lai
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperature_topic, String(t).c_str());
  }

  // PUBLISH den MQTT Broker (topic = Humidity)
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
  // Kiem tra neu khong the gui do am, kiem tra ket noi MQTT Broker va thu lai
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // Delay de dam bao client.publish khong xung dot khi thuc hien lenh client.connect
    client.publish(humidity_topic, String(h).c_str());
  }
  client.disconnect();  // ngat ket noi MQTT broker
  delay(1000*60);       // in gia tri moi 60s
}