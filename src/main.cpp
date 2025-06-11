/*#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_PIN 35

// Pines asignados a reflectores (evitamos 45 y 46)
const uint8_t reflectorPins[7] = {1, 2, 3, 4, 5, 6, 7};
IRsend* irReflectores[7];  // Objetos IR individuales

const uint64_t controlReflector[24] = {
  0x77906f, 0x77b847, 0x77f807, 0x77b04f,
  0x779867, 0x77d827, 0x778877, 0x77a857,
  0x77e817, 0x7748b7, 0x776897, 0x77b24d,
  0x7702fd, 0x7732cd, 0x7720df, 0x7700ff,
  0x7750af, 0x777887, 0x77708f, 0x7758a7,
  0x7738c7, 0x7728d7, 0x77f00f, 0x7730cf
};



void parpadearLed() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 46, 45);  // RX=46, TX=45

  // Inicializar los emisores IR
  for (int i = 0; i < 7; i++) {
    irReflectores[i] = new IRsend(reflectorPins[i]);
    irReflectores[i]->begin();
    Serial.printf("Reflector %d configurado en pin %d\n", i + 1, reflectorPins[i]);
  }

  Serial.println("Receptor listo. Esperando comandos...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (Serial2.available()) {
    String comando = Serial2.readStringUntil('\n');
    comando.trim();
    Serial.print("Comando recibido: ");
    Serial.println(comando);

    // Validar formato
    if (comando.startsWith("LORA>R") && comando.length() >= 8) {
      int reflectorID = comando.substring(6, 7).toInt();  // número 0–7
      int funcion = comando.substring(7).toInt();         // número 1–24

      if (reflectorID >= 0 && reflectorID <= 7 && funcion >= 1 && funcion <= 24) {
        uint64_t codigoIR = controlReflector[funcion - 1];

        if (reflectorID == 0) {
          // Todos los reflectores
          Serial.printf("Enviando función %d a TODOS los reflectores...\n", funcion);
          for (int i = 0; i < 7; i++) {
            irReflectores[i]->sendNEC(codigoIR);
            parpadearLed();
            delay(50);  // pequeño retardo entre envíos
          }
        } else {
          // Uno específico
          Serial.printf("Enviando función %d al reflector %d (pin %d)\n", 
                        funcion, reflectorID, reflectorPins[reflectorID - 1]);
          irReflectores[reflectorID - 1]->sendNEC(codigoIR);
          parpadearLed();
        }
      } else {
        Serial.println("Error: Números fuera de rango.");
      }
    } else {
      Serial.println("Formato incorrecto. Usa: LORA>R[d][f]");
    }
  }
}*/


#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <RCSwitch.h>

#define LED_PIN 35

RCSwitch TransmisorRF = RCSwitch();

// Pines asignados a reflectores (evitamos 45 y 46, y se incluye el pin 48 como el 8.º reflector)
const uint8_t reflectorPins[8] = {1, 2, 3, 4, 5, 6, 7, 48};
IRsend* irReflectores[8];  // Objetos IR individuales

/*const uint64_t controlReflector[24] = {
  0x77906f, 0x77b847, 0x77f807, 0x77b04f,
  0x779867, 0x77d827, 0x778877, 0x77a857,
  0x77e817, 0x7748b7, 0x776897, 0x77b24d,
  0x7702fd, 0x7732cd, 0x7720df, 0x7700ff,
  0x7750af, 0x777887, 0x77708f, 0x7758a7,
  0x7738c7, 0x7728d7, 0x77f00f, 0x7730cf
};*/

const uint64_t controlReflector[24] = {
  0xffa05f, 0xff20df, 0xff609f, 0xffe01f,
  0xff906f, 0xff10ef, 0xff50af, 0xffd02f,
  0xffb04f, 0xff30cf, 0xff708f, 0xfff00f,
  0xffa857, 0xff28d7, 0xff6897, 0xffe817,
  0xff9867, 0xff18e7, 0xff58a7, 0xffd827,
  0xff8877, 0xff08f7, 0xff48b7, 0xffc837
};

const uint64_t controlRF[4] = {
  8999768, 8999764, 8999762, 8999761
};

void parpadearLed() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 46, 45);  // RX=46, TX=45
  TransmisorRF.enableTransmit(33);  // Pin de transmisión RF

  // Inicializar los emisores IR
  for (int i = 0; i < 8; i++) {
    irReflectores[i] = new IRsend(reflectorPins[i]);
    irReflectores[i]->begin();
    Serial.printf("Reflector %d configurado en pin %d\n", i + 1, reflectorPins[i]);
  }

  Serial.println("Receptor listo. Esperando comandos...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (Serial2.available()) {
    String comando = Serial2.readStringUntil('\n');
    comando.trim();
    Serial.print("Comando recibido: ");
    Serial.println(comando);

    // Validar formato
    if (comando.startsWith("LORA>R") && comando.length() >= 8) {
      int reflectorID = comando.substring(6, 7).toInt();  // Número 0–8
      int funcion = comando.substring(7).toInt();         // Número Botón 1–24, o RF 25–28

      if (reflectorID >= 0 && reflectorID <= 8 && funcion >= 1 && funcion <= 24) {
        uint64_t codigoIR = controlReflector[funcion - 1];

        if (reflectorID == 0) {
          // Todos los reflectores
          Serial.printf("Enviando función %d a TODOS los reflectores...\n", funcion);
          for (int i = 0; i < 8; i++) {
            irReflectores[i]->sendNEC(codigoIR);
            parpadearLed();
            delay(50);  // pequeño retardo entre envíos
          }
        } else {
          // Uno específico
          Serial.printf("Enviando función %d al reflector %d (pin %d)\n", 
                        funcion, reflectorID, reflectorPins[reflectorID - 1]);
          irReflectores[reflectorID - 1]->sendNEC(codigoIR);
          parpadearLed();
        }

      } else if (funcion >= 25 && funcion <= 28) {
        // Enviar comando por RF en vez de IR
        int rfIndex = funcion - 25;  // Convertir 25-28 a 0-3
        if (rfIndex >= 0 && rfIndex < 4) {
          Serial.printf("Enviando comando RF %d: %llu\n", funcion, controlRF[rfIndex]);
          TransmisorRF.send(controlRF[rfIndex], 27);  // Enviar el código RF
          parpadearLed();
        } else {
          Serial.println("Error: Comando RF fuera de rango.");
        }

      } else {
        Serial.println("Error: Números fuera de rango.");
      }
    } else {
      Serial.println("Formato incorrecto. Usa: LORA>R[r][b]");
    }
  }
  delay(100);
}

