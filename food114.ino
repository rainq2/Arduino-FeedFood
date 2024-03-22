#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <DHT.h>

const char *ssid = "banana";       // Replace with your WiFi name
const char *password = "52695269"; // Replace with your WiFi password

Servo myservo;  // Create a Servo object to control the servo motor
int servoPin = 26;  // Replace with the pin connected to the servo motor

WebServer server(80);

#define DHTPIN 21  // Replace with the pin connected to DHT11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);
  dht.begin();  //初始化DHT
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  // Print WiFi IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", HTTP_GET, [](){
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    String html = "<h1>Banana</h1>";
    html += "<p>Temperature: " + String(temperature) + " C</p>";
    html += "<p>Humidity: " + String(humidity) + " %</p>";
    html += "<button onclick=\"rotateAndBack()\">feeding</button>";
    html += "<p id=\"currentAngle\"></p>";
    html += "<script>function rotateAndBack(){document.getElementById('currentAngle').innerText = 'running';var xhttp = new XMLHttpRequest();xhttp.open('GET', '/rotate?angle=90', true);xhttp.send();setTimeout(function(){document.getElementById('currentAngle').innerText = 'success';var xhttpBack = new XMLHttpRequest();xhttpBack.open('GET', '/rotate?angle=0', true);xhttpBack.send();}, 200);}</script>";
    
    server.send(200, "text/html", html);
  });

  server.on("/rotate", HTTP_GET, [](){
    String angleParam = server.arg("angle");
    int angle = angleParam.toInt();
    if (angle >= 0 && angle <= 180) {
      myservo.write(angle);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid angle");
    }
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
