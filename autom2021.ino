//Este codigo fue dieñado para un Nodemcu 1.0, con un Dht11, y los sensores de Bomba de agua mecanicos(tipo flotantes)

//Librerias Agregadas
 #include <ESP8266WiFi.h>  
 #include <WiFiClient.h>  
 #include <ESP8266mDNS.h>
 #include <ThingSpeak.h>
 #include <DHT.h>  
 
 #define DHTPIN D4  //definicion del pin usado para el sensor en este caso el d
 #define DHTTYPE DHT11
 
 
 DHT dht(DHTPIN, DHTTYPE);
 unsigned long previousMillis = 0;//definicion de variable para contador de tiempo

 const char* ssid = "tunobredered";  
 const char* password = "tuclavewifi"; 
 WiFiClient client;  
 
 unsigned long myChannelNumber = "numerodecanal";  
 const char * myWriteAPIKey = "WriteAPIKey";  
 uint8_t temperature, humidity;  //variables para dht
 
 //declaracion de pines
int Tanque = 14;//d2  sensor flotante a tanque 
int Cisterna = 5;//d1 Sensor flotante a Cisterna(tanque inferior
int  Bomba =12 ;//d6  Conectado a Motor
int ledbomba=13;//    Led indicador de funcionamiento del loop

//declaracion de variables para codigo
bool estadotanque=false;
bool estadocisterna=false;
int estadobomba;



void setup() 
{   
    digitalWrite(Bomba,HIGH);//en mi caso la declaro como High porque esta invertida mi salida, y aqui defino que inicie apagada
    
    Serial.begin(115200);   //iniciar puerto serie
    //Declaracion de entradas y salidas de pines
    pinMode(Tanque,INPUT);
    pinMode(Cisterna,INPUT);
    pinMode(Bomba,OUTPUT) ;
    pinMode(ledbomba,OUTPUT) ;
   //Hago un check de encendido con luces
    pinMode(LED_BUILTIN, HIGH);
    delay(1000);
    pinMode(LED_BUILTIN, LOW);
    
    delay(1000);
    //inicio el sensor de temperatura y humedad
    dht.begin();  
    delay(10);  

    // Connect to WiFi network  conexion a wifi
    Serial.println();  
    Serial.println();  
    Serial.print("Conectando a ");  
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
    Serial.println(WiFi.localIP());  

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ThingSpeak.begin(client);  
    

}

void loop() 
{

  digitalWrite(ledbomba,HIGH);//dejo encendido el led para indicar que el led funciona
  estadotanque = digitalRead(Tanque);  //lectura digital de pin
  estadocisterna = digitalRead(Cisterna);  //lectura digital de pin
  //estadobomba=digitalRead(Bomba);  //lectura digital de pin
  //realizo una impresion por serial para chequear los estados del sistema
  if(estadotanque)
  {
    Serial.println("tanque sin agua=1");
    delay(500);
  }
  else
  {
    Serial.println("tanque completo=0");
     delay(500);
  }
  delay(100);
   if(estadocisterna)
  {
    Serial.println("cisterna sin agua=1");
     delay(500);
  }
  else
  {
    Serial.println("Cisterna llena=0");
     delay(500);
  }
   
//Encendido de bomba si falta agua en el tanque y si la cisterna tiene agua
  if (estadotanque  && !estadocisterna)
    { 
    Serial.println("bomba en funcionamiento");
    delay(50);
    pinMode(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(Bomba,LOW);
    delay(50);
    estadobomba=1;
   
    }
 //apagar bomba si se vacio la cisterna o si se lleno el tanque 
   else if (estadocisterna  || !estadotanque )
  {
   Serial.println("bomba apagada");
   delay(50);
    pinMode(LED_BUILTIN, LOW);
    delay(500);
   digitalWrite(Bomba,HIGH);
   delay(50);
   estadobomba=0;
   
   
   delay(100);
  }
  //si la bomba esta en funcionamento envia la informacion a thingspeak para controlar su uso y horarios
  if (estadobomba)
  {
    ThingSpeak.writeField(myChannelNumber, 3, estadobomba, myWriteAPIKey);
  }
  
  delay(100);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 240000)//Cada 4min envia los valores leidos por el dth a thingspeak
   {  
      
      previousMillis = currentMillis;
      temperature = dht.readTemperature();  
      humidity = dht.readHumidity();  
      Serial.print("Temperature Value is :");  
      Serial.print(temperature);  
      Serial.println("C");  
      Serial.print("Humidity Value is :");  
      Serial.print(humidity);  
      Serial.println("%");  
      ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);  
      ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);  
   }

   
}
