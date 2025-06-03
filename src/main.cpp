#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 46, 45);  // RX=13, TX=14
  Serial.println("Heltec listo para recibir datos...");
}

void loop() {
  if (Serial1.available()) {
    String mensaje = Serial1.readStringUntil('\n');
    Serial.print("Mensaje recibido: ");
    Serial.println(mensaje);

    // Enviar confirmación de vuelta al ESP32
    Serial1.println("Mensaje recibido correctamente");
  }
}
