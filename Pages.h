// const char index_html[] PROGMEM = R"rawliteral(
// <!DOCTYPE HTML>
// <html>

// <head>
//   <title>Email Notification with Temperature</title>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
// </head>

// <body>
//   <h2>ESP32 Health Band</h2>
//   <h3>%TEMPERATURE% &deg;C</h3>
//   <h2>ESP Email Notification</h2>
//   <form action="/get">
//     Email Address <input type="email" name="email_input" value="%EMAIL_INPUT%" required><br>
//     Enable Email Notification <input type="checkbox" name="enable_email_input" value="true" %ENABLE_EMAIL%><br>
//     Temperature Threshold <input type="number" step="0.1" name="threshold_input" value="%THRESHOLD%" required><br>
//     <input type="submit" value="Submit">
//   </form>
// </body>

// </html>)rawliteral";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
    integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
      font-family: Arial;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
    }
    h2 {
      font-size: 3.0rem;
    }
    p {
      font-size: 3.0rem;
    }
    .units {
      font-size: 1.2rem;
    }
    .dht-labels {
      font-size: 1.5rem;
      vertical-align: middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 Health Band</h2>
  <p>
    <i class="fas fa-solid fa-notes-medical" style="color: #ff0000;"></i>
    <span class="dht-labels">Spo2</span>
    <span id="oxy">97%</span>
  </p>
  <p>
    <i class="fas fa-solid fa-heart" style="color: #e62d2d;"></i>
    <span class="dht-labels">BPM</span>
    <span id="bpm">73</span>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
    <span class="dht-labels">Temperature</span>
    <span id="temperature">20</span>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i>
    <span class="dht-labels">Humidity</span>
    <span id="humidity">27</span>
    <sup class="units">%</sup>
  </p>

  <h2>Visualization</h2>
  <button onclick="window.location.href='https://thingspeak.com/channels/2382108/feed.csv'">Download CSV Sheet</button>
  <div id="chart_div" style="width: 100%; height: 500px;">
    <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2382108/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
  </div>
  <div id="chart2_div" style="width: 100%; height: 500px;">
    <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2382108/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
  </div>
  <div id="chart3_div" style="width: 100%; height: 500px;">
    <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2382108/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>   
  </div>
  <div id="chart4_div" style="width: 100%; height: 500px;">
    <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2382108/charts/4?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
  </div>
</body>
<script>
  setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        console.log(this.responseText);
        var obj = JSON.parse(this.responseText);
        document.getElementById("oxy").innerHTML = ""+obj.oxy;
        //document.getElementById("bpm").innerHTML = ""+obj.bpm;
        document.getElementById("temperature").innerHTML = ""+obj.temperature;
        document.getElementById("humidity").innerHTML = ""+obj.humidity;
      }
    };
    xhttp.open("GET", "/get", true);
    xhttp.send();
  }, 3000);
</script>
</html>
)rawliteral";