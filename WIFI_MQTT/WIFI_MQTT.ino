#include<Wire.h>
#include<WiFi.h>
#include <PubSubClient.h>
#include<ADXL345_WE.h>

#define ADXL345_I2CADDR 0x53
#define BUTTON_ACT 2
ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR);
char buf[30];
char x_measure[8];
char y_measure[8];
char z_measure[8];
byte state = 0;
byte save_state;


const char* SSID = "NET_2.4";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "lucas1234"; 


const char* BROKER_MQTT = "mqtt.eclipseprojects.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;    
WiFiClient espClient;               // Porta do Broker MQTT
void conectaWiFi();
void conectaMQTT(); 
void mantemConexoes();
void enviaValores();

#define ID_MQTT  "ADXL_LUCAS_TESTE"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "MEDIDOR_ADXL_TESTE"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(espClient); 

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_ACT, INPUT_PULLDOWN);
  Serial.println("ADXL345_Sketch - Basic Data");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
  myAcc.setDataRate(ADXL345_DATA_RATE_50);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());
  myAcc.setRange(ADXL345_RANGE_4G);
  Serial.print("  /  g-Range: ");
  Serial.println(myAcc.getRangeAsString());
  Serial.println();

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
    xyzFloat g = myAcc.getGValues();
    dtostrf(g.x, 6, 2, x_measure);
    dtostrf(g.y, 6, 2, y_measure);
    dtostrf(g.z, 6, 2, z_measure);
    strcpy(buf,x_measure);
    strcat(buf,",");
    strcat(buf,y_measure);
    strcat(buf,",");
    strcat(buf,z_measure);
    MQTT.publish(TOPIC_PUBLISH, buf);
    Serial.println("Payload enviado!");
    Serial.println(g.z);
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
