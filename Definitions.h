// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "vivo Y33s";
const char *password = "24681013";

float tempReq = 23;
float bpmReq = 71;
int oxyReq = 97;
int humidityReq = 31;

// To send Emails using Gmail on port 465 (SSL)
#define emailSenderAccount "mernatarek228@gmail.com"
#define emailSenderPassword "xrqs foow xnye winb"
#define smtpServer "smtp.gmail.com"
#define smtpServerPort 465
#define emailSubject "[ALERT] ESP32 Health Band"

double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
int i = 0;
int Num = 100; // calculate SpO2 by this sampling interval
int Temperature;
int temp;
float ESpO2;            // initial value of estimated SpO2
double FSpO2 = 0.7;     // filter factor for estimated SpO2
double frate = 0.95;    // low pass filter for IR/red LED value to eliminate AC component
#define TIMETOBOOT 3000 // wait for this time(msec) to output SpO2
#define SCALE 88.0      // adjust to display heart beat and SpO2 in the same scale
#define SAMPLING 100    // 25 //5     // if you want to see heart beat more precisely, set SAMPLING to 1
#define FINGER_ON 30000 // if red signal is lower than this, it indicates your finger is not on the sensor
#define USEFIFO

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

// Default Recipient Email Address
String inputMessage = "mo.samy2613@gmail.com";
String enableEmailChecked = "checked";
String inputMessage2 = "true";
// Default Threshold Temperature Value
String inputMessage3 = "25.0";
String lastTemperature;
String lastHumidity;

// HTML web page to handle 3 input fields (email_input, enable_email_input, threshold_input)

// Flag variable to keep track if email notification was sent or not
bool emailSent = false;
bool Temperature_emailSent = false;
bool Humidity_emailSent = false;

bool Oxygen_emailSent = false;
bool HeartBeats_emailSent = false;

const char *PARAM_INPUT_1 = "email_input";
const char *PARAM_INPUT_2 = "enable_email_input";
const char *PARAM_INPUT_3 = "threshold_input";

// Interval between sensor readings. Learn more about timers: https://R...content-available-to-author-only...s.com/esp32-pir-motion-sensor-interrupts-timers/
unsigned long previousMillis = 0;
const long interval = 1000;