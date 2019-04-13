#include <Arduino.h>
//Este sketch permite demostrar como conectarse a un servidor MQTT para enviar datos de temperatura
//y para recibir comandos que permitan encender - apagar un led desde cualquier aplicación MQTT
//Se recomineda usar las app Android MQTT DASH

#include <PubSubClient.h>
#include <ESP8266WiFi.h>  
// WiFi parameters  
//const char* ssid = "GtiNetwork";  
//const char* password = "gi3z-mig1-tpkz"; 
const char* ssid = "Angelonet";  
const char* password = "Marcus336"; 

//Parametros para los mensajes MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//Broquer MQTT
//const char* mqtt_server = "iot.eclipse.org";
const char* mqtt_server ="192.168.0.15";
//const char* mqtt_server = "192.168.127.94";
//const char* mqtt_server = "192.168.0.26";

//Parametros de los pines
const int temperaturapin = A0;
const int ledpin = D1;

//Toma el valor en grados
float temperatura; 



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 0 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(ledpin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(ledpin, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, 75, "%f", temperatura);
      client.publish("temperaturaSalida",msg);
      Serial.println("enviando...");
      Serial.println(msg);
      // ... and resubscribe
      client.subscribe("accionLed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void configurarWifi(void){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED)  
  {  
   delay(500);  
   Serial.print(".");
  }  
  Serial.println("");  
  Serial.println("WiFi connected");  
  // Print the IP address  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

 void setup(void)  
 {  
  //Led en el digital 1ed
  pinMode(ledpin, OUTPUT);
  
  // Inicializar el puerto serial  
  Serial.begin(115200);  
  delay(500);
    
  // Connect to WiFi  
  configurarWifi();  

  //Inicializar el canal MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 }  

 void loop()  
 {  
   //Intentarconectarse al servidor MQTT
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
   //recibe la temperatura para el sensor LM35
   //Este calculo es para el ESP8266 NODEMCU 12 con LM35
   //int valoranalogo = analogRead(temperaturapin);
   //float mv = (valoranalogo/1024.0)*3.3;
   //temperatura = mv/10;

   //Este calculo es con TMP36
   int valoranalogo = analogRead(temperaturapin);
   //float mv = (valoranalogo/1024.0)*3.3;
   //temperatura = (mv - 0.5) * 100;   
   temperatura = valoranalogo;
   //Serial.print("Temperatura Grados: ");
   //Serial.println(temperatura);

  //Publicar el mensaje 
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    //++value;
    //snprintf (msg, 75, "hello world #%ld", value);
    snprintf (msg, 75, "%f", temperatura);
    Serial.print("Publicando temperatura: ");
    Serial.println(msg);
    client.publish("temperaturaSalida", msg);
  }
  delay(100);
}  