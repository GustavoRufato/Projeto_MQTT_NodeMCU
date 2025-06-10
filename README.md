# Controle de LEDs via MQTT com NodeMCU ESP8266

Este projeto utiliza um **NodeMCU ESP8266** para controle de LEDs em diferentes cômodos via MQTT.  
Ele também suporta atualização Over-the-Air (OTA) e foi testado com o **MQTT Explorer** como cliente.

---

## Sumário
- [Visão Geral](#visão-geral)
- [Estrutura de Pastas](#estrutura-de-pastas)
- [Pré-requisitos](#pré-requisitos)
- [Instalação e Configuração](#instalação-e-configuração)
  - [Drivers do NodeMCU](#drivers-do-nodemcu)
  - [PlatformIO](#platformio)
  - [Mosquitto (Broker MQTT)](#mosquitto-broker-mqtt)
  - [MQTT Explorer (Cliente Gráfico)](#mqtt-explorer-cliente-gráfico)
- [Código Fonte](#código-fonte)
  - [Configurações Wi-Fi e MQTT](#configurações-wi-fi-e-mqtt)
  - [Callback e Controle do LED](#callback-e-controle-do-led)
- [Estrutura de Tópicos](#estrutura-de-tópicos)
- [Como Compilar e Carregar](#como-compilar-e-carregar)
- [Testes e Exemplos](#testes-e-exemplos)
- [Estrutura de Tópicos por Cômodo](#estrutura-de-tópicos-por-cômodo)
- [Contribuindo](#contribuindo)

---

## Visão Geral

Este projeto tem como objetivo controlar **4 LEDs** via tópicos MQTT, representando diferentes cômodos de uma casa:

✅ Cozinha  
✅ Quarto 1  
✅ Quarto 2  
✅ Varanda

O NodeMCU conecta-se a uma rede Wi-Fi, escuta comandos em tópicos MQTT específicos e publica o estado de cada LED.

Ele também conta com suporte a **atualização OTA**, facilitando a manutenção do firmware.

---

## Estrutura de Pastas

```
Projeto_MQTT_NodeMCU/
├── .gitignore
├── README.md
├── platformio.ini
└── src/
    └── main.cpp
```

---

## Pré-requisitos

- **NodeMCU ESP8266**
- Protoboard e LEDs conectados aos pinos:
  - **D1 (GPIO5)** - Cozinha  
  - **D2 (GPIO4)** - Quarto 1  
  - **D3 (GPIO0)** - Quarto 2  
  - **D4 (GPIO2)** - Varanda
- **Broker MQTT** (ex.: Mosquitto)
- **Cliente MQTT** para testes (ex.: MQTT Explorer)

---

## Instalação e Configuração

### Drivers do NodeMCU
Baixe e instale os drivers para o chip **CP2102** (ou **CH340**) do seu NodeMCU.

### PlatformIO
1. Instale o PlatformIO como extensão do VS Code ou standalone.
2. Clone este repositório ou baixe o arquivo ZIP.

### Mosquitto (Broker MQTT)
- Instale e configure o Mosquitto em seu computador (Linux ou Windows).
- Configure o broker para escutar na porta **1883**.

### MQTT Explorer (Cliente Gráfico)
[Baixe e instale o MQTT Explorer para visualizar e interagir com os tópicos.](https://mqtt-explorer.com/)

---

## Código Fonte

### 📂 Configurações Wi-Fi e MQTT

```cpp
const char* ssid = "NomeDaRede";
const char* password = "SenhaDaRede";
const char* mqtt_server = "ipDaSuaMaquina";
const int mqtt_port = 1883;
```

### 📂 Callback e Controle do LED

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.toUpperCase();

  bool estado = (msg == "ON");

  String estado_json = "{\"estado\":\"" + msg + "\"}";

  if (String(topic) == sub_topic_cocina) {
    digitalWrite(LED_COCINA, estado ? HIGH : LOW);
    client.publish(pub_topic_cocina, estado_json.c_str(), true);
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
```

---

## Estrutura de Tópicos

### Subscrição (para controle)
- `casa/cozinha/led/set`
- `casa/quarto1/led/set`
- `casa/quarto2/led/set`
- `casa/varanda/led/set`

### Publicação (estado atual)
- `casa/cozinha/led/status`
- `casa/quarto1/led/status`
- `casa/quarto2/led/status`
- `casa/varanda/led/status`

---

## Como Compilar e Carregar

1️⃣ Abra o projeto no PlatformIO.  
2️⃣ Conecte o NodeMCU ao computador.  
3️⃣ Selecione a porta serial correta.  
4️⃣ Clique em **Upload** (ícone de seta para a direita).  

### OTA (opcional)
Com OTA habilitado, futuras atualizações podem ser feitas via Wi-Fi, sem cabo.

---

## Testes e Exemplos

Use o **MQTT Explorer** para enviar mensagens para:
- `casa/cozinha/led/set`
- `casa/quarto1/led/set`
- `casa/quarto2/led/set`
- `casa/varanda/led/set`

Envie **ON** ou **OFF** como payload para controlar os LEDs.

---

## Estrutura de Tópicos por Cômodo

✅ Cozinha:  
- Set: `casa/cozinha/led/set`  
- Status: `casa/cozinha/led/status`

✅ Quarto 1:  
- Set: `casa/quarto1/led/set`  
- Status: `casa/quarto1/led/status`

✅ Quarto 2:  
- Set: `casa/quarto2/led/set`  
- Status: `casa/quarto2/led/status`

✅ Varanda:  
- Set: `casa/varanda/led/set`  
- Status: `casa/varanda/led/status`

---

## Contribuindo

Contribuições são bem-vindas! Abra uma issue ou envie um pull request.  
