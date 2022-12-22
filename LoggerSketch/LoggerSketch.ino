// Sketch by ZeroTwoWeeb
// Some parts are from library examples
// Device to log data from Ultrasonic-Sensor onto SD-Card with Timestamp by RTC
// The Data can also be shown by a 16x2 LCD Display via button press (interrupt)



#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "LiquidCrystal.h"



#define PIN_TRIGGER 16
#define PIN_ECHO 17
#define BACKLIGHT_PIN 12
#define LCDBUTTON_PIN 35
#define uS_TO_S_FACTOR 1000000
#define SECONDS_TO_SLEEP 10
#define Sample_Nr 5 //from how many values the average should be taken(Ultrasonic-Sensor)
//#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 2000


RTC_DS3231 rtc;

//LCD Pins
LiquidCrystal lcd(32, 33, 25, 26, 27, 14);

//Supersonic-Sensor Var's
RTC_DATA_ATTR const int SENSOR_MAX_RANGE = 300; // in cm
RTC_DATA_ATTR unsigned long duration;
RTC_DATA_ATTR unsigned int distance;


//count Var's
RTC_DATA_ATTR int i = 0; //Var to show how many Datapoints are currently beeing stored(Session only)
RTC_DATA_ATTR boolean lcdStatus = false;
RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    Serial.begin(115200);
    log_d("Serial Begin...");



    attachInterrupt(LCDBUTTON_PIN, lcdinterrupt, RISING);

    pinMode(PIN_TRIGGER, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(BACKLIGHT_PIN, OUTPUT );
    pinMode(LCDBUTTON_PIN, INPUT);

    //start LCD
    lcd.begin(16, 2);
    lcd.print("startup...");

    delay(3000); // wait for console opening

    if (!rtc.begin())
    {
        log_e("Couldn't find RTC");
        while (1)
            ;
    }

    if (rtc.lostPower())
    {
        log_e("RTC lost power, lets set the time!");
        // following line sets the RTC to the date &amp; time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date &amp; time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    if (!SD.begin())
    {
        log_e("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        log_e("No SD card attached");
        return;
    }

    log_d("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        log_d("MMC");
    }
    else if (cardType == CARD_SD)
    {
        log_d("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        log_d("SDHC");
    }
    else
    {
        log_d("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    Serial.printf("Free space: %lluMB\n", (SD.totalBytes() / (1024 * 1024))- (SD.usedBytes() / (1024 * 1024)));


    if (!SD.exists("/data")) {
      createDir(SD, "/data");
    }
    if (!SD.exists("/logs")) {
      createDir(SD, "/logs");
    }
    if(!SD.exists("/data/data.txt")){
      writeFile(SD, "/data/data.txt", "\n"); //Overwrites old data.txt files!!!
    }
    if(!SD.exists("/logs/log.txt")){
      writeFile(SD, "/logs/log.txt", "\n");  //Overwrites old log.txt files!!!
    }

    appendFile(SD, "/data/data.txt", "---\n");
    getWakeUpReason();
}

void loop(){
    DateTime now = rtc.now();
    Serial.println(now.timestamp());

    appendFile(SD, "/data/data.txt", String(now.timestamp()+", "+String(i)+", "+getDistance()+"\n").c_str());
    Serial.print(getDistance()+"\n");
    setLCD(String(String(now.month())+"/"+String(now.day())+" "+String(now.hour())+":"+String(now.minute())), getDistance());
    i++;
    delay(1000);
    if (lcdStatus==false) {
      gotoSleep();
    }
    
}

boolean listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        log_e("Failed to open directory");
        return false;
    }
    if(!root.isDirectory()){
        log_e("Not a directory");
        return false;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            return true;
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
            return true;
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
        appendFile(SD, "/logs/log.txt", String("[TASK][SD] created Directory: "+String(path)+"\n").c_str());
    }
    else
    {
        Serial.println("mkdir failed");
        appendFile(SD, "/logs/log.txt", String("[ERR][SD] mkdir failed: "+String(path)+"\n").c_str());
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
        appendFile(SD, "/logs/log.txt", String("[ERR][SD] Failed Writing to: "+String(path)+"\n").c_str());
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
        appendFile(SD, "/logs/log.txt", String("[TASK][SD] Written new File: "+String(path)+"\n").c_str());
    }
    else
    {
        Serial.println("Write failed");
        appendFile(SD, "/logs/log.txt", String("[TASK][SD] Write Failed to: "+String(path)+"\n").c_str());
    }
    file.close();
}

String getDistance(){
    int duration;
    float distance[Sample_Nr];
    // change to US-100 take 5-10 values and return average (maybe check for far of values)
    for (int counter = 0; counter < Sample_Nr; counter++) {
    digitalWrite(PIN_TRIGGER, HIGH);
    delayMicroseconds(500);
    digitalWrite(PIN_TRIGGER, LOW);
    duration = pulseIn(PIN_ECHO, HIGH);
    distance[counter] = (duration / 2.0) / 2.91;
    }
    long sum = 0L ;  // sum will be larger than an item, long for safety.
    for (int counter = 0 ; counter < Sample_Nr ; counter++)
      sum += distance[counter] ;
    return  String(String(((float) sum) / Sample_Nr)+"mm").c_str();  // average will be fractional, so float may be appropriate.
    
}

void setLCD(String Line1, String Line2){
  if(lcdStatus==true){
    DateTime now = rtc.now();
    lcd.clear();
    //set Line1
    lcd.setCursor (0, 0);
    lcd.print(Line1);
    log_d("wrote LCD line 1");
    //set Line2
    lcd. setCursor (0, 1);
    lcd.print(Line2);
    log_d("wrote LCD line 2");

    if(Line1.length()<16){
      appendFile(SD, "/logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 1 of LCD Module (ERR STRING < 16)\n").c_str()); 
    }
    if(Line2.length()<16){
      appendFile(SD, "/logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 2 of LCD Module (ERR STRING < 16)\n").c_str()); 
    }
  }
}  
void setLCD(String Line1){
  if(lcdStatus==true){
    DateTime now = rtc.now();
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print(Line1);
    log_d("wrote LCD line 1");

    if(!Line1.length()<16){
      appendFile(SD, "/logs/log.txt", String(now.timestamp()+"[ERR] [SD] Error while Setting Line 1 of LCD Module (ERR STRING < 16)\n").c_str()); 
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
    appendFile(SD, "/logs/log.txt", String("[STATUS][LCD] lcd turned on by interrupt\n").c_str());
    lcdOn();
  }else{
    appendFile(SD, "/logs/log.txt", String("[STATUS][LCD] lcd turned off by interrupt\n").c_str());
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print("LCD: OFF...");
    lcdOff();
  }
}


void gotoSleep() {

  log_d("Going to sleep");
  
  // Allow wake up if Button Switch is pressed
  esp_sleep_enable_ext0_wakeup((gpio_num_t)LCDBUTTON_PIN, 1);  // could use: GPIO_NUM_26

  // Allow RTC to wake up this device after X seconds
  esp_sleep_enable_timer_wakeup(SECONDS_TO_SLEEP * uS_TO_S_FACTOR);

  // Sleep starts
  esp_deep_sleep_start();
}


void getWakeUpReason() {
  log_w("This is (re)boot #%d", ++bootCount);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      log_w("Wakeup caused by external signal using RTC_IO");
      lcdOn();
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      log_w("Wakeup caused by external signal using RTC_CNTL");
      lcdOn();
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      log_w("Wakeup caused by RTC timer");
      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      log_w("Wakeup caused by touchpad");
      break;
    case ESP_SLEEP_WAKEUP_ULP:
      log_w("Wakeup caused by ULP program");
      break;
    default:
      log_w("Wakeup was not caused by deep sleep: %d", wakeup_reason);
  }
}

