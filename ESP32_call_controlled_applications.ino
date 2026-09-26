// ================================================================
// ESP32 + SIM800L + RELAY
// Authorized Number se CALL aayegi
// → ESP32 number check karega
// → Authorized number hua to call reject karega
// → Relay state toggle karega
//
// Communication:
// Computer <----> ESP32 <----> SIM800L
//
// ESP32 UART2:
// RX2 = GPIO 16
// TX2 = GPIO 17
//
// SIM800L TX  ----------> ESP32 GPIO 16 (RX)
// SIM800L RX  <---------- ESP32 GPIO 17 (TX)
//
// Relay:
// ESP32 GPIO 2
// ================================================================


// ---------------------------------------------------------------
// ESP32 ki Hardware Serial/UART library
//
// ESP32 ke andar hardware UART available hote hain.
// Isliye SoftwareSerial ki zarurat nahi hai.
// ---------------------------------------------------------------
#include <HardwareSerial.h>


// ---------------------------------------------------------------
// HardwareSerial ka object create kar rahe hain.
//
// "2" ka matlab:
// ESP32 ka UART2 / Hardware Serial Port 2 use karna hai.
//
// IMPORTANT:
// Yaha 2 relay pin nahi hai.
// Ye UART number hai.
// ---------------------------------------------------------------
HardwareSerial gsm(2);


// ---------------------------------------------------------------
// Authorized mobile number
//
// Is number se call aayegi to ESP32 us call ko authorized
// maanega aur relay toggle karega.
//
// const char* = ek constant character string ka pointer.
// ---------------------------------------------------------------
const char *USER = "+919876543210";


// ---------------------------------------------------------------
// Relay ki initial state
//
// true  = HIGH
// false = LOW
//
// Yaha relay ki initial logical state true rakhi gayi hai.
//
// Baad mein:
// relay = !relay;
//
// se state toggle hogi:
//
// true  -> false
// false -> true
// ---------------------------------------------------------------
bool relay = true;


void setup()
{
  // =============================================================
  // 1. COMPUTER <----> ESP32 SERIAL COMMUNICATION
  // =============================================================

  // Computer ke Serial Monitor ke saath communication start.
  //
  // 115200 = baud rate
  //
  // Is Serial ka use debugging ke liye karenge.
  Serial.begin(115200);


  // =============================================================
  // 2. ESP32 <----> SIM800L SERIAL COMMUNICATION
  // =============================================================

  // GSM module ke saath UART2 communication start.
  //
  // gsm.begin(
  //     baud rate,
  //     serial format,
  //     ESP32 RX pin,
  //     ESP32 TX pin
  // );
  //
  // 9600       = SIM800L baud rate
  // SERIAL_8N1 = 8 data bits, No parity, 1 stop bit
  // 16         = ESP32 RX pin
  // 17         = ESP32 TX pin
  //
  // Connection:
  //
  // SIM800L TX  ----------> ESP32 GPIO 16 (RX)
  // SIM800L RX  <---------- ESP32 GPIO 17 (TX)
  // =============================================================
  gsm.begin(9600, SERIAL_8N1, 16, 17);


  // =============================================================
  // 3. RELAY PIN SETUP
  // =============================================================

  // GPIO 2 ko output bana rahe hain
  // kyunki ESP32 is pin se relay control karega.
  pinMode(2, OUTPUT);


  // Relay ko starting state HIGH de rahe hain.
  //
  // IMPORTANT:
  // Ye relay module ke type par depend karta hai ki
  // HIGH par relay ON hoga ya OFF.
  //
  // Is code mein logical relay state ko true se start kiya gaya hai.
  digitalWrite(2, HIGH);


  // SIM800L ko initialize hone ke liye 3 seconds wait.
  delay(3000);


  // =============================================================
  // 4. SIM800L TEST
  // =============================================================

  // SIM800L ko AT command send.
  //
  // Agar communication correct hai:
  //
  // AT
  // OK
  //
  // response milega.
  gsm.println("AT");


  // SIM800L ke response ke liye thoda wait.
  delay(500);


  // =============================================================
  // 5. CALLER ID ENABLE
  // =============================================================

  // CLIP = Calling Line Identification Presentation
  //
  // AT+CLIP=1 ka matlab:
  // Incoming call aane par caller ka number SIM800L
  // ESP32 ko provide kare.
  //
  // Example response:
  //
  // +CLIP: "+919876543210",145,...
  //
  // Isi number ko hum USER ke saath compare karenge.
  gsm.println("AT+CLIP=1");
}


void loop()
{
  // =============================================================
  // CHECK:
  // Kya SIM800L ne koi data ESP32 ko bheja hai?
  // =============================================================

  if (gsm.available())
  {
    // -----------------------------------------------------------
    // SIM800L se ek complete line read kar rahe hain.
    //
    // readStringUntil('\n'):
    // Jab tak newline '\n' nahi milta,
    // tab tak characters read karta rahega.
    //
    // Incoming GSM response example:
    //
    // RING
    //
    // +CLIP: "+919876543210",145,...
    // -----------------------------------------------------------
    String data = gsm.readStringUntil('\n');


    // SIM800L se jo data mila,
    // use Serial Monitor par display karo.
    //
    // Isse debugging easy hoti hai.
    Serial.println(data);


    // =========================================================
    // AUTHORIZED NUMBER CHECK
    // =========================================================

    // Check kar rahe hain ki received GSM data ke andar
    // authorized number USER present hai ya nahi.
    //
    // indexOf(USER):
    //
    // Agar number mil gaya:
    //     -1 se different value
    //
    // Agar number nahi mila:
    //     -1
    // =========================================================
    if (data.indexOf(USER) != -1)
    {

      // ---------------------------------------------------------
      // AUTHORIZED CALL MIL GAYI
      // ---------------------------------------------------------

      // ATH = Hang Up
      //
      // Incoming call ko reject/end kar diya jayega.
      //
      // Isliye user ko call connected rakhne ki zarurat nahi.
      gsm.println("ATH");


      // ---------------------------------------------------------
      // RELAY STATE TOGGLE
      // ---------------------------------------------------------

      // ! ka matlab NOT / opposite state.
      //
      // Agar relay = true:
      //     !true = false
      //
      // Agar relay = false:
      //     !false = true
      //
      // Matlab har authorized call par relay ki state change hogi.
      relay = !relay;


      // ---------------------------------------------------------
      // NEW RELAY STATE GPIO 2 PAR SEND
      // ---------------------------------------------------------

      // relay ki current state GPIO 2 par output kar rahe hain.
      digitalWrite(2, relay);


      // Serial Monitor par current relay state show.
      //
      // relay true  -> "Relay ON"
      // relay false -> "Relay OFF"
      //
      // ?: = ternary operator
      Serial.println(relay ? "Relay ON" : "Relay OFF");


      // ---------------------------------------------------------
      // DUPLICATE CLIP RESPONSE IGNORE KARNE KE LIYE DELAY
      // ---------------------------------------------------------

      // SIM800L ek hi incoming call ke liye multiple
      // CLIP/RING related messages bhej sakta hai.
      //
      // 1500 ms = 1.5 seconds
      //
      // Is delay se duplicate detection ko reduce karne ki
      // koshish ki gayi hai.
      delay(1500);
    }
  }
}
