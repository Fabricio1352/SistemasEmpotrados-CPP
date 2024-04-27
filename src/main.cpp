#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <time.h>
#include <NoDelay.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"


#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS (10)

Adafruit_BMP280 bmp; // I2C
const char *ssid = "Ubee1D42-2.4G";
const char *pw = "B5D7FC1D42";
DHT dht(5, DHT11);
typedef struct
{
    float temperaturaCent;
} sDht11;

sDht11 sdht11;
AsyncWebServer server(80);
noDelay pausa(2000);

void printValues(sDht11 *psDht11);
bool readDht11(sDht11 *psDht11);
void lecturaBmp280();
void lecturaDht11();
void lecturaYl();
void conectaRedWifi(const char *ssid, const char *pw);
void inicializaSPIFFS();
void configuraServidor();
void noHallada(AsyncWebServerRequest *request);

String clasificarHumedad(int valor_analogico) {
    if (valor_analogico > 3500 && valor_analogico <= 4095) {
        return "Seco";
    } else if (valor_analogico < 3500 && valor_analogico > 1900) {
        return "Húmedo";
    } else if(valor_analogico <1900 && valor_analogico > 1150){
        return "Demasiado húmedo";
    } else {
      return "Ocurrio un error";
    }
}

void setup()
{
    // if (!bmp.begin(0x76))
  // {
  //   Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
  //                    "try a different address!"));
  //   while (1)
  //     delay(10);
  // }
  // bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
  //                 Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
  //                 Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
  //                 Adafruit_BMP280::FILTER_X16,      /* Filtering. */
  //                 Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  // Serial.begin(9600);
  // delay(100);
  dht.begin();
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

void lecturaBmp280(){
    // must call this to wake sensor up and get new measurement data
  // it blocks until measurement is complete
  if (bmp.takeForcedMeasurement())
  {
    // can now print out the new measurements
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    delay(2000);
  }
  else
  {
    Serial.println("Forced measurement failed!");
  }
}

bool readDht11(sDht11 *psDht11)
{
    float temperatura = dht.readTemperature();

    if (isnan(temperatura))
    {
        Serial.println("Error al leer la temperatura en el sensor DHT11");
        return false;
    }

    psDht11->temperaturaCent = temperatura;
    return true;
}



void printValues(sDht11 *psDht11)
{
    Serial.print("Temperatura = ");
    Serial.print(psDht11->temperaturaCent);
}


void lecturaDht11(){
      if (readDht11(&sdht11))
        printValues(&sdht11);
}

void lecturaYl(){
    int lectura = analogRead(2);

  Serial.println("La lectura es: ");
  Serial.println(lectura);
  Serial.println(clasificarHumedad(lectura));
  delay(1000);
}