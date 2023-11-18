/*Incluimos primero la librería*/
#include <IOXhop_FirebaseESP32.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


/*Definimos nuestras credenciales de la red WiFi*/
const char* ssid = "LINEA_DE_TRABAJO";
const char* pass = "Trabajo.2023.*";

#define AUTH "https://esp32practica-fa041-default-rtdb.firebaseio.com/"                  // URL de la base de datos.
#define HOST "6ko52vcLd5zAjWnsgQJvlFJSLb5lrEtSUsEzS7h2"


/* Configuracion de terminales para led RGB*/
#define R_ADC 32
#define G_ADC 33
#define B_ADC 34

/** Pin del LED **/
#define PinLed 12

bool isOn = true;

void firebaseStreamCallback(FirebaseStream stream);


String path = "/";

void setup() {
  Serial.begin(115200);
  Serial.print("DHT TEST");
  dht.begin();

    /*Iniciamos la conexión a la red WiFi definida*/
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
  /*Con la conexión ya realizada, se pasa a imprimir algunos datos importantes, como la dirección IP asignada a nuestro dispositivo*/
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(PinLed, OUTPUT);
  digitalWrite(PinLed, LOW);
  analogReadResolution(9);

  Firebase.begin(AUTH, HOST);

  Firebase.stream(path, firebaseStreamCallback);

  //set value
  Firebase.setBool("Led", isOn);
  
}

void loop() {
  /*No se contempla el uso de un loop en el ejemplo de esta semana*/
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(isnan(h) || isnan(t)){
    Serial.println("Failed to read from DTH sensor:");
    return;
  }

  Serial.println("Humidity:");
  Serial.print(h);
  Serial.print(" %\t");

  Serial.println("Temperature:");
  Serial.print(t);
  Serial.print(" °C");

  //set value
  Firebase.setFloat("Humidity", h);
  Firebase.setFloat("Temperature", t);

  /* Lectura analogica RGB */
  Firebase.setInt("RGB/R", analogRead(R_ADC)/2);

  Firebase.setInt("RGB/G", analogRead(G_ADC)/2);

  Firebase.setInt("RGB/B", analogRead(B_ADC)/2);


  //Handle error
  if(Firebase.failed()){
    Serial.print("Setting/number failed:");
    Serial.println(Firebase.error());
    return;
  }

  Serial.println("Temperature and Humidity Data Sent Successfully");
  delay(1000);
}


  void firebaseStreamCallback(FirebaseStream stream) {
    if (stream.getEvent() == "put") {
      String namePath = stream.getPath();
      Serial.println("Changes in the database: " + namePath);
      
      if (namePath.equals("/Led")) {  // Ensure the path matches your Firebase structure
        bool ledState = stream.getDataBool();
        Serial.print("State of the LED: ");
        Serial.println(ledState ? "On" : "Off");

        digitalWrite(PinLed, ledState ? HIGH : LOW);
      }
    }
  }

