// Sketch by ZeroTwoWeeb
// Some parts are from library examples
// Device to log data from Ultrasonic-Sensor onto SD-Card with Timestamp by RTC
// The Data can also be shown by a 16x2 LCD Display via button press (interrupt)

//to-do: 
//-detect voltage Drop(Voltage Sensor)
//-show SD Usage on Screen
//-Button to swap LCD Menu
//-Code efficency/energy efficency
//-Detect spikes/wrong values from Ultrasonic-Sensor
//-document detection and creation whithout overwriting old Data
//-add Error Log
//-write important events to log (interrupts, Errors, Storage usage, Voltage, Suspect values)


#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "LiquidCrystal.h"


#define PIN_TRIGGER 16
#define PIN_ECHO 17
#define BACKLIGHT_PIN 12
#define LCDBUTTON_PIN 35


RTC_DS3231 rtc;

//LCD Pins
LiquidCrystal lcd(32, 33, 25, 26, 27, 14);

//Supersonic-Sensor Var's
const int SENSOR_MAX_RANGE = 300; // in cm
unsigned long duration;
unsigned int distance;


//count Var's
int i = 0;
boolean lcdStatus = false;

void setup()
{
 
    Serial.begin(115200);

    attachInterrupt(LCDBUTTON_PIN, lcdinterrupt, RISING);

    pinMode(PIN_TRIGGER, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode ( BACKLIGHT_PIN, OUTPUT );
    pinMode(LCDBUTTON_PIN, INPUT);

    //start LCD
    lcd.begin(16, 2);
    lcd.print("startup...");

    delay(3000); // wait for console opening

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        while (1)
            ;
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power, lets set the time!");
        // following line sets the RTC to the date &amp; time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date &amp; time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    listDir(SD, "/data", 0);
    createDir(SD, "/logs");
    createDir(SD, "/data");
    listDir(SD, "/data", 0);
    writeFile(SD, "/data/data.txt", "\n");  //Overwrites old data.txt files!!! need to create Number generation! and check before?(count up from old file nr)
    //teste ob datei existiert und wenn nicht dann erstellung...
}

void loop(){
    DateTime now = rtc.now();
    Serial.println(now.timestamp());

    appendFile(SD, "/data/data.txt", String(now.timestamp()+", "+String(i)+", "+getDistance()+"\n").c_str());
    Serial.println(getDistance());
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    setLCD(String(String(now.month())+"/"+String(now.day())+" "+String(now.hour())+":"+String(now.minute())), getDistance());
    delay(1000);
    i++;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
//void readFile(fs::FS &fs, const char * path){
//    Serial.printf("Reading file: %s\n", path);
//
//    File file = fs.open(path);
//    if(!file){
//        Serial.println("Failed to open file for reading");
//        return;
//    }
//
//    Serial.print("Read from file: ");
//    while(file.available()){
//        Serial.write(file.read());
//    }
//    file.close();
//}

void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

String getDistance(){
    int duration;
    float distance;
    digitalWrite(PIN_TRIGGER, HIGH);
    delayMicroseconds(500);
    digitalWrite(PIN_TRIGGER, LOW);
    duration = pulseIn(PIN_ECHO, HIGH);
    distance = (duration / 2.0) / 2.91;
    
    return String(String(distance)+String(" mm")).c_str();
}

void setLCD(String Line1, String Line2){
  if(lcdStatus==true){
    DateTime now = rtc.now();
    lcd.clear();
    if(Line1.length()<16){
      Serial.print(Line1.length(), DEC);
      lcd.setCursor (0, 0);
      lcd.print(Line1);
    }else{
      //error log soon
      appendFile(SD, "logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 1 of LCD Module (ERR STRING < 16)").c_str()); 
    }
    if(Line2.length()<16){
      lcd. setCursor (0, 1);
      lcd.print(Line2);
    }else{
      //error log soon
      appendFile(SD, "logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 2 of LCD Module (ERR STRING < 16)").c_str()); 
    }
  }
}  
void setLCD(String Line1){
  if(lcdStatus==true){
    DateTime now = rtc.now();
    if(!Line1.length()<16){
      lcd.clear();
      lcd.setCursor (0, 0);
      lcd.print(Line1);
    }else{
      //error log soon
      appendFile(SD, "logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 1 of LCD Module (ERR STRING < 16)").c_str()); 
    }
  }  
}

void lcdOn(){
  digitalWrite(BACKLIGHT_PIN, HIGH);
  lcdStatus=true;
}
void lcdOff(){
  digitalWrite(BACKLIGHT_PIN, LOW);
  lcdStatus=false;
}

void lcdinterrupt(){
  //if lcd is off turn it on else of
  if(lcdStatus==false){
    appendFile(SD, "logs/log.txt", String("[STATUS][LCD] lcd turned on by interrupt").c_str());
    lcdOn();
  }else{
    appendFile(SD, "logs/log.txt", String("[STATUS][LCD] lcd turned off by interrupt").c_str());
    lcdOff();
  }
}
