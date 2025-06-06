#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define LED_PIN 35

// Pines asignados a reflectores (evitamos 45 y 46)
const uint8_t reflectorPins[7] = {1, 2, 3, 4, 5, 6, 7};
IRsend* irReflectores[7];  // Objetos IR individuales

const uint64_t controlReflector[24] = {
  0xffa05f, 0xff20df, 0xff609f, 0xffe01f,
  0xff906f, 0xff10ef, 0xff50af, 0xffd02f,
  0xffb04f, 0xff30cf, 0xff708f, 0xfff00f,
  0xffa857, 0xff28d7, 0xff6897, 0xffe817,
  0xff9867, 0xff18e7, 0xff58a7, 0xffd827,
  0xff8877, 0xff08f7, 0xff48b7, 0xffc837
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
}
