#include<Wire.h>
#include<WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define BUTTON_ACT 2 // GPIO do botão
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345); // Instancia o acelerômetro

char buf[30]; // array utilizado para concatenar as medições
char x_measure[8];
char y_measure[8];
char z_measure[8];
byte state = 0; // variaveis de estado do botão
byte save_state;


const char* SSID = "NET_2.4";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "lucas1234"; 


const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;    
WiFiClient espClient;               // Porta do Broker MQTT
void conectaWiFi();
void conectaMQTT(); 
void mantemConexoes();

#define ID_MQTT  "ADXL_LUCAS_TESTE"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "MEDIDOR_ADXL_TESTE"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(espClient); 

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_ACT, INPUT_PULLDOWN);
  Serial.println("Accelerometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  Serial.println("");

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void conectaWiFi() {

  if(WiFi.status() == WL_CONNECTED){
    return;
  }

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Em processo de conexao");
  }

    Serial.println("Conectado!");
}


void conectaMQTT() {   
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}


void loop() {

  if (digitalRead(BUTTON_ACT) == HIGH && state == 0){
    delay(20);
    sensors_event_t event; 
    accel.getEvent(&event);
    dtostrf(event.acceleration.x, 6, 2, x_measure);
    dtostrf(event.acceleration.y, 6, 2, y_measure);
    dtostrf(event.acceleration.z, 6, 2, z_measure);
    strcpy(buf,x_measure);
    strcat(buf,",");
    strcat(buf,y_measure);
    strcat(buf,",");
    strcat(buf,z_measure);
    MQTT.publish(TOPIC_PUBLISH, buf);
    Serial.println("Payload enviado!");
    Serial.println(event.acceleration.x);
    state = 1;
  }

  if (digitalRead(BUTTON_ACT) == LOW && state == 1){
    delay(20);
    state = 0;
  }
  // put your main code here, to run repeatedly:
  mantemConexoes();
  MQTT.loop();
}
