// Bibliotecas
#include <HTTPClient.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

#define ONBOARD_LED 2

WiFiManager wifiManager; // Objeto de manipulação do wi-fi

void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();

void setup()
{
    
    Serial.begin(115200);
    Serial.println("...");

    // callback para quando entra em modo de configuração AP
    wifiManager.setAPCallback(configModeCallback);
    // callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
    wifiManager.setSaveConfigCallback(saveConfigCallback);
}

void loop()
{

    if (WiFi.status() != WL_CONNECTED)
    {
        if (!wifiManager.autoConnect("SMART-FARM-CONFIG", "12345678"))// Função para se autoconectar na rede
        { 
                Serial.println("Falha ao conectar"); // Se caso não conectar na rede mostra mensagem de falha
        }else{
            Serial.println("Conectado na Rede!!!");
        }
    }else{
        Serial.println("Conectado na Rede!!!");
    }
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
