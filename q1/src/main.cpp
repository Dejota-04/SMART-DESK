/*
  PROJETO: SMART DESK (HÍBRIDO) - VERSÃO FINAL
  DISCIPLINA: IOT - FIAP
  DESCRIÇÃO: Simulação de dados ergonômicos com envio via HTTP (ThingSpeak) e MQTT (Node-RED).
  FIX: Implementação de ID MQTT Dinâmico para evitar desconexões no broker público.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <math.h>

// ==========================================
// CONFIGURAÇÕES DO THINGSPEAK
// ==========================================
String apiKey = "6X292TRG9LPRWZ4P";
const char* serverName = "http://api.thingspeak.com/update";

// ==========================================
// CONFIGURAÇÕES DO MQTT (Para Node-RED)
// ==========================================
const char *BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char *TOPIC_PUBLISH = "smartdesk/medicoes";

// ==========================================
// CONFIGURAÇÕES GERAIS (Wokwi)
// ==========================================
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";
#define PIN_LED 15
#define PUBLISH_DELAY 20000 // 20s (Respeitando limite do ThingSpeak)

// Objetos Globais
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Variáveis de Controle
unsigned long publishUpdate = 0;
int contadorTempoSentado = 0;
float anguloSimulacao = 0.0;

// --- Função para gerar ID Único (Baseado no MAC Address) ---
String getDeviceUUID() {
  uint64_t chipid = ESP.getEfuseMac();
  char uuid[13];
  snprintf(uuid, 13, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
  return String(uuid);
}

void initWiFi() {
  Serial.print("Conectando WiFi: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi Conectado!");
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando MQTT... ");

    // --- FIX: ID DINÂMICO ---
    // Gera um nome único tipo: "SmartDesk_A1B2C3D4"
    // Isso evita que o Broker derrube a conexão por conflito de nomes
    String clientId = "SmartDesk_" + getDeviceUUID();

    if (MQTT.connect(clientId.c_str())) {
      Serial.print("Conectado! (ID: ");
      Serial.print(clientId);
      Serial.println(")");
    } else {
      Serial.print("Falha. Rc=");
      Serial.print(MQTT.state());
      Serial.println(" Tentando em 2s...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  initWiFi();
  initMQTT();
}

void loop() {
  // Garante conexões
  if (WiFi.status() != WL_CONNECTED) initWiFi();
  if (!MQTT.connected()) reconnectMQTT();

  MQTT.loop(); // Mantém a conexão MQTT viva

  // Timer de 20 segundos
  if ((millis() - publishUpdate) >= PUBLISH_DELAY) {
    publishUpdate = millis();

    // ####### 1. SIMULAÇÃO DE DADOS #######
    anguloSimulacao += 0.2;

    // Temperatura (Simula oscilação entre 19 e 29)
    float temp = 24.0 + (5.0 * sin(anguloSimulacao));

    // Iluminação (Simula oscilação)
    int lux = (int)(350.0 + (300.0 * sin(anguloSimulacao * 0.5)));

    // Altura Tela (Simula ajuste de mesa)
    float altura = 115.0 + (20.0 * sin(anguloSimulacao * 0.2));

    // Tempo Sentado (Contador cíclico)
    contadorTempoSentado += 20;
    if (contadorTempoSentado > 90) contadorTempoSentado = 0;

    // Postura (Simulação baseada na onda)
    int posturaNumerica = 0;
    String posturaTexto = "CORRETA";
    float ondaPostura = sin(anguloSimulacao * 0.3);

    if (ondaPostura > 0.3) {
        posturaNumerica = 0;
        posturaTexto = "CORRETA";
    } else if (ondaPostura > -0.3) {
        posturaNumerica = 1;
        posturaTexto = "INCLINADA";
    } else {
        posturaNumerica = 2;
        posturaTexto = "CURVADO";
    }

    String myUUID = getDeviceUUID();

    // ####### 2. ENVIO VIA HTTP (Para o ThingSpeak) #######

    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Formato URL Encoded pro ThingSpeak
    String httpRequestData = "api_key=" + apiKey +
                             "&field1=" + String(temp) +
                             "&field2=" + String(lux) +
                             "&field3=" + String(contadorTempoSentado) +
                             "&field4=" + String(altura) +
                             "&field5=" + String(posturaNumerica) +
                             "&field6=" + myUUID;

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      Serial.print("ThingSpeak (HTTP): OK (");
      Serial.print(httpResponseCode);
      Serial.println(")");
    } else {
      Serial.print("ThingSpeak (HTTP): Erro ");
      Serial.println(httpResponseCode);
    }
    http.end();

    // ####### 3. ENVIO VIA MQTT (Para o Node-RED) #######

    // Cria um JSON bonito para o Node-RED ler fácil
    JsonDocument doc;
    doc["uuid"] = myUUID;
    doc["temperatura"] = temp;
    doc["iluminacao"] = lux;
    doc["tempo_sentado"] = contadorTempoSentado;
    doc["altura_tela"] = altura;
    doc["postura_id"] = posturaNumerica;
    doc["postura_desc"] = posturaTexto;

    char mqttBuffer[512];
    serializeJson(doc, mqttBuffer);

    MQTT.publish(TOPIC_PUBLISH, mqttBuffer);
    Serial.println("Broker MQTT (JSON): Enviado!");
    Serial.println(mqttBuffer);
    Serial.println("----------------------------------");
  }
}