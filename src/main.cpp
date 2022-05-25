// Bibliotecas
#include <HTTPClient.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#define DHTPIN 4 
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

WiFiManager wifiManager; // Objeto de manipulação do wi-fi
WiFiClient wifiClient;

PubSubClient mqttClient(wifiClient);

BH1750 lightMeter;

const int potPin = 34;

float temperature = 0;
float luminosity = 0;
float airHumidity = 0;
float soilHumidity = 0;

void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void reconnect();
String getJson();

void setup()
{

    Serial.begin(115200);
    Serial.println("...");

    if (WiFi.status() != WL_CONNECTED)
    {
        if (!wifiManager.autoConnect("SMART-FARM-CONFIG", "12345678")) // Função para se autoconectar na rede
        {
            Serial.println("Falha ao conectar"); // Se caso não conectar na rede mostra mensagem de falha
        }
        else
        {
            Serial.println("Conectado na Rede!!!");
        }
    }
    else
    {
        Serial.println("Conectado na Rede!!!");
    }

    // callback para quando entra em modo de configuração AP
    wifiManager.setAPCallback(configModeCallback);
    // callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    mqttClient.setServer("192.168.0.17", 1883);

    dht.begin();

    Wire.begin();

    lightMeter.begin();
}

int cont = 0;

void loop()
{

    temperature = dht.readTemperature();

    airHumidity = dht.readHumidity();

    luminosity = lightMeter.readLightLevel();

    soilHumidity = analogRead(potPin);

    if (!mqttClient.connected())
    {
        reconnect();
    }

    mqttClient.loop();
    mqttClient.publish("SENSORES", getJson().c_str());

    cont++;
    Serial.println("cont: " + String(cont));
    Serial.println(mqttClient.state());
    Serial.println(mqttClient.connected());
    delay(3000);
}

// callback que indica que o ESP entrou no modo AP
void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.println("Entrou no modo de configuração");

    Serial.println(WiFi.softAPIP());                      // imprime o IP do AP
    Serial.println(myWiFiManager->getConfigPortalSSID()); // imprime o SSID criado da rede
}

// Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback()
{
    Serial.println("Configuração salva");
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    // message received
}

void reconnect()
{
    // Loop until we're reconnected
    while (!mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("teste", "smartfarm", "smartfarm123"))
        {
            Serial.println("connected");
            // Subscribe
            mqttClient.subscribe("SENSORES");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

String getJson()
{
    String json = "{";
    json += "\"temperature\":\"" + String(temperature) + "\",";
    json += "\"luminosity\":\"" + String(luminosity) + "\",";
    json += "\"airHumidity\":\"" + String(airHumidity) + "\",";
    json += "\"soilHumidity\":\"" + String(soilHumidity) + "\"";
    json += "}";
    return json;
}
