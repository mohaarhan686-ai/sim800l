// ================================================================
// SIM800L SERIAL COMMUNICATION TEST
// Arduino/ESP8266 <----> SIM800L <----> Computer Serial Monitor
//
// IMPORTANT:
// Arduino TX  -----------> GSM RX
// Arduino RX  <----------- GSM TX
//
// GSM RX ko Arduino TX ke voltage level se protect karne ke liye
// suitable voltage divider/level shifting use karein.
//
// ESP8266 use kar rahe ho to pins accordingly change karo.
// ESP8266 me pin number ke according D1, D2 etc. use kar sakte ho.
// ================================================================


// SoftwareSerial library use kar rahe hain
// Is library se hum Arduino ke normal hardware Serial ke alawa
// ek aur RX-TX serial communication bana sakte hain.
#include <SoftwareSerial.h>


// ---------------------------------------------------------------
// SoftwareSerial object create kar rahe hain
//
// SoftwareSerial sim800(RX, TX);
//
// Yaha:
// Arduino pin 7 = RX
// Arduino pin 8 = TX
//
// Matlab:
// SIM800 TX  ----------> Arduino pin 7 (RX)
// SIM800 RX  <---------- Arduino pin 8 (TX)
// ---------------------------------------------------------------
SoftwareSerial sim800(7, 8);


void setup()
{
  // -------------------------------------------------------------
  // Computer ke Serial Monitor ke saath communication start
  // -------------------------------------------------------------
  //
  // Ye Serial Arduino/ESP ko computer se connect karta hai.
  // Serial Monitor me hum AT commands type karenge.
  //
  // 115200 = baud rate
  // -------------------------------------------------------------
  Serial.begin(115200);


  // -------------------------------------------------------------
  // SIM800L ke saath serial communication start
  // -------------------------------------------------------------
  //
  // Yaha SIM800L ka baud rate 9600 hai.
  //
  // Ab:
  // Arduino/ESP  <----9600---->  SIM800L
  //
  // Dono side same baud rate hona chahiye.
  // -------------------------------------------------------------
  sim800.begin(9600);


  // -------------------------------------------------------------
  // SIM800L ko start hone ke liye thoda time de rahe hain.
  // -------------------------------------------------------------
  delay(3000);


  // -------------------------------------------------------------
  // SIM800L ko automatically "AT" command bhej rahe hain.
  //
  // AT = basic attention/test command.
  //
  // Agar SIM800L properly connected hai to response:
  //
  // OK
  //
  // F("AT") ka matlab:
  // String "AT" ko RAM ke bajay Flash memory me store karna.
  // Isse RAM thodi save hoti hai.
  // -------------------------------------------------------------
  sim800.write(F("AT"));


  // -------------------------------------------------------------
  // SIM800L ko response dene ke liye 1 second wait
  // -------------------------------------------------------------
  delay(1000);


  // Ab setup complete.
  //
  // Initial AT command send ho chuka hai.
  // Ab actual communication loop() ke andar chalega.
}


void loop()
{
  // =============================================================
  // PART 1:
  // COMPUTER SERIAL MONITOR -> ARDUINO/ESP -> SIM800L
  // =============================================================

  // Check kar rahe hain ki computer ke Serial Monitor se
  // koi data/command receive hua hai ya nahi.
  //
  // Agar hum Serial Monitor me:
  //
  // AT
  //
  // type karenge to Serial.available() true ho jayega.
  if (Serial.available() > 0)
  {
    // Serial Monitor se ek character read karo
    // aur usi character ko SIM800L ko send karo.
    //
    // Example:
    //
    // Serial Monitor:
    // A T Enter
    //
    // Arduino:
    // A -> SIM800
    // T -> SIM800
    //
    // Is tarah command SIM800L tak pahunchti hai.
    sim800.write(Serial.read());
  }


  // =============================================================
  // PART 2:
  // SIM800L -> ARDUINO/ESP -> COMPUTER SERIAL MONITOR
  // =============================================================

  // Ab check kar rahe hain ki SIM800L ne koi response/data
  // Arduino ko bheja hai ya nahi.
  if (sim800.available() > 0)
  {
    // SIM800L se ek character read karo
    // aur us character ko computer ke Serial Monitor par print karo.
    //
    // Example:
    //
    // SIM800L:
    // OK
    //
    // Arduino:
    // OK ko Serial Monitor par display karega.
    Serial.write(sim800.read());
  }
}
