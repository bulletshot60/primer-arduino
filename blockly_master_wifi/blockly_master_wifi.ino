#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define TURN_LEFT 100
#define TURN_RIGHT 101
#define MOVE_FORWARD 102
#define MOVE_BACKWARD 103
#define RED 104
#define GREEN 105
#define BLUE 106
#define BLACK 107
#define PURPLE 108
#define PINK 109
#define ORANGE 110
#define YELLOW 111

char ssid[] = "primer";      //  your network SSID (name) 
char pass[] = "e727ab1a5d83";  // your network password
char server[] = "primer-bot.herokuapp.com";
WiFiClient client;
bool waiting = false;
int my_color;

void attempt_wifi_connect() {
  Wire.begin(1);
  Wire.onReceive(receiveEvent);

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network:    
  WiFi.begin(ssid, pass);

  // wait 10 seconds for connection:
  delay(10000); 
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
   
  // connection success, print to serial
  Serial.println("You're connected to the network");
}

//submit a request for the program
//returns a json string containing a boolean running and an array of arrays program
String get_program() {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /getProgram HTTP/1.1");
    client.println("Host: primer-bot.herokuapp.com");
    client.println();
  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return read_response();
}

//submit a request for the serve to halt execution
//return a json string containing a boolean success
String stop_program() {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /stopProgram HTTP/1.1");
    client.println("Host: primer-bot.herokuapp.com");
    client.println();
  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return read_response();
}

//retrieve a command from the server
String get_command() {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /getCommand HTTP/1.1");
    client.println("Host: primer-bot.herokuapp.com");
    client.println();
  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return read_response();
}

//tell the server which block we are on so it can highlight the block for the user
//return a json string containing a boolean running
String highlight_block(int block_id) {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    //make a string because we have to know the length of the data we are posting
    String data = "current_block=";
    data += block_id;
    client.println("POST /setCurrentBlock HTTP/1.1");
    client.println("Host: primer-bot.herokuapp.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("");
    client.println(data);
    client.println();
  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return read_response();
}

//tell the server to switch the next command to a label id
String goto_label(int label_id) {
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) { 
    attempt_wifi_connect();
  }
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    //make a string because we have to know the length of the data we are posting
    String data = "goto=";
    data += label_id;
    client.println("POST /setCommand HTTP/1.1");
    client.println("Host: primer-bot.herokuapp.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("");
    client.println(data);
    client.println();
  } else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return read_response();
}

//read the json response from teh server ignoring headers
String read_response() {
  while(client.connected() && !client.available()) delay(10); //wait for data
  
  String server_response = "";
  bool reading = false;
  
  while (client.available() > 0) {
    char c = client.read();
    if(c == '{') { //we are finally past the headers so start reading
      reading = true; 
    }
    if(c == '\n') { //we have reached the end of the json, don't record the null terminator
      reading = false; 
    }
    if(reading) {
      server_response += c;
    }
  }
  client.stop();
  return server_response;
}

//turn the robot left 90 degrees
void turn_left() {
  Wire.beginTransmission(2);
  Wire.write(TURN_LEFT);
  Wire.endTransmission();
  waiting = true;
  while(waiting) { delay(1000); }
}

//turn the robot right 90 degrees
void turn_right() {
  Wire.beginTransmission(2);
  Wire.write(TURN_RIGHT);
  Wire.endTransmission(); 
  waiting = true;
  while(waiting) { delay(1000); }
}

void move_forward() {
  Wire.beginTransmission(2);
  Wire.write(MOVE_FORWARD);
  Wire.endTransmission();  
  waiting = true;
  while(waiting) { delay(1000); }
}

void move_backward() {
  Wire.beginTransmission(2);
  Wire.write(MOVE_BACKWARD);
  Wire.endTransmission();  
  waiting = true;
  while(waiting) { delay(1000); }
}

//are we facing a wall of a particular color
bool handle_is_on_color(int argument1) {
  int color = argument1;
  
  Wire.beginTransmission(3);
  Wire.write(1);
  Wire.endTransmission();
  waiting = true;
  while(waiting) { delay(1000); }
  switch(color) {
    case 1: return my_color == RED;
    case 2: return my_color == GREEN;
    case 3: return my_color == YELLOW;
    case 4: return my_color == PINK;
    case 5: return my_color == BLACK;
    case 6: return my_color == PURPLE;
    case 7: return my_color == BLUE;
    case 8: return my_color == ORANGE;
  }
  
  return false;  
}

//execute the program until we reach the end
void run_program() {
  bool last_boolean_value = false;
  Serial.println(highlight_block(0)); //mark that we are not yet running
  while(true) {
    String serverData = get_command();
    //note that .c_str() will not work in this case so we have to do this
    char* data = new char[serverData.length() + 1];
    for(int i = 0; i < serverData.length(); ++i) { data[i] = serverData[i]; }
    data[serverData.length()] = '\0';
    //end grunt code
    Serial.println(data);
    StaticJsonBuffer<50> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(data);
    bool running = root["running"];
    if(!running) {
      delete data;
      return; 
    }
    int command = root["command"][0];
    delete data;
    
    switch(command) {
      case 1: break; //no-op label
      case 2:  // goto_if_true
        if(last_boolean_value) {
          Serial.println(goto_label(root["command"][1]));
        }
        break;
      case 3:  // goto_if_false
        if(!last_boolean_value) {
          Serial.println(goto_label(root["command"][1]));
        }
        break;
      case 4:  // goto
        Serial.println(goto_label(root["command"][1]));
        break;
      case 5:  // turn (01: left, 02: right)
        if(root["command"][1] == 1) {
          turn_left();
        } else {
          turn_right();
        }
        break;
      case 6:  // move_forward
        move_forward();
        break;
      case 7:  // move_backward
        move_backward();
        break;
      case 10: // is_on_color
        last_boolean_value = handle_is_on_color(root["command"][1]);
        break;
      case 13: // not
        last_boolean_value = !last_boolean_value;
        break;
      case 14: // highlight block
        int argument = root["command"][1];
        String serverData2 = highlight_block(argument);
        //note that .c_str() will not work in this case so we have to do this
        char* data2 = new char[serverData2.length() + 1];
        for(int i = 0; i < serverData2.length(); ++i) { data2[i] = serverData2[i]; }
        data2[serverData2.length()] = '\0';
        //end grunt code
        Serial.println(data2);
        StaticJsonBuffer<50> jsonBuffer2;
        JsonObject& root2 = jsonBuffer2.parseObject(data2);
        bool running = root2["running"];
        delete data2;
        if(!running) { //server responded we are no longer running halt
          return;
        }
        break;
      }
      delay(1000); //give the server time to update after each command
    }

    return;
}

void loop() {
  String serverData = get_program();
  //note that .c_str() will not work in this case so we have to do this
  char* data = new char[serverData.length() + 1];
  for(int i = 0; i < serverData.length(); ++i) { data[i] = serverData[i]; }
  data[serverData.length()] = '\0';
  //end grunt code
  Serial.println(data);
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);
  bool running = root["running"];
  delete data;
  if(running) { //here we go!!
    run_program();
    Serial.println(stop_program());
  }
  //note that you must clean up after yourself
  delay(10000);
}

void receiveEvent(int howMany) {
  int x = Wire.read(); // receive byte as an integer
  my_color = x;
  waiting = false;
}
