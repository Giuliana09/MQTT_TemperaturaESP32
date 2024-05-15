#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Definições de rede
const char* ssid = "nome_rede"; // depois mudar para nome_rede
const char* password = "senha_rede"; // depois mudar para senha_rede

// Credenciais do servidor MQTT
const char* mqttServer = "ipServidorMQTT"; // depois mudar para ipServidorMQTT
const int mqttPort = 1883;
const char* mqttUser = "nomeUserMQTT"; // depois mudar para nomeUserMQTT
const char* mqttPassword = "senhaUserMQTT"; // depois mudar para senhaUserMQTT

// Definições do sensor DHT
#define DHTPIN 4          // Pino de dados do sensor conectado ao ESP32
#define DHTTYPE DHT22     // Tipo do sensor DHT (DHT11 ou DHT22)
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// Tópico MQTT para publicação dos dados de temperatura
const char* temperatureTopic = "casa/sala/temperatura";

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede WiFi");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao servidor MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tente novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(2000);

  float temperature = dht.readTemperature(); // Lê a temperatura em Celsius
  if (isnan(temperature)) {
    Serial.println("Falha ao ler a temperatura do sensor DHT!");
    return;
  }

  Serial.print("Temperatura: ");
  Serial.println(temperature);

  // Publica a temperatura no tópico MQTT
  char temperatureStr[10];
  snprintf(temperatureStr, sizeof(temperatureStr), "%.2f", temperature);
  client.publish(temperatureTopic, temperatureStr);
}
