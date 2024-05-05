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
#include <time.h>
#include <NoDelay.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <Wire.h>
#include <SPI.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiClient.h>
// #include "Firebase_ESP_Client.h"
#include "ArrayList.h"
// #include <addons/TokenHelper.h>
// #include <addons/RTDBHelper.h>

// #include <BlynkEdgent.h>
// https://www.youtube.com/watch?v=HRGQQATYJCQ&t=465s

// #define BMP_SCK (13)
// #define BMP_MISO (12)
// #define BMP_MOSI (11)
// #define BMP_CS (10)
// #define API_KEY "API_KEY"
// #define DATABASE_URL "https://esp32-b69ac-default-rtdb.firebaseio.com/"
// #define USER_EMAIL "firebase-adminsdk-piomm@esp32-b69ac.iam.gserviceaccount.com"
// #define USER_PASSWORD "sr5olqF7CxXBPEnvWbTD6TruPL221ZHuKKk4c2e4"
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;
bool firebaseConfigReady = false;



ArrayList<int> datos, humedadData, tempData, uviData, promsHumedad;
// FirebaseData firebaseData;

Adafruit_BMP280 bmp; // I2C
char authBlynk[] = "wkObE_YGSF3F0F_XrAIkxYlzuvK_HiJR";
const char *ssid = "Ubee1D42-2.4G";
const char *pw = "B5D7FC1D42";
// const byte pinBomba = 4;

// const char *ssid = "IoT_LV323";
// const char *pw = "@dm1nLV323";
DHT dht(5, DHT11);
String temperatura, humedad, presion;
String serverName = "https://api.openweathermap.org/data/3.0/onecall?lat=29.10&lon=-110.97&units=metric&exclude=hourly,daily,minutely&appid=bb5a9e25d5335be9568d59bf17797bb5";
BlynkTimer timer;
int current_uv;
int lastTime = 0;
AsyncWebServer server(80);
noDelay pausa(10000),pausa2(15000),pausa3(600000);
// void lecturaBmp280();
void actualizaLectura();
String processor(const String &var),obtenTemp(),obtenHumedad(),obtenerPresion();
void conectaRedWifi(const char *ssid, const char *pw);
void inicializaSPIFFS();
void configuraServidor();
// void noHallada(AsyncWebServerRequest *request);
void callApi();
// void setupFirebase();
int calculaPromedio(ArrayList<int> lista);

void setup()
{
  Serial.begin(9600);
  Blynk.begin(authBlynk, ssid, pw, "blynk.cloud", 80);
  // if (!bmp.begin(0x76) || !bmp.begin(0x50) || !bmp.begin(0x40))
  // {
  //   Serial.print("hola");
  //   Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
  //                    "try a different address!"));
  //   while (1)
  //     ;
  // }
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
  // setupFirebase();
}

void loop()
{
  ArrayList<int> datos, humedadData, tempData, uviData, promsHumedad;


  if (pausa.update())
  {
    actualizaLectura();
    Blynk.run();
    Blynk.virtualWrite(V2, temperatura);
    Blynk.virtualWrite(V0, humedad.toInt());
    Blynk.virtualWrite(V3, current_uv);
    humedadData.add(humedad.toInt());
  }
  if (pausa2.update())
  {

    // guardamos en un arreglo cada current
    int promH = calculaPromedio(humedadData);
    promsHumedad.add(promH);
    tempData.add(temperatura.toInt());
    uviData.add(current_uv);
    humedadData.clear();
  }
  if (pausa3.update())
  {
    int promH = calculaPromedio(promsHumedad);
    int promT = calculaPromedio(tempData);
    int promU = calculaPromedio(uviData);
    // aqui si metemos a la base de datos un promedio general

    Firebase.RTDB.setInt(&firebaseData, "/sensoresdata/humedad", promH);
    Firebase.RTDB.setInt(&firebaseData, "/sensoresdata/temperatura", promT);
    Firebase.RTDB.setInt(&firebaseData, "/sensoresdata/uvi", promU);
  }
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
/**
 * Esta funcion monta el sistema de archivos SPIFFS
 */
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

/**
 * Mapea las funcionalidades del servidor a los URL con las que SERAN INVOCADAS
 */

void configuraServidor()
{

  server.serveStatic("/", SPIFFS, "/");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.on("/a", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estadisticas.html", "text/html", false, processor); });

  server.on("/obtenTemperatura", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", temperatura); });

  server.on("/obtenHumedad", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", humedad); });
  server.on("/controladores", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("https://blynk.cloud/dashboard/168098/global/devices/1257163/organization/168098/devices/616016/dashboard"); });
};

String processor(const String &var)
{
  if (var == "TEMPERATURA")
  {
    return temperatura;
  }
  else if (var == "HUMEDAD")
  {
    return humedad;
  }
  return String();
}

void actualizaLectura()
{
  temperatura = obtenTemp();
  humedad = obtenHumedad();
  callApi();
  // presion = obtenerPresion();
}

String obtenTemp()
{
  float tempCent = dht.readTemperature();
  if (isnan(tempCent))
  {
    Serial.println("Error al leer el sensor DHT11");
    return String();
  }
  return String(tempCent);
}
// String obtenerPresion()
// {
//   if (bmp.takeForcedMeasurement())
//   {
//     float p = bmp.readPressure();
//     Serial.print(F("Pressure = "));
//     Serial.print(bmp.readPressure());
//     return String(p);
//   }
//   else
//   {
//     Serial.println("Forced measurement failed!");
//   }
//   return String();
// }
String obtenHumedad()

{
  float lectura = analogRead(32);
  return String(((lectura / 4095) * 100));
}

//  obtenemos los datos de una api de
//    - UV HERMOSILLO
//    - RADIACION SOLAR HERMOSILLO
//  luego, guardamos esos datos en variables
//  y mandamos esos datos al blynk
// ahora ocupamos guardar el valor del switch de la bomba

void callApi()
{
  String payload;

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String serverPath = serverName;

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

  lastTime = millis();

  DynamicJsonDocument jsonDoc(768); // Tamaño del documento JSON
  DeserializationError error = deserializeJson(jsonDoc, payload);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  JsonObject current = jsonDoc["current"];
  current_uv = current["uvi"];
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

// void setupFirebase()
// {
//     if (firebaseConfigReady)
//         return;

//     firebaseConfigReady = true;

//     // For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

//     /* Assign the api key (required) */
//     config.api_key = "";

//     /* Assign the user sign in credentials */
//     auth.user.email = "firebase-adminsdk-piomm@esp32-b69ac.iam.gserviceaccount.com";
//     auth.user.password =  "sr5olqF7CxXBPEnvWbTD6TruPL221ZHuKKk4c2e4";

//     /* Assign the RTDB URL (required) */
//     config.database_url = "https://esp32-b69ac-default-rtdb.firebaseio.com/";

//     /* Assign the callback function for the long running token generation task */
//     config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

//     // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
//     Firebase.reconnectNetwork(true);

//     // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
//     // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
//     fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

//     // Or use legacy authenticate method
//     // config.database_url = DATABASE_URL;
//     // config.signer.tokens.legacy_token = "<database secret>";

//     // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

//     Firebase.begin(&config, &auth);

//     Firebase.setDoubleDigits(5);
// }

// para ir haciendo las pruebas unitarias, hay que conectar la base de datos
//   lo que vamos a guardar son datos
//       -- rayos uv
//       -- temperatura
//       -- humedad relativa
//       OPCIONAL -- agua utilizada en la semana
//  pero no no sirve guardar cada hora un valor
//  ocupamos el valor promedio de la semana, o la temperatura de cada 7 dias

// traemos otra vez los datos, cada hora, filtramos los que ocupamos nada mas
// traemos esos datos de cada hora, guardamos en la base de datos, cuando se consulte la pagina de recomendaciones, traemos esos datos de la bd, ahi sacamos el promedio y hacemos las graficas y algoritmos