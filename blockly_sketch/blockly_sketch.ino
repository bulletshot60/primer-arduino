#include <SPI.h>
#include <WiFi.h>
#include "aJSON.h"

char ssid[] = "GRV-Secure";      //  your network SSID (name) 
char pass[] = "M33@W1r3S3cur3";  // your network password
char server[] = "blockly-bot.herokuapp.com";
int status = WL_IDLE_STATUS;     // the Wifi radio's status
WiFiClient client;
bool maze_completed = false;

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
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:    
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
   
  // connection success, print to serial
  Serial.println("You're connected to the network");
}

//submit a request for the program
//returns a json string containing a boolean running and an array of arrays program
String get_program() {
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /getProgram HTTP/1.1");
    client.println("Host: blockly-bot.herokuapp.com");
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
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    client.println("GET /stopProgram HTTP/1.1");
    client.println("Host: blockly-bot.herokuapp.com");
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
  if (client.connect(server, 80)) {
    // Make a HTTP request:
    //make a string because we have to know the length of the data we are posting
    String data = "current_block=";
    data += block_id;
    client.println("POST /setCurrentBlock HTTP/1.1");
    client.println("Host: blockly-bot.herokuapp.com");
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
  while(client.connected() && !client.available()) delay(1); //wait for data
  
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
  
}

//turn the robot right 90 degrees
void turn_right() {
  
}

//move forward (note that this assumes we have nothing in front of us, IE call handle_is_facing_wall first)
void move_forward() {
  
}

//are we obstructed
bool handle_is_facing_wall() {
  return false;
}

//was the maze marked as finished
bool handle_is_completed() {
  return maze_completed; 
}

//are we facing a wall of a particular color
bool handle_is_facing_color(aJsonObject* argument1) {
  int color = aJson.getArrayItem(argument1, 0)->valueint;
  return false;  
}

bool handle_boolean(aJsonObject* program);

//handle and statements
bool handle_and(aJsonObject* argument1, aJsonObject* argument2) {
  return handle_boolean_command(argument1) && handle_boolean_command(argument2);
}

//handle or statements
bool handle_or(aJsonObject* argument1, aJsonObject* argument2) {
  return handle_boolean_command(argument1) || handle_boolean_command(argument2);  
}

//handle not statements
bool handle_not(aJsonObject* argument1) {
  return !handle_boolean_command(argument1);
}

//handle boolean conditions 
//these are very lisp-like (op (argument1) (argument2))
bool handle_boolean_command(aJsonObject* command_array) {
  int command = aJson.getArrayItem(command_array, 0)->valueint;
  Serial.println(command);
  switch(command) {
    case 8: // is_facing_wall
      return handle_is_facing_wall();
      break;
    case 9: // is_completed
      return handle_is_completed();
      break;
    case 10: // is_facing_color (01: red, 02: green)
      return handle_is_facing_color(aJson.getArrayItem(command_array, 1));
      break;
    case 11: // and
      return handle_and(aJson.getArrayItem(command_array, 1), aJson.getArrayItem(command_array, 2));
      break;
    case 12: // or
      return handle_or(aJson.getArrayItem(command_array, 1), aJson.getArrayItem(command_array, 2));
      break;
    case 13: // not
      return handle_not(aJson.getArrayItem(command_array, 1));
      break;
  }
}

//execute the program until we reach the end
void run_program(aJsonObject* program) {
  int current_command = 0, max_command = aJson.getArraySize(program), argument1 = 0;
  bool last_boolean_value = false;
  maze_completed = false; //mark the maze as not completed

  highlight_block(0); //mark that we are not yet running

  while(current_command < max_command) {
    aJsonObject* command_array = aJson.getArrayItem(program, current_command);
    int command = aJson.getArrayItem(command_array, 0)->valueint;
    Serial.println(command);
    switch(command) {
      case 1:  // label
        ++current_command;
        break;
      case 2:  // goto_if_true
        if(last_boolean_value) {
          argument1 = aJson.getArrayItem(command_array, 1)->valueint;
          for(int i = 0; i < max_command; ++i) {
             aJsonObject* temp = aJson.getArrayItem(program, i);
             if(aJson.getArrayItem(temp, 0)->valueint == 1) {
                 if(aJson.getArrayItem(temp, 1)->valueint == argument1) {
                   current_command = i; 
                 }
             }
          }
        }
        break;
      case 3:  // goto_if_false
        if(!last_boolean_value) {
          argument1 = aJson.getArrayItem(command_array, 1)->valueint;
          for(int i = 0; i < max_command; ++i) {
             aJsonObject* temp = aJson.getArrayItem(program, i);
             if(aJson.getArrayItem(temp, 0)->valueint == 1) {
                 if(aJson.getArrayItem(temp, 1)->valueint == argument1) {
                   current_command = i; 
                 }
             }
          }
        }
        break;
      case 4:  // goto
        argument1 = aJson.getArrayItem(command_array, 1)->valueint;
        for(int i = 0; i < max_command; ++i) {
           aJsonObject* temp = aJson.getArrayItem(program, i);
           if(aJson.getArrayItem(temp, 0)->valueint == 1) {
               if(aJson.getArrayItem(temp, 1)->valueint == argument1) {
                 current_command = i; 
               }
           }
        }
        break;
      case 5:  // turn (01: left, 02: right)
        argument1 = aJson.getArrayItem(command_array, 0)->valueint;
        if(argument1 == 1) {
          turn_left();
        } else {
          turn_right();
        }
        ++current_command;
        break;
      case 6:  // move_forward
        if(handle_is_facing_wall()) {
          return;
        } else {
          move_forward();
        }
        ++current_command;
        break;
      case 7:  // maze_completed
        maze_completed = true;
        ++current_command;
        break;
      case 8:  // is_facing_wall
      case 9:  // is_completed
      case 10: // is_facing_color (01: red, 02: green)
      case 11: // and
      case 12: // or
      case 13: // not
        last_boolean_value = handle_boolean_command(command_array);
        ++current_command;
        break;
      case 14: // highlight block
        argument1 = aJson.getArrayItem(command_array, 1)->valueint;
        String serverData = highlight_block(argument1);
        //note that .c_str() will not work in this case so we have to do this
        char* data = new char[serverData.length() + 1];
        for(int i = 0; i < serverData.length(); ++i) { data[i] = serverData[i]; }
        data[serverData.length()] = '\0';
        //end grunt code
        Serial.println(data);
        aJsonObject* root = aJson.parse(data);
        bool running = aJson.getObjectItem(root, "running")->valuebool;
        //note that you must clean up after yourself
        aJson.deleteItem(root);
        delete data;
        if(!running) { //server responded we are no longer running halt
          return;
        }
        ++current_command;
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
  aJsonObject* root = aJson.parse(data);
  aJsonObject* running = aJson.getObjectItem(root, "running");
  aJsonObject* program = aJson.getObjectItem(root, "program");
  if(running->valuebool) { //here we go!!
    run_program(program);
    stop_program();
  }
  //note that you must clean up after yourself
  aJson.deleteItem(root); //should also get running and program since they are children
  delete data;
  delay(10000);
}
