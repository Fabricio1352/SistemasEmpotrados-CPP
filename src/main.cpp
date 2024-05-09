#define BLYNK_TEMPLATE_ID "TMPL2HKZ88unP"
#define BLYNK_TEMPLATE_NAME "Plant Data"
#define BLYNK_AUTH_TOKEN "wkObE_YGSF3F0F_XrAIkxYlzuvK_HiJR"
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <NoDelay.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <SPI.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClient.h>
#include "ArrayList.h"
#define relay 4

ArrayList<int> humedadData, tempData, uviData, promsHumedad;
HTTPClient http;
Adafruit_BMP280 bmp;

bool firebaseConfigReady = false;
char authBlynk[] = "wkObE_YGSF3F0F_XrAIkxYlzuvK_HiJR";
const char *ssid = "Ubee1D42-2.4G";
const char *pw = "B5D7FC1D42";
// const char *ssid = "IoT_LV323";
// const char *pw = "@dm1nLV323";

DHT dht(5, DHT11);
// String temperatura,
double temperatura;
String humedad, presion;
BlynkTimer timer;
int current_uv, current_presure, current_windspeed;
AsyncWebServer server(80);
// noDelay pausa(10000), pausa2(15000), pausa3(600000);
noDelay pausa(1000), pausa2(120000);
void actualizaLectura();
double obtenTemp();
void conectaRedWifi(const char *ssid, const char *pw);
void inicializaSPIFFS();
void configuraServidor();
void callApi();
void urlData(double t, double hr, double uvi);
int calculaPromedio(ArrayList<int> lista);

void setup()
{
  Serial.begin(9600);
  Blynk.begin(authBlynk, ssid, pw, "blynk.cloud", 80);
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  dht.begin();
  conectaRedWifi(ssid, pw);
  inicializaSPIFFS();
  configuraServidor();
  server.begin();
  Serial.println("Servidor web inicializado");
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  callApi();
}

void loop()
{

  if (pausa.update())
  {
    actualizaLectura();

    // humedadData.add(humedad.toInt());
  }
  if (pausa2.update())
  {
    callApi();
    Serial.println(current_windspeed);
  }
  /*
    // if (pausa2.update())
  // {

  //   // guardamos en un arreglo cada current
  //   humedadData.add(50);
  //   humedadData.add(30);
  //   humedadData.add(75);

  //   int promH = calculaPromedio(humedadData);
  //   promsHumedad.add(promH);
  //   tempData.add(temperatura.toInt());
  //   uviData.add(current_uv);
  //   humedadData.clear();
  // }
  // if (pausa3.update())
  // {

  // tempData.add(59);
  // tempData.add(54);
  // tempData.add(51);
  // uviData.add(2);
  // uviData.add(1);
  // uviData.add(7);
  // double promH = calculaPromedio(promsHumedad);
  // double promT = calculaPromedio(tempData);
  // double promU = calculaPromedio(uviData);
  // Serial.println("promedio h: " + String(promH));
  // Serial.println("Promedio t:" + String(promT));
  // Serial.println("Promedio u:" + String(promU));
  // Serial.println("arreglos: ");
  // for (int i = 0; i < uviData.size(); i++)
  // {
  //   Serial.println(uviData.get(i));
  // }
  // for (int i = 0; i < tempData.size(); i++)
  // {
  //   Serial.println(tempData.get(i));
  // }    for (int i = 0; i < promsHumedad.size(); i++)
  // {
  //   Serial.println(promsHumedad.get(i));
  // }
  // aqui si metemos a la base de datos un promedio general
  // urlData(1, 1, 1);
  // }


  */
}

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
    if (pausa.update())
    {
      Serial.print(".");
    }
  }
  Serial.println('\n');
  Serial.println("Connexion establecida");
  // Obten la direccion IP del microcontrolador ESP32
  Serial.print("Direccion IP del servidor web: ");

  Serial.println(WiFi.localIP());
}

void inicializaSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("Ocurrió un error al montar SPIFFS");
  }
  else
  {
    Serial.println("Se monto SPIFFS con exito");
  }
}

void configuraServidor()
{

  server.serveStatic("/", SPIFFS, "/");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.on("/a", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estadisticas.html", "text/html", false, NULL); });

  server.on("/obtenTemperatura", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(temperatura)); });

  server.on("/obtenHumedad", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", humedad); });
  server.on("/obtenPresion", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(current_presure)); });
  server.on("/controladores", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("https://blynk.cloud/dashboard/168098/global/devices/1257163/organization/168098/devices/616016/dashboard"); });
};

void actualizaLectura()
{
  // temperatura = obtenTemp();
  humedad = obtenHumedad();
  Blynk.run();
  Blynk.virtualWrite(V2, temperatura);
  Blynk.virtualWrite(V0, humedad.toInt());
  Blynk.virtualWrite(V3, current_uv);
  Blynk.virtualWrite(V4, current_windspeed);
}

double obtenTemp()
{
  float tempCent = dht.readTemperature();
  if (isnan(tempCent))
  {
    // Serial.println("Error al leer el sensor DHT11");
    return 0;
  }
  return tempCent;
}

String obtenHumedad()

{
  float lectura = analogRead(32);
  return String(100 - ((lectura / 4095) * 100));
}

void callApi()
{
  String payload;

  if (WiFi.status() == WL_CONNECTED)
  {
    String serverPath = "https://api.openweathermap.org/data/3.0/onecall?lat=33.44&lon=-94.04&exclude=hourly,minutely,daily&units=metric&appid=f86f1ae55d3838b3aa1bee067582b656";

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {

      payload = http.getString();
    }
    else
    {
      Serial.println(" Error codeeeee: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("Disconected");
  }

  DynamicJsonDocument jsonDoc(768);
  DeserializationError error = deserializeJson(jsonDoc, payload);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  JsonObject current = jsonDoc["current"];
  current_uv = current["uvi"];
  current_presure = current["pressure"];
  current_windspeed = current["wind_speed"];
  temperatura = current["temp"];
}

int calculaPromedio(ArrayList<int> lista)
{
  int suma = 0;
  for (int i = 0; i < lista.size(); i++)
  {
    suma = suma + lista.get(i);
  }
  int promedio = suma / lista.size();
  return promedio;
}

void urlData(double t, double hr, double uvi)
{
  http.begin("http://localhost:8080/create");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // int httpResponseCode = http.POST("t=" + String(t) + "&hr=" + String(hr) + "=&uvi=" + String(uvi));
  int httpResponseCode = http.POST("t=1&hr=1&uvi=1");
  if (httpResponseCode > 0)
  {
    Serial.print("Respuesta del servidor: ");
    Serial.println(http.getString());
  }
  else
  {
    Serial.print("Error en la solicitud. Código de respuesta: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

BLYNK_WRITE(V1)
{
  bool Relay = param.asInt();
  if (Relay == 1)
  {
    digitalWrite(relay, LOW);
  }
  else
  {
    digitalWrite(relay, HIGH);
  }
}