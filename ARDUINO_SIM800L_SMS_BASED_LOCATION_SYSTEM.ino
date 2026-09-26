// ================================================================
// ARDUINO + SIM800L SMS BASED LOCATION SYSTEM
//
// IMPORTANT:
// Arduino TX  ----------------->  SIM800 RX
// Arduino RX  <-----------------  SIM800 TX
//
// Arduino TX ka voltage SIM800 RX ke liye suitable banane ke liye
// voltage divider / level shifting use karein.
// ================================================================


#include <SoftwareSerial.h>
// SoftwareSerial library include kar rahe hain.
// Isse Arduino ke digital pins ko RX/TX serial communication
// ke liye use kar sakte hain.


// ================================================================
// SIM800L SERIAL SETUP
// ================================================================

SoftwareSerial sim800(7, 8);
// SoftwareSerial object "sim800" create kiya.
//
// sim800(RX, TX)
//
// Arduino pin 7 = RX
// Arduino pin 8 = TX
//
// Connection:
// SIM800 TX  ------> Arduino pin 7 (RX)
// SIM800 RX  <------ Arduino pin 8 (TX)


// ================================================================
// SMS DATA STORE KARNE KE LIYE VARIABLES
// ================================================================

String incomingSMS = "";
// SIM800L se aane wale SMS data ko temporarily store karega.


String phoneNumber = "";
// SMS bhejne wale sender ka mobile number isme store hoga.


// ================================================================
// AUTHORIZED USERS / WHITELIST
// ================================================================
//
// Sirf in numbers se SMS aane par location reply ki jayegi.
// Baaki unknown numbers ko ignore kar diya jayega.
// ================================================================

const int MAX_USERS = 5;
// Maximum 5 authorized numbers store kar sakte hain.


String authorizedUsers[MAX_USERS] = {

  "+919876543210",   // Mom
  // Authorized user 1

  "+919812345678",   // Dad
  // Authorized user 2

  "+919900112233",   // Me
  // Authorized user 3

  "",                // Empty slot
  // Future mein ek aur number add kar sakte hain.

  ""                 // Empty slot
  // Future mein ek aur number add kar sakte hain.
};


// ================================================================
// SETUP
// ================================================================

void setup() 
{
  Serial.begin(9600);
  // Computer/Serial Monitor ke saath communication start.
  // 9600 = baud rate.
  // Iska use debugging aur messages dekhne ke liye hoga.


  sim800.begin(9600);
  // Arduino aur SIM800L ke beech serial communication start.
  // SIM800L bhi 9600 baud par communicate karega.


  delay(4000);
  // SIM800L ko initialize/network side par ready hone ke liye
  // 4 seconds wait.


  sim800.println("AT");
  // SIM800L ko basic AT test command bhej rahe hain.
  //
  // Agar communication sahi hai to SIM800L normally:
  //
  // OK
  //
  // response dega.


  delay(1000);
  // SIM800L ko response dene ke liye 1 second wait.


  sim800.println("AT+CMGF=1");
  // SMS ko TEXT MODE mein set kar rahe hain.
  //
  // CMGF = SMS Message Format
  //
  // 1 = Text mode
  // 0 = PDU mode
  //
  // Hum text mode use karenge kyunki SMS ko simple String
  // ke form mein handle karna hai.


  delay(1000);
  // SIM800L ko command process karne ka time.


  sim800.println("AT+CNMI=2,2,0,0,0");
  // New SMS aane par SIM800L ko SMS ka data directly
  // serial interface par ESP/Arduino ko provide karne ke liye
  // configure kar rahe hain.
  //
  // Is wajah se new SMS receive hote hi SIM800L serial output
  // mein sender number aur message related data aa sakta hai.


  delay(1000);
  // Command process hone ke liye wait.


  sim800.println("AT+CIPGSMLOC=1,1");
  // SIM800L ke GSM/LBS based location function ko trigger kar rahe hain.
  //
  // Ye GPS nahi hai.
  // SIM800L GSM network ke Cell/LBS information se approximate
  // location obtain karne ki koshish karta hai.


  delay(1000);
  // SIM800L ko location service initialize/warm-up karne ka time.


  Serial.println("Ready! Only authorized users will get a reply.");
  // Serial Monitor par status message print.
  //
  // Matlab system ab SMS receive karne ke liye ready hai.
}


// ================================================================
// MAIN LOOP
// ================================================================

void loop()
{

  // --------------------------------------------------------------
  // CHECK:
  // Kya SIM800L se koi data Arduino ko mila?
  // --------------------------------------------------------------

  if (sim800.available()) 
  {
    
    char c = sim800.read();
    // SIM800L se ek character read kar rahe hain.
    //
    // SMS data ek saath nahi bhi aa sakta,
    // isliye characters ko one-by-one read kar rahe hain.


    incomingSMS += c;
    // Jo character mila usko incomingSMS String ke end mein
    // add kar rahe hain.
    //
    // Example:
    //
    // "R" → incomingSMS = "R"
    // "I" → incomingSMS = "RI"
    // "N" → incomingSMS = "RIN"
    //
    // Dheere-dheere complete GSM response/SMS data collect hoga.


    // ----------------------------------------------------------
    // CHECK:
    // Kya incoming data mein "+CMT:" aa gaya?
    //
    // +CMT: generally new incoming SMS ka indication hota hai
    // jab CNMI setting SMS ko directly serial interface par
    // deliver kar rahi ho.
    //
    // Aur "\n" check karke dekh rahe hain ki line complete hui
    // ya nahi.
    // ----------------------------------------------------------

    if (incomingSMS.indexOf("+CMT:") >= 0 &&
        incomingSMS.indexOf("\n") > 0) 
    {
      
      delay(500);
      // Thoda wait kar rahe hain taaki incoming SMS ka
      // additional data buffer mein aa sake.


      // ========================================================
      // SENDER PHONE NUMBER EXTRACT KARNA
      // ========================================================

      int quoteStart = incomingSMS.indexOf("\"") + 1;
      // First quotation mark '"' find kar rahe hain.
      //
      // Example:
      //
      // +CMT: "+919876543210",...
      //       ↑
      //       quotation mark
      //
      // +1 isliye kiya kyunki hume quote ke baad se number
      // read karna hai.


      int quoteEnd = incomingSMS.indexOf("\"", quoteStart);
      // Ab second quotation mark find kar rahe hain.
      //
      // Example:
      //
      // "+919876543210"
      //  ↑            ↑
      // Start        End


      phoneNumber = incomingSMS.substring(quoteStart, quoteEnd);
      // Dono quotes ke beech ka actual mobile number extract kar rahe hain.
      //
      // Result:
      //
      // phoneNumber = "+919876543210"


      phoneNumber.trim();
      // Number ke beginning/end mein agar extra spaces hain
      // to remove kar deta hai.


      Serial.println("SMS from: " + phoneNumber);
      // Serial Monitor par sender ka number display.


      // ========================================================
      // AUTHORIZED NUMBER CHECK
      // ========================================================

      if (isAuthorized(phoneNumber)) 
      {
        // Check kar rahe hain:
        //
        // Kya sender ka number authorizedUsers list mein hai?
        //
        // Agar YES:
        // location obtain karke reply karo.


        Serial.println("Authorized. Sending location...");
        // Serial Monitor par authorized status show.


        getLocationAndReply();
        // Location obtain karo
        // aur sender ko SMS ke through Google Maps link bhejo.
      }


      else
      {
        // Agar number authorized list mein nahi mila.


        Serial.println("Unauthorized. Ignoring.");
        // Unauthorized sender ko ignore kar rahe hain.


        // Optional:
        // Agar chaho to unauthorized user ko reply bhej sakte ho.
        //
        // sendSMS(phoneNumber, "Access denied.");
        //
        // Abhi ye line comment hai, isliye koi reply nahi jayega.
      }


      // --------------------------------------------------------
      // Current SMS/GSM data ko clear kar rahe hain.
      //
      // Taaki next SMS ka data purane data ke saath mix na ho.
      // --------------------------------------------------------

      incomingSMS = "";
    }
  }
}


// ================================================================
// AUTHORIZED NUMBER CHECK FUNCTION
// ================================================================

bool isAuthorized(String number) 
{
  number.trim();
  // Received number ke extra spaces remove.


  // ------------------------------------------------------------
  // Authorized users ki poori list check karenge.
  // ------------------------------------------------------------

  for (int i = 0; i < MAX_USERS; i++) 
  {

    // Check:
    //
    // 1. Authorized slot empty nahi hai
    // 2. Received number aur authorized number same hain
    //

    if (authorizedUsers[i].length() > 0 &&
        number == authorizedUsers[i]) 
    {
      
      return true;
      // Number mil gaya.
      //
      // true return = Authorized user.
    }
  }


  return false;
  // Poore list mein number nahi mila.
  //
  // false = Unauthorized user.
}


// ================================================================
// LOCATION OBTAIN KARKE SMS REPLY KARNA
// ================================================================

void getLocationAndReply() 
{

  // --------------------------------------------------------------
  // SIM800L ke serial buffer mein agar purana data pada hai
  // to usko clear kar rahe hain.
  // --------------------------------------------------------------

  while (sim800.available())
    sim800.read();


  // --------------------------------------------------------------
  // SIM800L ko location command send.
  //
  // CIPGSMLOC = GSM/LBS location related command.
  // --------------------------------------------------------------

  sim800.println("AT+CIPGSMLOC=1,1");


  delay(3000);
  // Location response ke liye 3 seconds wait.


  String response = "";
  // SIM800L ka location response store karne ke liye
  // empty String create.


  // --------------------------------------------------------------
  // SIM800L se available response read karte rahenge.
  // --------------------------------------------------------------

  while (sim800.available()) 
  {
    response += (char)sim800.read();
    // SIM800L se ek-ek character read karke response String
    // mein add kar rahe hain.
  }


  Serial.println("Response: " + response);
  // Complete SIM800L response Serial Monitor par display.


  // ============================================================
  // DEFAULT LOCATION VALUES
  // ============================================================

  String lat = "0", lng = "0";
  // Initially latitude aur longitude ko 0 rakha.
  //
  // Agar actual location nahi mili to ye 0 hi rahenge.


  int locStart = response.indexOf("+CIPGSMLOC: ");
  // Response ke andar "+CIPGSMLOC: " text search kar rahe hain.
  //
  // Agar mil gaya:
  // locStart >= 0
  //
  // Agar nahi mila:
  // locStart = -1


  // ============================================================
  // LOCATION DATA MILA?
  // ============================================================

  if (locStart >= 0) 
  {
    
    String data = response.substring(locStart + 11);
    // "+CIPGSMLOC: " ke baad ka data extract kar rahe hain.
    //
    // Example response format roughly:
    //
    // +CIPGSMLOC: 0,77.5946,12.9716,2026/09/26,10:30:00
    //
    // Yaha comma separated values ko baad mein alag karenge.


    int comma1 = data.indexOf(',');
    // First comma find.


    int comma2 = data.indexOf(',', comma1 + 1);
    // Second comma find.


    int comma3 = data.indexOf(',', comma2 + 1);
    // Third comma find.


    if (comma1 > 0 && comma2 > 0) 
    {
      
      lng = data.substring(comma1 + 1, comma2);
      // First aur second comma ke beech ka data longitude
      // ke roop mein extract kar rahe hain.


      lat = data.substring(comma2 + 1, comma3);
      // Second aur third comma ke beech ka data latitude
      // ke roop mein extract kar rahe hain.
    }
  }


  // ============================================================
  // LOCATION FAILED?
  // ============================================================

  if (lat == "0" || lng == "0")
  {
    
    sendSMS(phoneNumber, "Sorry, couldn't get location. Try again.");
    // Agar latitude ya longitude 0 hai,
    // sender ko location failed ka SMS bhej do.


    return;
    // Function yahin stop kar do.
    // Neeche Google Maps link create nahi hoga.
  }


  // ============================================================
  // GOOGLE MAPS LINK CREATE
  // ============================================================

  String mapsLink =
    "https://maps.google.com/?q=" + lat + "," + lng;


  // Latitude + Longitude ko Google Maps URL ke andar daal rahe hain.
  //
  // Example:
  //
  // https://maps.google.com/?q=26.8467,80.9462
  //
  // Is link ko mobile par open karne par location map par
  // show ho sakti hai.


  sendSMS(phoneNumber, "My Location: " + mapsLink);
  // Sender ke number par Google Maps location link
  // SMS ke through send kar rahe hain.
}


// ================================================================
// SMS SEND FUNCTION
// ================================================================

void sendSMS(String number, String message) 
{

  // --------------------------------------------------------------
  // SMS ko Text Mode mein ensure kar rahe hain.
  // --------------------------------------------------------------

  sim800.println("AT+CMGF=1");


  delay(1000);
  // SIM800L ko command process karne ka time.


  // --------------------------------------------------------------
  // Receiver number set kar rahe hain.
  // --------------------------------------------------------------

  sim800.print("AT+CMGS=\"");


  sim800.print(number);
  // Jis number par SMS bhejna hai, wo print.


  sim800.println("\"");
  // Closing quotation mark bhejkar AT+CMGS command complete.
  //
  // Example:
  //
  // AT+CMGS="+919876543210"


  delay(1000);
  // SIM800L ke SMS prompt ka wait.


  // --------------------------------------------------------------
  // Actual SMS message send kar rahe hain.
  // --------------------------------------------------------------

  sim800.print(message);


  delay(100);
  // Message ko serial buffer mein send hone ka short wait.


  // --------------------------------------------------------------
  // CTRL+Z character send.
  //
  // ASCII 26 = Ctrl+Z
  //
  // SIM800L ko batata hai:
  // "Message complete hai, ab SMS send karo."
  // --------------------------------------------------------------

  sim800.write(26);


  delay(5000);
  // SMS send hone ke liye 5 seconds wait.


  Serial.println("SMS sent to " + number);
  // Serial Monitor par confirmation message.
}
