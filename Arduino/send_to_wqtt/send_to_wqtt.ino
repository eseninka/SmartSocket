#include <WiFi.h>
#include <PubSubClient.h>
#include "read_AC.h"
#include <WiFiClient.h>
#include "Cl_timestamp.h"

// Настройки Wi-Fi
const char* ssid = "WI-FI";
const char* password = "6LpEL3nx";


// Настройки MQTT
const char* mqtt_server = "m5.wqtt.ru";
const int mqtt_port = 14182;
const char* mqtt_user = "Rasbery";
const char* mqtt_pass = "154321";
const char* mqtt_topic_pub = "kvant/R22/BV/update";
//const char* mqtt_topic_sub = "electro/wqtt/esp32/led";  // для управление LED


WiFiClient espClient;
PubSubClient client(espClient);
timeSt test_send_time;


read_AC read_ac;

bool mqttConnected = false;  // Флаг подключения

void setup() {
  Serial.begin(115200);
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(4096);  // Увеличиваем буфер сообщений
  //client.setCallback(callback);  // Функция обработки входящих сообщений
  read_ac.initialization();
  test_send_time.timeSetting("pool.ntp.org", 3 * 3600, 0);  // для timestamp | GMT+3 (Москва) = 3 * 3600 секунд, Летнее время (0, если не используется)
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
  read_ac.read_current();
  read_ac.read_voltage();
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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi подключён");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
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
    //client.subscribe(mqtt_topic_sub);
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
/*
// обработка входящих MQTT-сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Получено сообщение [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Управление LED
  if (String(topic) == mqtt_topic_sub) {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);
      ledState = true;  // Сохраняем состояние
      client.publish(mqtt_topic_status, "ON", true);  // retain=true
    } 
    else if (message == "OFF") {
      digitalWrite(ledPin, LOW);
      ledState = false;  // Сохраняем состояние
      client.publish(mqtt_topic_status, "OFF", true);  // retain=true
    }
  }
}
*/