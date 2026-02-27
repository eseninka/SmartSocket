#include <WiFi.h>
#include <PubSubClient.h>
#include "read_AC.h"
#include <WiFiClient.h>
#include "Cl_timestamp.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C  // Чаще всего 0x3C для 128x64


// Настройки Wi-Fi
// const char* ssid = "ForEsp32";
// const char* password = "aztj5781";

// const char* ssid = "WI-FI";
// const char* password = "6LpEL3nx";

const char* ssid = "CPOD";
const char* password = "ApoX51s42wR7FDK8";

// Настройки MQTT
const char* mqtt_server = "m5.wqtt.ru";
const int mqtt_port = 14182;
const char* mqtt_user = "Rasbery";
const char* mqtt_pass = "154321";
const char* mqtt_topic_pub = "kvant/R22/BV/update";
const char* mqtt_topic_sub = "kvant/R22/BV/reception";  // для управление LED


WiFiClient espClient;
PubSubClient client(espClient);
timeSt test_send_time;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


read_AC read_ac;

bool mqttConnected = false;  // Флаг подключения

void setup() {
  Serial.begin(115200);
  delay(5000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while (true) {
      delay(1000);
      Serial.println("Display not found!");
    }
  }

  delay(1000);

  client.setCallback(callback);  // Функция обработки входящих сообщений
  delay(2000);

  read_ac.initialization();
  test_send_time.timeSetting("pool.ntp.org", 3 * 3600, 0);  // для timestamp | GMT+3 (Москва) = 3 * 3600 секунд, Летнее время (0, если не используется)
  Serial.println("Display OK!");
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(25, 20);
  display.println("SS");
  display.display();

  delay(1000);

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(4096);  // Увеличиваем буфер сообщений
}

void loop() {
  
  if (!client.connected()) {
    mqttConnected = false;
    reconnectMQTT();
  } else {
    if (!mqttConnected) {
      mqttConnected = true;
      Serial.println("MQTT подключён стабильно!");
    }
  }

  client.loop();
  //Публикация сообещния
  read_ac.read_current_and_voltage();
  String json = create_json(read_ac.current_A, read_ac.voltage_V, read_ac.rms_A, read_ac.rms_voltage);
  Serial.println(json);
  client.publish(mqtt_topic_pub, json.c_str());

  delay(5000);
}
// Подключение к WiFi
void setupWiFi() {
  Serial.println();

  Serial.print("Подключение к ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();
  delay(1000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi подключён");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi OK!");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
  delay(1000);
}

//Подключение к серверу
void reconnectMQTT() {
  static unsigned long lastAttempt = 0;

  // Пытаемся переподключаться не чаще 1 раза в 5 секунд
  if (millis() - lastAttempt < 5000) {
    return;
  }
  lastAttempt = millis();

  Serial.print("Попытка подключения к MQTT...");

  // Случайный ID клиента для избежания конфликтов
  String clientId = "ESP32-" + String(random(0xFFFF), HEX);

  if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    Serial.println("Успешно!");
    delay(500);
    client.subscribe(mqtt_topic_sub);
  } else {
    Serial.print("Ошибка, rc=");
    Serial.println(client.state());
  }
}

String create_json(float A[20], float V[20], float rms_A, float rms_V) {
  test_send_time.timeStam();

  String As = "";
  String Vs = "";

  for (int i = 0; i < 20; i++) {
    As += A[i];
    Vs += V[i];
    if (i != 19) {
      As += ", ";
      Vs += ", ";
    }
  }

  String jsonchik = "{";
  jsonchik += "\"key\":\"info_about_AC\",";
  jsonchik += "\"uuid\":\"c8b6b828-7bef-4aa4-83a4-ed0fbc27e917\",";
  //jsonchik += "\"timestamp\":" + String(test_send_time.timeS) + ",";
  jsonchik += "\"timestamp\":" + String(test_send_time.timeS) + ",";
  jsonchik += "\"amper\": { \"data\":[" + As + "]},";
  jsonchik += "\"voltage\": { \"data\":[" + Vs + "]},";
  jsonchik += "\"rms_A\":" + String(rms_A) + ",";
  jsonchik += "\"rms_V\":" + String(rms_V);
  jsonchik += "}";

  return jsonchik;
}

void update_oled(float P, float A_rms, float V_rms, float money, float cosfi) {
  String data = "P = " + String(P) + " W\n";
  data += "I = " + String(A_rms) + " A\n";
  data += "V = " + String(V_rms) + " V\n";
  data += "Money = " + String(money) + " rub\n";
  data += "cosfi = " + String(cosfi) + "\n";
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("SmartSocket\n");
  display.println(data);
  display.display();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  float data[5] = {doc["P"], doc["I_mg"], doc["V_mg"], doc["money"], doc["cosfi"]};
  update_oled(data[0], data[1], data[2], data[3], data[4]);

}