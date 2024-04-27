#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <time.h>
#include <NoDelay.h>

const char *ssid = "Ubee1D42-2.4G";
const char *pw = "B5D7FC1D42";

AsyncWebServer server(80);
noDelay pausa(2000);

void conectaRedWifi(const char *ssid, const char *pw);
void inicializaSPIFFS();
void configuraServidor();
void noHallada(AsyncWebServerRequest *request);

void setup()
{
  Serial.begin(9600);
  delay(100);

  conectaRedWifi(ssid, pw);
  inicializaSPIFFS();
  configuraServidor();
  server.begin();
  Serial.println("Servidor web inicializado");
}

void loop()
{
}
/*
 * Conecta el microcontrolador ESP32 a una red WiFi
 */
void conectaRedWifi(const char *ssid, const char *password)
{
  // Conexion a la red
  WiFi.begin(ssid, password);
  Serial.print("Conectandose a la red ");
  Serial.print(ssid);
  Serial.println(" ...");
  // Mientras no se ha conectado a la red WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connexion establecida");
  // Obten la direccion IP del microcontrolador ESP32
  Serial.print("Direccion IP del servidor web: ");

          Serial.println(WiFi.localIP());
}
/**
 * Esta funcion monta el sistema de archivos LittleFS
 */
void inicializaSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("Ocurrió un error al montar LittleFS");
  }
  else
  {
    Serial.println("Se monto LittleFS con exito");
  }
}

/**
 * Mapea las funcionalidades del servidor a los URL con las que SERAN INVOCADAS
*/

void configuraServidor(){
  // carga los archivos estaticos desde la raiz del sistema de archviso LittleFS

  server.serveStatic("/",SPIFFS, "/");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request -> send(SPIFFS, "/index.html", "text/html");
  });
}


