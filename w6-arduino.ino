#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D4         // Pin where the DHT11 is connected
#define DHTTYPE DHT11      // DHT 11
#define LEDPIN D6          // เลือกพินอื่นสำหรับ LED

const char *ssid = "มองทำไม";
const char *password = "223334444";
const char *mqtt_server = "192.168.16.74";
const char *dht_topic = "DHT11";
const char *led_topic = "LED";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(100);
  Serial.begin(115200);
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(String(topic));

  String command = "";
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }

  command.trim(); 
  Serial.print("Command received: ");
  Serial.println(command);

  if (command == "On") {
    digitalWrite(LEDPIN, HIGH);
    Serial.println("LED turned ON");
  } else if (command == "Off") {
    digitalWrite(LEDPIN, LOW);
    Serial.println("LED turned OFF");
  } else {
    Serial.println("Unknown command");
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(led_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LEDPIN, OUTPUT);  // ตั้งค่าพิน LED เป็น OUTPUT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.println("Humidity: " + String(humidity) + "%, Temperature: " + String(temperature) + "°C");

  if (!isnan(humidity) && !isnan(temperature)) {
    String dhtData = "Humidity: " + String(humidity) + "%, Temperature: " + String(temperature) + "°C";
    client.publish(dht_topic, dhtData.c_str());
  }

  delay(7000);  // Wait for 5 seconds before sending DHT11 data again
}