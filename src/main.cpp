#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Pines IR
const uint16_t PIN_REFLECTOR = 48; // Reflector
const uint16_t PIN_TIRA_LED = 47;  // Tira LED

// Objetos IR
IRsend irReflector(PIN_REFLECTOR);
IRsend irTiraLed(PIN_TIRA_LED);

// Códigos IR del reflector (24 botones)
const uint64_t controlReflector[24] = {
  0xff906f, 0xffb847, 0xfff807, 0xffb04f,
  0xff9867, 0xffd827, 0xff8877, 0xffa857,
  0xffe817, 0xff48b7, 0xff6897, 0xffb24d,
  0xff02fd, 0xff32cd, 0xff20df, 0xff00ff,
  0xff50af, 0xff7887, 0xff708f, 0xff58a7,
  0xff38c7, 0xff28d7, 0xfff00f, 0xff30cf
};

// Códigos IR de la tira LED (24 botones)
const uint64_t controlTiraLed[24] = {
  0x77906f, 0x77b847, 0x77f807, 0x77b04f,
  0x779867, 0x77d827, 0x778877, 0x77a857,
  0x77e817, 0x7748b7, 0x776897, 0x77b24d,
  0x7702fd, 0x7732cd, 0x7720df, 0x7700ff,
  0x7750af, 0x777887, 0x77708f, 0x7758a7,
  0x7738c7, 0x7728d7, 0x77f00f, 0x7730cf
};

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 46, 45);  // RX=46, TX=45

  irReflector.begin();
  irTiraLed.begin();

  Serial.println("ESP32 receptor listo para reflector y tira LED...");
}

void loop() {
  if (Serial1.available()) {
    String mensaje = Serial1.readStringUntil('\n');
    mensaje.trim();  // Elimina espacios y saltos de línea
    Serial.print("Mensaje recibido: ");
    Serial.println(mensaje);

    if (mensaje.startsWith("R")) {
      int num = mensaje.substring(1).toInt();
      if (num >= 1 && num <= 24) {
        uint64_t codigo = controlReflector[num - 1];
        irReflector.sendNEC(codigo, 32);
        Serial.println("Código IR enviado al reflector.");
      } else {
        Serial.println("Número fuera de rango para reflector.");
      }
    }
    else if (mensaje.startsWith("T")) {
      int num = mensaje.substring(1).toInt();
      if (num >= 1 && num <= 24) {
        uint64_t codigo = controlTiraLed[num - 1];
        irTiraLed.sendNEC(codigo, 32);
        Serial.println("Código IR enviado a la tira LED.");
      } else {
        Serial.println("Número fuera de rango para tira LED.");
      }
    }
    else {
      Serial.println("Comando no reconocido. Usa R1-R24 o T1-T24.");
    }

    // Confirmación
    Serial1.println("Mensaje procesado correctamente");
  }
}
