/*
-----Includes-----
*/

//Server
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

//RFID
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>

//LEDS

/*
------DEFINES------------
*/
//RFID
#define SS_PIN 5
#define RST_PIN 2
//#define LED_PIN 14
#define NUM_LEDS 218
#define SERVO_PIN 15

// LEDS
#define LED_PIN 14
#define NUM_LEDS 218
#define IR_DER_A  32 //Entrada digital conectada al sensor infrarrojo
#define IR_IZQ_A  34

/*
----VARIABLES
*/
//Server
const char* ssid = "TP-LINK_167E52"; 
const char* password = "teleko4ever"; 
int marcador1 = 0; // Variable marcador1
int marcador2 = 0; // Variable marcador2 
bool gol_izq_registrado = false; // Variable estado sensor izquierdo
bool gol_der_registrado = false; // Variable estado sensor derecho
WebServer server(80);

//RFID
Servo servo; 
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo;
bool cardDetected = false;
bool allowed = false;
const byte ALLOWED_CARDS[][4] = { // UID de las tarjetas permitidas
  {0x91, 0x81, 0x6D, 0x1D},
  {0xC9, 0xA3, 0xA8, 0x14},
  // Agregar más tarjetas aquí
};

//LEDS
int i = 0;

/*
------FUNCIONES------
*/
//server
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='4'><style>body { background: linear-gradient(to bottom right, #FF66CC, #6600CC); font-family: Arial, sans-serif; } .container { display: flex; flex-direction: row; justify-content: center; align-items: center; height: 100vh; } .score-container { position: relative; width: 400px; height: 450px; margin-right: 20px; } .score-border { position: absolute; top: 0; left: 0; right: 0; bottom: 0; border: 5px solid #FFF; border-radius: 10px; background-color: #333; display: flex; flex-direction: column; justify-content: center; align-items: center; } .team-name { position: absolute; bottom: 5px; left: 50%; transform: translateX(-50%); font-size: 1.5vw; color: #FFF; } .square { position: absolute; top: -15px; width: 30px; height: 30px; border-radius: 5px; } .square-white { background-color: #FFF; } .square-half-red { background: linear-gradient(to right, red 50%, #FFF 50%); } h1 { font-size: 15vw; margin: 0; color: #FFF; } @media (max-width: 480px) { .score-container { width: 150px; height: 180px; margin-right: 10px; } .bottom-row { font-size: 6vw; } .team-name { font-size: 4vw; } h1 { font-size: 15vw; } footer { background-color: #a7a7a7; color: white; padding: 10px; text-align: center; } }</style></head><body><div class='container'><div class='score-container'><div class='score-border'><div class='square square-white'></div><h1>" + String(marcador1) + "</h1><div class='team-name'>Real de Madrid</div></div></div><div class='score-container'><div class='score-border'><div class='square square-half-red'></div><h1>" + String(marcador2) + "</h1><div class='team-name'>Atl. de Madrid</div></div></div></div><footer><p>Contact with <a href='https://t.me/Alex1to'>@Alex1to</a> ♨ on Telegram for any questions about this page</p></footer></body></html>";
  server.send(200, "text/html", html);
}

//RFID

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 218; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }    
    strip.show();
    delay(wait);
  }
  strip.clear();
  strip.show();
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


void rojo(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 218; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 255, 0, 0);
    }    
    strip.show();
    delay(wait);
  }
  strip.clear();
  strip.show();
}

//LEDS

void arcoiris() {
  strip.begin(); // Inicializa la tira de LEDs
  
  int tiempo = 30; // Tiempo de espera entre cada cambio de color en milisegundos
  int num_leds = strip.numPixels(); // Número de LEDs en la tira
  
  // Ciclo para desplazar el color a lo largo de la tira de LEDs
  for (int desplazamiento = 0; desplazamiento < 256; desplazamiento++) {
    for (int i = 0; i < num_leds; i++) {
      int pixel_index = (i + desplazamiento) % num_leds; // Índice del LED actual
      
      // Establece el color del LED actual en función del desplazamiento
      strip.setPixelColor(pixel_index, Wheel((i * 256 / num_leds + desplazamiento) & 255));
    }
    strip.show(); // Muestra los colores en la tira de LEDs
    delay(tiempo); // Espera un corto tiempo para que se vea el efecto de cambio de color
  }
}



void rojoBlanco() {
  strip.begin(); // Inicializa la tira de LEDs
  Serial.println("Gol atleti");
  while (i < 3) {
    // Ciclo para establecer el color de los LEDs pares e impares
    for(int i=0; i<strip.numPixels(); i++) {
      if (i % 20 < 10) { // Si el índice del LED es par
        strip.setPixelColor(i, strip.Color(255, 0, 0)); // Pone el LED en rojo
      } else { // Si el índice del LED es impar
        strip.setPixelColor(i, strip.Color(255, 255, 255)); // Pone el LED en blanco
      }
    }
    strip.show(); // Muestra los colores en la tira de LEDs
    delay(300); // Espera un corto tiempo para que se vea el efecto de cambio de color
    
    // Ciclo para apagar todos los LEDs
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Apaga el LED
    }
    strip.show(); // Muestra los colores en la tira de LEDs
    delay(300); // Espera un corto tiempo para que se vea el efecto de cambio de color
    i++;
  strip.clear();
  }
  i= 0;
  
}

void todosBlanco() {
  strip.begin(); // Inicializa la tira de LEDs
  
  while (i < 3) {
    // Ciclo para establecer todos los LEDs en blanco
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255)); // Pone el LED en blanco
    }
    strip.show(); // Muestra los colores en la tira de LEDs
    delay(500); // Espera un corto tiempo para que se vea el efecto de cambio de color
    
    // Ciclo para apagar todos los LEDs
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Apaga el LED
    }
    strip.show(); // Muestra los colores en la tira de LEDs
    delay(500); // Espera un corto tiempo para que se vea el efecto de cambio de color
    i++; 
    strip.clear();
    
  }
  i= 0;  
  
}

void apagar() {
  strip.begin();
  while(true){
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Apaga el LED
    }
    strip.show();
  }
}

/*
-----SETUP-----
*/
void setup() {

  //Server
  // Configurar los pines de los sensores como entrada
  pinMode(IR_DER_A, INPUT);
  pinMode(IR_IZQ_A, INPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  //RFID
  SPI.begin();
  rfid.PCD_Init();
  strip.begin();
  servo.attach(15);
  cardDetected = false;

  //LEDS  
  strip.begin();
  pinMode(LED_PIN,OUTPUT);

}


/*
-----LOOP----
*/
void loop() {
  //SERVER
  // Actualizar marcador si se detecta corte de haz en el sensor 1
  int valor_der = digitalRead(IR_DER_A); //leemos el valor del sensor infrarrojo
  int valor_izq = digitalRead(IR_IZQ_A);
  if (valor_der == HIGH && !gol_der_registrado) {
    gol_der_registrado = true;
    marcador1++;
    todosBlanco();
    Serial.println("Se detectó corte de haz en el sensor 1");
    Serial.println("Marcador1: " + String(marcador1));
  } else if (valor_der == LOW) {
    gol_der_registrado = false;
  }

  // Actualizar marcador si se detecta corte de haz en el sensor 2
  if (valor_izq == HIGH && !gol_izq_registrado) {
    gol_izq_registrado = true;
    marcador2++;
    rojoBlanco();
    Serial.println("Se detectó corte de haz en el sensor 2");
    Serial.println("Marcador2: " + String(marcador2));
  } else if (valor_izq == LOW) {
    gol_izq_registrado = false;
  }

  server.handleClient();

  //RFID
  if (cardDetected) {   
    return;    
  }
  
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  bool allowed = false;
  for (byte i = 0; i < sizeof(ALLOWED_CARDS) / sizeof(ALLOWED_CARDS[0]); i++) {
    if (memcmp(rfid.uid.uidByte, ALLOWED_CARDS[i], 4) == 0) {
      allowed = true;
      

    }
  }

  if(allowed){
    servo.write(90);
    rainbow(4);
    strip.clear();
    strip.show();
    servo.write(0);
    marcador1=0;
    marcador2=0;
    delay(100);
  }
  
  if (!allowed) {
    Serial.println("Acceso denegado");
    rojo(3);
    strip.clear();
    strip.show();
    return;
  }
 

  Serial.print("Tarjeta permitida: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  //LEDS
/*
  if((valor_der == HIGH) && (valor_izq == LOW)){
    rojoBlanco();
    i = 0;
    strip.clear();

  }
  else if((valor_izq == HIGH) && (valor_der == LOW)){
    todosBlanco();
    i = 0;
    strip.clear();
  
  } else{
    strip.clear();
  }   
*/
}