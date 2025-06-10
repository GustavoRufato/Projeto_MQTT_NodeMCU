#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

const char* ssid = "dead.spades";
const char* password = "deadspades";
const char* mqtt_server = "192.168.43.2";
const int mqtt_port = 1883;

const char* sub_topic_cozinha = "casa/cozinha/led/set";
const char* pub_topic_cozinha = "casa/cozinha/led/status";

const char* sub_topic_quarto1 = "casa/quarto1/led/set";
const char* pub_topic_quarto1 = "casa/quarto1/led/status";

const char* sub_topic_quarto2 = "casa/quarto2/led/set";
const char* pub_topic_quarto2 = "casa/quarto2/led/status";

const char* sub_topic_varanda = "casa/varanda/led/set";
const char* pub_topic_varanda = "casa/varanda/led/status";

const uint8_t LED_COZINHA = D1;
const uint8_t LED_QUARTO1 = D2;
const uint8_t LED_QUARTO2 = D3;
const uint8_t LED_VARANDA = D4;

WiFiClient espClient;
PubSubClient client(espClient);

bool testeLEDs = false;

void setup_wifi() {
  Serial.print("Conectando na rede Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Wi-Fi conectado! IP: ");
  Serial.println(WiFi.localIP());
}

void setup_ota() {
  ArduinoOTA.setHostname("esp8266-ota");

  ArduinoOTA.onStart([]() {
    Serial.println("Iniciando OTA...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA concluída!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progresso OTA: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Erro OTA [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha de autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Erro no início");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Erro de conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Erro de recebimento");
    else if (error == OTA_END_ERROR) Serial.println("Erro ao finalizar");
  });

  ArduinoOTA.begin();
  Serial.println("OTA pronto!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.toUpperCase();

  bool estado = (msg == "ON");

  String estado_json = "{\"estado\":\"" + msg + "\"}";

  if (String(topic) == sub_topic_cozinha) {
    digitalWrite(LED_COZINHA, estado ? HIGH : LOW);
    client.publish(pub_topic_cozinha, estado_json.c_str(), true);
    Serial.println("Cozinha: " + msg);
  } else if (String(topic) == sub_topic_quarto1) {
    digitalWrite(LED_QUARTO1, estado ? HIGH : LOW);
    client.publish(pub_topic_quarto1, estado_json.c_str(), true);
    Serial.println("Quarto1: " + msg);
  } else if (String(topic) == sub_topic_quarto2) {
    digitalWrite(LED_QUARTO2, estado ? HIGH : LOW);
    client.publish(pub_topic_quarto2, estado_json.c_str(), true);
    Serial.println("Quarto2: " + msg);
  } else if (String(topic) == sub_topic_varanda) {
    digitalWrite(LED_VARANDA, estado ? HIGH : LOW);
    client.publish(pub_topic_varanda, estado_json.c_str(), true);
    Serial.println("Varanda: " + msg);
  } else {
    Serial.print("Tópico desconhecido: ");
    Serial.println(topic);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "NodeMCU-";
    clientId += String(random(0xffff), HEX);
    Serial.print("Tentando conectar ao broker MQTT... ID = ");
    Serial.println(clientId);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker!");

      client.subscribe(sub_topic_cozinha);
      client.subscribe(sub_topic_quarto1);
      client.subscribe(sub_topic_quarto2);
      client.subscribe(sub_topic_varanda);
      Serial.println("Inscrito em todos os tópicos de controle!");
    } else {
      Serial.print("Falha na conexão. Código de erro: ");
      Serial.print(client.state());
      Serial.println(" – tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(50);
  Serial.println("\n=== Iniciando NodeMCU + MQTT + OTA ===");

  pinMode(LED_COZINHA, OUTPUT);
  pinMode(LED_QUARTO1, OUTPUT);
  pinMode(LED_QUARTO2, OUTPUT);
  pinMode(LED_VARANDA, OUTPUT);

  digitalWrite(LED_COZINHA, LOW);
  digitalWrite(LED_QUARTO1, LOW);
  digitalWrite(LED_QUARTO2, LOW);
  digitalWrite(LED_VARANDA, LOW);

  if (!testeLEDs) {
    setup_wifi();
    setup_ota();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
  }
}

void loop() {
  if (testeLEDs) {
    digitalWrite(LED_COZINHA, HIGH);
    Serial.println("LED Cozinha ON");
    delay(1000);
    digitalWrite(LED_COZINHA, LOW);

    digitalWrite(LED_QUARTO1, HIGH);
    Serial.println("LED Quarto1 ON");
    delay(1000);
    digitalWrite(LED_QUARTO1, LOW);

    digitalWrite(LED_QUARTO2, HIGH);
    Serial.println("LED Quarto2 ON");
    delay(1000);
    digitalWrite(LED_QUARTO2, LOW);

    digitalWrite(LED_VARANDA, HIGH);
    Serial.println("LED Varanda ON");
    delay(1000);
    digitalWrite(LED_VARANDA, LOW);
  } else {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    ArduinoOTA.handle();
  }
}
