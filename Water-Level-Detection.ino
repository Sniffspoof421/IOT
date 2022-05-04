#include <Ultrasonic.h>
#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Wire.h>
uint8_t pin_led = 22;

int distance;
float duration;
float heightTank=168.0;
float overflowHeight=168.0;
//int c = heightTank-distance;
//int val = 0;
float percTank;
float litrWater;



const char* PARAM_MESSAGE = "message";
const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";

// Instantiate trig and echo pins for ultrasonic sensors
Ultrasonic ultrasonic (D1, D2);
// Replace with your network credentials
const char* ssid = "JioFi_office";
const char* password = "8c6rsd0v3d";
#include "config.h" 
// Create AsyncWebServer object on port 80
ESP8266WebServer server(80);

/*void getDistance() 
{ 
    int a = digitalRead(pin_led);
    Serial.println("getDistance value");
    Serial.println(pin_led);
    int temp= a;
    String sensor_value = String(temp);
    

 }*/

 const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
   
  
</div>
      <h1>Sensor Value:<span id="ADCValue">0</span></h1><br>
      <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
   <script type="text/javascript">
      google.charts.load('current', {'packages':['gauge']});
      google.charts.setOnLoadCallback(drawChart);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     var x= document.getElementById("ADCValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "http://192.168.15.21/readADC", true);
  xhttp.send();
}
setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

      function drawChart() {

        var data = google.visualization.arrayToDataTable([
          ['Label', 'Value'],
          ['Top Tank',45 ],
          ['Bottom Tank', 55],
        ]);

        var options = {
          width: 800, height: 360,
          redFrom: 90, redTo: 100,
          yellowFrom:75, yellowTo: 90,
          minorTicks: 5
        };

        var chart = new google.visualization.Gauge(document.getElementById('chart_div'));

        chart.draw(data, options);

        setInterval(function() {
          data.setValue(0, 1,document.getElementById("ADCValue").innerHTML );
          chart.draw(data, options);
        }, 13000);
        setInterval(function() {
          data.setValue(1, 1,document.getElementById("ADCValue").innerHTML);
          chart.draw(data, options);
        }, 5000);
        setInterval(function() {
          data.setValue(2, 1, document.getElementById("ADCValue").innerHTML);
          chart.draw(data, options);
        }, 26000);
      }
    </script>
  </head>


  <body>
    <div id="chart_div" style="width: 400px; height: 120px;"></div>
  
  </body>
</html>

)=====";



void handleRoot() {
 //String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", MAIN_page); //Send web page
}
 
void handleADC() {
    
     int temp = percTank;
     Serial.print("Percentage Tank: ");
    percTank=((heightTank-distance)/overflowHeight)*100;
    String adcValue = String(temp);
    Serial.println("Sensor value");
    Serial.print(adcValue);
    server.send(200, "text/plane", adcValue);
 
 distance = ultrasonic.read();
  Serial.println("Ultrasonic Read");
  Serial.println(distance);
 // distance= d*0.034/2;
  percTank= ((heightTank-distance)/overflowHeight)*100;
  if (isnan(distance)) {
    Serial.println("Failed to read from HC-SR04 sensor!");
   
  }
  else {
    Serial.println("Tank%");
    Serial.println(percTank);
    //int a = analogRead(D2);
 
    delayMicroseconds(16);
    //return ("Perc Tank", String(percTank));
  }

  digitalWrite(D1, LOW); //set trigger signal low for 2us
  delayMicroseconds(20);
  digitalWrite(D1, HIGH);  // make trigger pin active high
  delayMicroseconds(10);  
   
 // val = analogRead(Echo_pin);// wait for 10 microseconds
  digitalWrite(D1, LOW);   // make trigger pin active low
  //Serial.println(val);
  /*Measure the Echo output signal duration or pulss width */
  duration = pulseIn(D2,HIGH); // save time duration value in "duration variable

 
  distance= duration*0.034/2; //Convert pulse duration into distance
  Serial.println((heightTank-distance)*29.76);
  

 /* if distance greater than 10cm, turn on LED 
    if ( distance == 160)
    {
    digitalWrite(trigger_pin, HIGH);
    }
    else 
    {
    digitalWrite(trigger_pin, LOW);
    } 

    */
    // print measured distance value on Arduino serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    delayMicroseconds (1000);
   
    Serial.print(((heightTank-distance)/overflowHeight)*100);
    Serial.println(" %");
    Serial.print("Litres Water: ");
    litrWater = (heightTank-distance)*29.76;
    Serial.println((heightTank-distance)*29.76);

 
  //Send ADC value only to client ajax request
}

void setup () {
  // Serial port for debugging purposes
  Serial.begin (9600);

  // Initialize SPIFFS
  if (! SPIFFS.begin ()) {
    Serial.println ("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for web page
  

  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}


void loop() {
 server.handleClient();

}

