#include <HardwareSerial.h>
// ESP32 ki HardwareSerial library include kar rahe hain.
// Isse ESP32 ka hardware UART use karke SIM800L se
// serial communication kar sakte hain.


HardwareSerial gsm(2);
// HardwareSerial ka "gsm" object create kiya.
// (2) ka matlab ESP32 ka UART2 use karna hai.
// Ye GPIO 2 nahi hai, ye UART number 2 hai.


// ================================================================
// RELAY / CHANNEL PINS
// ================================================================

#define CH1 2
// Channel 1 ke relay ko ESP32 GPIO 2 se control karenge.

#define CH2 4
// Channel 2 ke relay ko ESP32 GPIO 4 se control karenge.

#define CH3 5
// Channel 3 ke relay ko ESP32 GPIO 5 se control karenge.

#define CH4 18
// Channel 4 ke relay ko ESP32 GPIO 18 se control karenge.


// ================================================================
// AUTHORIZED USER
// ================================================================

const String USER = "+919876543210";
// Sirf is mobile number se aane wale SMS commands accept honge.
// Ye authorized/whitelisted mobile number hai.


// ================================================================
// SMS MESSAGE STORE KARNE KE LIYE VARIABLE
// ================================================================

String msg;
// SIM800L se receive hua complete data/SMS is String mein store hoga.


void setup()
{
  // ==============================================================
  // 1. COMPUTER <----> ESP32 SERIAL COMMUNICATION
  // ==============================================================

  Serial.begin(115200);
  // Computer ke Serial Monitor ke saath communication start.
  // 115200 = baud rate.
  // Iska use debugging ke liye hoga.


  // ==============================================================
  // 2. ESP32 <----> SIM800L COMMUNICATION
  // ==============================================================

  gsm.begin(9600, SERIAL_8N1, 16, 17);
  // ESP32 ke UART2 ko SIM800L ke saath start kar rahe hain.
  //
  // 9600       = Baud rate
  // SERIAL_8N1 = 8 Data Bits, No Parity, 1 Stop Bit
  // 16         = ESP32 RX
  // 17         = ESP32 TX
  //
  // Connection:
  //
  // SIM800L TX  ------> ESP32 GPIO16 (RX)
  // SIM800L RX  <------ ESP32 GPIO17 (TX)


  // ==============================================================
  // 3. RELAY PINS KO OUTPUT BANANA
  // ==============================================================

  pinMode(CH1, OUTPUT);
  // GPIO 2 ko output banaya.
  // Ab ESP32 is pin se CH1 relay ko control karega.


  pinMode(CH2, OUTPUT);
  // GPIO 4 ko output banaya.
  // CH2 relay control.


  pinMode(CH3, OUTPUT);
  // GPIO 5 ko output banaya.
  // CH3 relay control.


  pinMode(CH4, OUTPUT);
  // GPIO 18 ko output banaya.
  // CH4 relay control.


  // ==============================================================
  // 4. SIM800L TEST COMMAND
  // ==============================================================

  gsm.println("AT");
  // SIM800L ko basic AT command bhej rahe hain.
  //
  // Agar SIM800L properly connected hai to normally:
  //
  // OK
  //
  // response milega.


  delay(1000);
  // SIM800L ko response dene ke liye 1 second wait.


  // ==============================================================
  // 5. SMS TEXT MODE
  // ==============================================================

  gsm.println("AT+CMGF=1");
  // SMS ko TEXT MODE mein set kar rahe hain.
  //
  // CMGF = SMS Message Format
  //
  // 1 = Text mode
  // 0 = PDU mode
  //
  // Hum simple text commands jaise:
  //
  // CH1 ON
  // CH2 OFF
  //
  // receive karna chahte hain.


  // ==============================================================
  // 6. NEW SMS DIRECTLY SERIAL PAR RECEIVE KARNA
  // ==============================================================

  gsm.println("AT+CNMI=2,2,0,0,0");
  // New SMS aane par SIM800L SMS information/data ko
  // directly ESP32 ke serial interface par bhejega.
  //
  // Iske baad incoming SMS ka sender number aur message
  // ESP32 ke gsm serial buffer mein aayega.
}


void loop() 
{
  // ==============================================================
  // CHECK:
  // Kya SIM800L ne ESP32 ko koi data bheja?
  // ==============================================================

  if(gsm.available())
  {
    // Agar SIM800L ke serial buffer mein data available hai
    // to andar ka code execute hoga.


    // ============================================================
    // SMS / GSM DATA READ KARNA
    // ============================================================

    msg = gsm.readString();
    // SIM800L se available complete serial data ko read karke
    // "msg" String mein store kar rahe hain.
    //
    // Example incoming data:
    //
    // +CMT: "+919876543210",...
    // CH1 ON
    //
    // Ye poora data "msg" mein aa jayega.


    Serial.println(msg);
    // Jo data SIM800L se receive hua,
    // usko Serial Monitor par print kar rahe hain.
    //
    // Debugging ke liye useful hai.


    // ============================================================
    // AUTHORIZED NUMBER CHECK
    // ============================================================

    if(msg.indexOf(USER) == -1)
      return;
    // YE CODE KI SABSE IMPORTANT SECURITY LINE HAI.
    //
    // msg.indexOf(USER)
    // received SMS/GSM data ke andar authorized mobile number
    // search karta hai.
    //
    // Agar number mil gaya:
    //
    // indexOf() -> 0 ya koi positive number
    //
    // Agar number nahi mila:
    //
    // indexOf() -> -1
    //
    // Isliye:
    //
    // == -1
    //
    // ka matlab:
    // "Authorized number nahi mila."
    //
    // Agar authorized number nahi mila to:
    //
    // return;
    //
    // loop() ke current execution ko yahin stop kar do.
    //
    // Matlab unknown number ka SMS ignore ho jayega
    // aur relay control commands execute nahi hongi.


    // ============================================================
    // CHANNEL 1 CONTROL
    // ============================================================

    if(msg.indexOf("CH1 ON")!=-1)
      digitalWrite(CH1,HIGH);
    // Agar SMS/message mein "CH1 ON" mila:
    //
    // CH1 pin ko HIGH karo.
    //
    // Example SMS:
    // CH1 ON
    //
    // GPIO 2 -> HIGH
    //
    // Relay module active-HIGH hai to relay ON hoga.


    if(msg.indexOf("CH1 OFF")!=-1)
      digitalWrite(CH1,LOW);
    // Agar "CH1 OFF" mila:
    //
    // CH1 pin LOW.
    //
    // GPIO 2 -> LOW
    //
    // Relay OFF ho jayega agar module active-HIGH hai.


    // ============================================================
    // CHANNEL 2 CONTROL
    // ============================================================

    if(msg.indexOf("CH2 ON")!=-1)
      digitalWrite(CH2,HIGH);
    // "CH2 ON" command mili:
    // GPIO 4 HIGH.


    if(msg.indexOf("CH2 OFF")!=-1)
      digitalWrite(CH2,LOW);
    // "CH2 OFF" command mili:
    // GPIO 4 LOW.


    // ============================================================
    // CHANNEL 3 CONTROL
    // ==============================================================

    if(msg.indexOf("CH3 ON")!=-1)
      digitalWrite(CH3,HIGH);
    // "CH3 ON" command:
    // GPIO 5 HIGH.


    if(msg.indexOf("CH3 OFF")!=-1)
      digitalWrite(CH3,LOW);
    // "CH3 OFF" command:
    // GPIO 5 LOW.


    // ============================================================
    // CHANNEL 4 CONTROL
    // ==============================================================

    if(msg.indexOf("CH4 ON")!=-1)
      digitalWrite(CH4,HIGH);
    // "CH4 ON" command:
    // GPIO 18 HIGH.


    if(msg.indexOf("CH4 OFF")!=-1)
      digitalWrite(CH4,LOW);
    // "CH4 OFF" command:
    // GPIO 18 LOW.


    // ============================================================
    // ALL CHANNELS ON
    // ============================================================

    if(msg.indexOf("ALL ON")!=-1)
    {
      // Agar SMS mein "ALL ON" mila,
      // to saare four channels ON kar do.

      digitalWrite(CH1,HIGH);
      // CH1 ON

      digitalWrite(CH2,HIGH);
      // CH2 ON

      digitalWrite(CH3,HIGH);
      // CH3 ON

      digitalWrite(CH4,HIGH);
      // CH4 ON
    }


    // ============================================================
    // ALL CHANNELS OFF
    // ============================================================

    if(msg.indexOf("ALL OFF")!=-1)
    {
      // Agar SMS mein "ALL OFF" mila,
      // to saare four channels OFF kar do.

      digitalWrite(CH1,LOW);
      // CH1 OFF

      digitalWrite(CH2,LOW);
      // CH2 OFF

      digitalWrite(CH3,LOW);
      // CH3 OFF

      digitalWrite(CH4,LOW);
      // CH4 OFF
    }
  }
}
