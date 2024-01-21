#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ESP32_MailClient.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
// #include "MAX30105.h" //sparkfun MAX3010X library
// #include "heartRate.h"
#include <EMailSender.h>
#include "Pages.h"
#include "Definitions.h"
#include "MAX30102_PulseOximeter.h"
#include <HTTPClient.h>



/*void printBPM(irValue,beatsPerMinute,beatAvg){
  //delay(500);

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
}*/
#define DHTPIN 18     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
// MAX30105 particleSensor;
PulseOximeter pox;
EMailSender emailSend("mernatarek228@gmail.com", "xrqs foow xnye winb");
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

String readDHTTemperature()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else
  {
    Serial.println(t);
    return String(t);
  }
}
// Replaces placeholder with DS18B20 values
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "TEMPERATURE")
  {
    return lastTemperature;
  }
  else if (var == "EMAIL_INPUT")
  {
    return inputMessage;
  }
  else if (var == "ENABLE_EMAIL")
  {
    return enableEmailChecked;
  }
  else if (var == "THRESHOLD")
  {
    return inputMessage3;
  }
  return String();
}

// The Email Sending data object contains config and data to send
SMTPData smtpData;

void initialize(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  // Start the DHT11 sensor
  dht.begin();
  delay(2000);
  if (!pox.begin()) {
      Serial.println("POX FAILED");
      for (;;)
          ;
  } else {
      Serial.println("POX SUCCESS");
  }

  pox.setIRLedCurrent(MAX30102_LED_CURR_20_8MA);
}

String getRequest(){
  String jsonString = "{";
  jsonString += "\"oxy\": " + String(oxyReq) + ", ";
  jsonString += "\"bpm\": " + String(bpmReq) + ", ";
  jsonString += "\"humidity\": " + String(humidityReq) + ", ";
  jsonString += "\"temperature\": " + String(tempReq);
  jsonString += "}";  
  return jsonString;
}

void setup()
{
  initialize();
  // byte ledBrightness = 0x7F; // Options: 0=Off to 255=50mA
  // byte sampleAverage = 4;    // Options: 1, 2, 4, 8, 16, 32
  // byte ledMode = 2;          // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  // // Options: 1 = IR only, 2 = Red + IR on MH-ET LIVE MAX30102 board
  // int sampleRate = 200; // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  // int pulseWidth = 411; // Options: 69, 118, 215, 411
  // int adcRange = 16384; // Options: 2048, 4096, 8192, 16384
  // particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings
  // particleSensor.enableDIETEMPRDY();
  
  #pragma region server
  // Home
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  // Long polling endpoint
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", getRequest()); });

  // server.on("/submit", HTTP_GET, [] (AsyncWebServerRequest *request) {
  //     if (request->hasParam(PARAM_INPUT_1)) {
  //       inputMessage = request->getParam(PARAM_INPUT_1)->value();
  //       if (request->hasParam(PARAM_INPUT_2)) {
  //         inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
  //         enableEmailChecked = "checked";
  //       }
  //       else {
  //         inputMessage2 = "false";
  //         enableEmailChecked = "";
  //       }
  //       if (request->hasParam(PARAM_INPUT_3)) {
  //         inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
  //       }
  //     }
  //     else {
  //       inputMessage = "No message sent";
  //     }
  //     Serial.println(inputMessage);
  //     Serial.println(inputMessage2);
  //     Serial.println(inputMessage3);
  //     request->send(200, "text/html", "HTTP GET request sent to your ESP.<br><a href=\"/\">Return to Home Page</a>");
  // });
  server.onNotFound(notFound);
  server.begin();

  #pragma endregion server
}

void loop()
{
  pox.update();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    lastTemperature = String(getTemprature());
    Serial.println("Temperature: " + lastTemperature + " C");

    lastHumidity = String(getHumidity());
    Serial.println("Humidity: " + lastHumidity + " %");
    
    float temperature = lastTemperature.toFloat();
    float humidity = lastHumidity.toFloat();
    
    float Spo2 = oxygen_level();
    Serial.print("Oxygen level: ");
    Serial.print(Spo2);
    Serial.println("%");
    
    float bpm = heart_rate();
    Serial.print("Heart rate: ");
    Serial.print(bpm);
    Serial.println(" b/m");
    writeToApi();
    // Check if temperature is above threshold and if it needs to send the Email alert
    #pragma region email_temp
    // if (temperature > inputMessage3.toFloat() && inputMessage2 == "true" && !emailSent)
    // {
    //   String emailMessage = String("Temperature above threshold. Current temperature: ") +
    //                         String(temperature) + String("C");
    //   if (sendEmailNotification(emailMessage))
    //   {
    //     Serial.println(emailMessage);
    //     emailSent = true;
    //   }
    //   else
    //   {
    //     Serial.println("Email failed to send");
    //   }
    // }
    #pragma endregion email_temp
    
    #pragma region email_temp_high
    // // Check if temperature is below threshold and if it needs to send the Email alert
    // else if ((temperature < inputMessage3.toFloat()) && inputMessage2 == "true" && emailSent)
    // {
    //   String emailMessage = String("Temperature below threshold. Current temperature: ") +
    //                         String(temperature) + String(" C");
    //   EMailSender ::EMailMessage message;
    //   message.subject = "Temperature Status";
    //   message.message = emailMessage;
    //   EMailSender ::Response response = emailSend.send("mirna.tarik@gmail.com", message);
    //   Serial.println("responce status: ");
    //   Serial.println(response.status);
    //   Serial.println(response.code);

    //   Serial.println(response.desc);
    //   /*if(sendEmailNotification(emailMessage)) {
    //     Serial.println(emailMessage);
    //     emailSent = false;
    //   }
    //   else {
    //     Serial.println("Email failed to send");
    //   }*/
    // }
    #pragma endregion email_temp_high

    #pragma region email_oxy
    // Check if oxygen level is below threshold and if it needs to send the Email alert
    if (Spo2 >70 && Spo2 < 96 && inputMessage2 == "true" && !Oxygen_emailSent)
    {
      String emailMessage = String("Oxygen level below threshold. Current oxygen level: ") +
                            String(Spo2) + String("%");
      if (sendEmailNotification(emailMessage))
      {
        Serial.println(emailMessage);
        Oxygen_emailSent = true;
      }
      else
      {
        Serial.println("Email failed to send");
      }
    }
    #pragma endregion email_oxy

    #pragma region email_bpm_high
    // Check if bpm is above threshold and if it needs to send the Email alert
    if ((bpm > 90 || bpm < 50) && bpm>30 && inputMessage2 == "true" && !HeartBeats_emailSent)
    {
      String emailMessage = String("Heart rate out of threshold. Current heart rate: ") +
                            String(bpm) + String(" b/m");
      if (sendEmailNotification(emailMessage))
      {
        Serial.println(emailMessage);
        HeartBeats_emailSent = true;
      }
      else
      {
        Serial.println("Email failed to send");
      }
    }
    #pragma endregion email_bpm_high
  
  }

}

bool sendEmailNotification(String emailMessage)
{
  // Set the SMTP Server Email host, port, account and password
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);

  // For library version 1.2.0 and later which STARTTLS protocol was supported,the STARTTLS will be
  // enabled automatically when port 587 was used, or enable it manually using setSTARTTLS function.
  // smtpData.setSTARTTLS(true);

  // Set the sender name and Email
  smtpData.setSender("ESP32", emailSenderAccount);

  // Set Email priority or importance High, Normal, Low or 1 to 5 (1 is highest)
  smtpData.setPriority("High");

  // Set the subject
  smtpData.setSubject(emailSubject);

  // Set the message with HTML format
  smtpData.setMessage(emailMessage, true);

  // Add recipients
  smtpData.addRecipient(inputMessage);

  smtpData.setSendCallback(sendCallback);

  // Start sending Email, can be set callback function to track the status
  if (!MailClient.sendMail(smtpData))
  {
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
    return false;
  }
  // Clear all data from Email object to free memory
  smtpData.empty();
  return true;
}

// Callback function to get the Email sending status
void sendCallback(SendStatus msg)
{
  Serial.println(msg.info());

  if (msg.success())
  {
    Serial.println("----------------");
  }
}

float getTemprature(){
  float t = dht.readTemperature();
  tempReq = t;
  if (isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return 0;
  }
  
  tempReq =t;
  return t;
}

float getHumidity(){
  float h = dht.readHumidity();
  humidityReq = h;
  if (isnan(h))
  {
    Serial.println("Failed to read from DHT sensor!");
    return 0;
  }
  humidityReq = h;
  return h;
}

float heart_rate(){
  float bpm = pox.getHeartRate();
  bpmReq = bpm;
  if (bpm < 0) {
    Serial.println("No Heart Rate detected");
    return 0;
  }
  bpmReq = bpm;
  return bpm;


}

float oxygen_level(){
  float spo2 = pox.getSpO2();
  if (spo2 < 0) {
    Serial.println("No SpO2 detected");
    return 0;
  }
  oxyReq = spo2;
  return spo2;
}

void writeToApi(){
  String url = "http://api.thingspeak.com/update?api_key=GDGXKXV8SMTK11X8&field1=" + String(tempReq) + "&field2=" + String(humidityReq) + "&field3=" + String(bpmReq) + "&field4=" + String(oxyReq);
  if (WiFi.status() == WL_CONNECTED && bpmReq > 40) { 
    HTTPClient http;
    http.begin(url.c_str());
    int httpCode = http.GET();
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println("Updated "+ payload);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }
    Serial.println(httpCode);
    http.end();
    Serial.println("Updated");
} else {
    Serial.println("Not connected to Wi-Fi");
}
}