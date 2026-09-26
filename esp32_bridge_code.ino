/*
  ESP32 GPIO16 (RX2) <----- SIM800 TX
  ESP32 GPIO17 (TX2) -----> SIM800 RX

  TX hamesha opposite device ke RX se connect hota hai.
  RX hamesha opposite device ke TX se connect hota hai.
*/


#include <HardwareSerial.h>
// ESP32 ki HardwareSerial library include kar rahe hain.
// Isse ESP32 ke hardware UART ports ko use kar sakte hain.
// SIM800L ke saath communication ke liye Hardware UART use karenge.


HardwareSerial sim800(2);
// HardwareSerial ka "sim800" naam ka object create kiya.
// (2) ka matlab ESP32 ka UART2 use karna hai.
// Yaha 2 GPIO 2 nahi hai, ye UART number 2 hai.


void setup()
{
  Serial.begin(115200);
  // Computer/PC ke Serial Monitor ke saath communication start.
  // 115200 = baud rate.
  // Is Serial ka use hum command type karne aur SIM800 ka response
  // Serial Monitor par dekhne ke liye karenge.


  sim800.begin(9600, SERIAL_8N1, 16, 17);
  // ESP32 aur SIM800L ke beech serial communication start.
  //
  // 9600        = communication baud rate
  // SERIAL_8N1  = 8 Data Bits, No Parity, 1 Stop Bit
  // 16          = ESP32 RX pin
  // 17          = ESP32 TX pin
  //
  // Connection:
  // SIM800 TX  ------> ESP32 GPIO16 (RX)
  // SIM800 RX  <------ ESP32 GPIO17 (TX)


  delay(3000);
  // 3 seconds wait.
  // Isse SIM800L ko properly initialize/stabilize hone ka time milta hai.


  Serial.println("ESP32 SIM800 Bridge Ready");
  // Serial Monitor par message print karega.
  // Iska matlab ESP32 ka SIM800 bridge ready ho gaya hai.
}


void loop()
{
  // ============================================================
  // PC / SERIAL MONITOR  ----->  ESP32  ----->  SIM800L
  // ============================================================


  while (Serial.available())
  // Check kar rahe hain ki Computer/Serial Monitor se
  // ESP32 ko koi data receive hua hai ya nahi.
  //
  // while ka matlab:
  // Jab tak data available hai, tab tak data read karte raho.
  {
    sim800.write(Serial.read());
    // Serial Monitor se ek character read karo
    // aur usi character ko SIM800L ko send karo.
    //
    // Example:
    // Agar Serial Monitor me "AT" type kiya:
    //
    // PC ---> ESP32 ---> SIM800L
    //       A          A
    //       T          T
    //
    // Yani ESP32 sirf command ko SIM800L tak forward kar raha hai.
  }


  // ============================================================
  // SIM800L  ----->  ESP32  ----->  PC / SERIAL MONITOR
  // ============================================================


  while (sim800.available())
  // Check kar rahe hain ki SIM800L ne ESP32 ko
  // koi response/data bheja hai ya nahi.
  //
  // Jab tak SIM800L ke buffer me data available hai,
  // tab tak usko read karte rahenge.
  {
    Serial.write(sim800.read());
    // SIM800L se ek character read karo
    // aur us character ko Computer ke Serial Monitor par bhejo.
    //
    // Example:
    //
    // SIM800L ---> ESP32 ---> PC
    //
    // SIM800L response:
    // OK
    //
    // Serial Monitor par:
    // OK
  }
}
