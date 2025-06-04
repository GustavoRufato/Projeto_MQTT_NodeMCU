#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

const char* ssid = "dead.spades";
const char* password = "deadspades";
const char* mqtt_server = "192.168.43.2"; 
const int mqtt_port = 1883;

const char* sub_topic_cocina = "casa/cozinha/led/set";
const char* pub_topic_cocina = "casa/cozinha/led/status";

const char* sub_topic_quarto1 = "casa/quarto1/led/set";
const char* pub_topic_quarto1 = "casa/quarto1/led/status";

const char* sub_topic_quarto2 = "casa/quarto2/led/set";
const char* pub_topic_quarto2 = "casa/quarto2/led/status";

const char* sub_topic_varanda = "casa/varanda/led/set";
const char* pub_topic_varanda = "casa/varanda/led/status";

const uint8_t LED_PIN = D4;


WiFiClient espClient;
PubSubClient client(espClient);

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

  if (msg == "ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("Recebido 'ON' em ");
    Serial.println(topic);
  } else if (msg == "OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.print("Recebido 'OFF' em ");
    Serial.println(topic);
  } else {
    Serial.print("Comando desconhecido: ");
    Serial.println(msg);
    return; 
  }

  String estado_json = "{\"estado\":\"" + msg + "\"}";

  if (String(topic) == sub_topic_cocina) {
    client.publish(pub_topic_cocina, estado_json.c_str(), true);
  } else if (String(topic) == sub_topic_quarto1) {
    client.publish(pub_topic_quarto1, estado_json.c_str(), true);
  } else if (String(topic) == sub_topic_quarto2) {
    client.publish(pub_topic_quarto2, estado_json.c_str(), true);
  } else if (String(topic) == sub_topic_varanda) {
    client.publish(pub_topic_varanda, estado_json.c_str(), true);
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

      client.subscribe(sub_topic_cocina);
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
  Serial.println();
  Serial.println("=== Iniciando NodeMCU + MQTT + OTA ===");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  setup_wifi();
  setup_ota();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle(); 
}
