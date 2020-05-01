#include <ESP8266WiFi.h> // Include the Wi-Fi library
#include <ESP8266WebServer.h>
#include <Adafruit_Fingerprint.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "cdac"; // The name of the Wi-Fi network that will be created
const char *password = ""; // The password required to connect to it, leave blank for an
const int buttonPin1=4;
int button_prev1=HIGH;

ESP8266WebServer server(80);
SoftwareSerial mySerial(13, 12);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id,deleteid,s;


String pname="";
byte C;
int a;

uint8_t search2(){
  uint8_t r;
  Serial.println("Scanning mode");
 while (r != FINGERPRINT_OK) {
    r = finger.getImage();
    Serial.println("1st error");
 }
  if (r != FINGERPRINT_OK){  
    Serial.println("2nd error");
  return 1 ;
  }

  r = finger.image2Tz();
  if (r != FINGERPRINT_OK) { 
    Serial.println("3rd error");
    return 1 ;
  }

  r = finger.fingerFastSearch();
  if (r != FINGERPRINT_OK){ 
  Serial.println("Not a registered finger!!");
   //server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <p><b>NOT A REGISTERED FINGER!!</b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
  //delay(3000);
  return -1;

  
  }
 
  String match="";
  int loc=100+(10*(finger.fingerID))+1;
  match=read_String(loc);  
   //found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); Serial.print(" of  "); 
  Serial.println(match);  
   String pname="";
   pname=match; 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);


  //server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <h3>WELCOME MR.</h3></P>" + match + "<form action=\"/ \" method=\"POST\"><p></br></br><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\" style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");

    HTTPClient http;

   
    http.begin("http://10.208.42.154/?checked=done");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
    String payload = http.getString();   //Get the request response payload
    Serial.print("payload : ");
    Serial.println(payload);//Print the response payload

    }
 
  http.end();  

delay(2000);
 
//check if above line works for redirect
return finger.fingerID;

}



void setup() {
Serial.begin(115200); // Start the Serial communication to send messages to the computer
delay(10);
Serial.println('\n');
WiFi.begin(ssid, password); // Connect to the network
Serial.print("Connecting to ");
Serial.print(ssid); Serial.println(" ...");
int i = 0;
while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
delay(1000);
Serial.print(++i); Serial.print(' ');
}

Serial.println('\n');
Serial.println("Connection established!");
Serial.print("IP address:\t");
Serial.println(WiFi.localIP());
 pinMode(buttonPin1,INPUT_PULLUP);
finger.begin(57600);
  
if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
} else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
 }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  //Serial.println("Waiting for valid finger...");

 EEPROM.begin(1024);
  
server.on("/",matched); 
server.on("/login",p1);
server.on("/scan",p2);
server.on("/entered",handleP);
server.on("/choice1",handlechoice1);
server.on("/registchoice",handleReg);
server.on("/manualID",handlemanualID);
server.on("/withName",regprocess);
server.on("/regagain",regagain);
server.on("/delete",del);
server.on("/delagain",delagain);
  
server.begin();
Serial.println("HTTP server started");
}

void loop() {
  for(a=0;a<51;a++){
   C=EEPROM.read(600+a);

   if( C!='Y' && C!='N') {
    EEPROM.write(a+600,'N'); 
    EEPROM.commit();
   }
  }
  int buttonState1;
  server.handleClient();

  buttonState1=digitalRead(buttonPin1);
   if (button_prev1==LOW && buttonState1==HIGH){
    search2();
    }

button_prev1= buttonState1;
  
  }


void delagain(){
server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>PLEASE ENTER THE ID YOU WANT TO DELETE: </b></p><form action=\"/delete\" method=\"POST\"><input type=\"text\" name=\"delbut\" ></form></center></body></html>");    
}

int del(){
   deleteid = server.arg("delbut").toInt();
   byte M;
   M=EEPROM.read(600+deleteid);
   delay(2000);
  if(M=='N'){
    server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center><p><b>NO RECORDS FOUND FOR THIS ID</b></p><p><b>PLEASE RE-ENTER THE ID YOU WANT TO DELETE:</b> </p><form action=\"/delete\" method=\"POST\"><input type=\"text\" name=\"delbut\" ></form></br></br><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
  }
  else if(M=='Y'){ 
    
    uint8_t q= -1;
  
  q = finger.deleteModel(deleteid);

  if (q == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  EEPROM.write(600+deleteid,'N');
  EEPROM.commit();
  int dadd,b;
  dadd=100+(10*deleteid)+1;
  for(b=0;b<10;b++){
  EEPROM.write(dadd+b,'\0');
  EEPROM.commit();
  }
  server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>RECORDS FOR THIS HAS BEEN DELETED </b></p><p><b>DO YOU WANT TO DELETE AGAIN?</b></p><form action=\"/delagain\" method=\"POST\"><input type=\"submit\" style=\"font: bold 14px Arial;\" value=\"YES\"></form></br> <form action=\"/ \" method=\"POST\"><input type=\"submit\"style=\"font: bold 14px Arial;\"  value=\"NO \"></form></center></body></html>");
  } else if (q == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
      EEPROM.write(600+deleteid,'N');
  EEPROM.commit();
  int dadd,b;
  dadd=100+(10*deleteid)+1;
  for(b=0;b<10;b++){
  EEPROM.write(dadd+b,'\0');
  EEPROM.commit();
  }
  server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>RECORDS FOR THIS HAS BEEN DELETED </b></p><p><b>DO YOU WANT TO DELETE AGAIN?</b></p><form action=\"/delagain\" method=\"POST\"><input type=\"submit\" style=\"font: bold 14px Arial;\" value=\"YES\"></form></br> <form action=\"/ \" method=\"POST\"><input type=\"submit\"style=\"font: bold 14px Arial;\"  value=\"NO \"></form></center></body></html>");  
       //return q ;
  } else if (q == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return q ;
  } else if (q == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return q ;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(q, HEX);
    return q ;
  }   
    
    
    }
  
}

void regagain(){
server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center><h1>|||----------REGISTRATION PHASE----------|||</h1></br><p><b>PRESS 1 FOR AUTOMATIC ID GENERATION or ELSE PRESS 2 FOR MANUAL ENTERING</b></p><form action=\"/registchoice\" method=\"POST\"><input type=\"text\" name=\"regchoice\" > </form></center></body></html>");
  }


int regprocess(){
            int p = -1;
   Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) { 
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
       break; 
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
     server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>ERROR WHILE REGISTERING YOUR FINGERPRINT </b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\" style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>ERROR WHILE REGISTERING YOUR FINGERPRINT </b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
    delay(2000);
    return p;
  } else {
    Serial.println("Unknown error");
    server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>ERROR WHILE REGISTERING YOUR FINGERPRINT </b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    EEPROM.write(600+id,'Y');
    EEPROM.commit();
    //Serial.println("Array updated:Value stored at "); Serial.print(id); Serial.print(" "); Serial.print(EEPROM.read(id));
    int addr;
    addr=100+(10*id)+1;
    writeString(addr,server.arg("uname"));
    server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center><h3>YOUR REGISTRATION PROCESS IS NOW COMPLETED!!</h3><p></br></br><b>DO YOU WANT TO REGISTER MORE?</b></p><form action=\"/regagain\" method=\"POST\"><input type=\"submit\" style=\"font: bold 14px Arial;\"  value=\"YES\"></form></br> <form action=\"/ \" method=\"POST\"><input type=\"submit\" style=\"font: bold 14px Arial;\"  value=\"NO \"></form></center></body></html>");

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>ERROR WHILE REGISTERING YOUR FINGERPRINT </b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
    return p;
  }
  
  
  }


void handlemanualID(){
    
      byte K;      
      id =server.arg("uid").toInt();
          
      K=EEPROM.read(600+id);
      if(K=='Y'){
       server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>ALREADY REGISTERED WITH THIS ID: </b></p><form action=\"/manualID\" method=\"POST\"><p><b>PLEASE ENTER A NEW ID:</b></p><input type=\"text\" name=\"uid\" placeholder=\"ID here..\"></form></center></body></html> ");
      }
    else if(K!='Y'){
      server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><form action=\"/withName\" method=\"POST\"><p><b>ENTER THE NAME FOR THE ID:</b></p><input type=\"text\" name=\"uname\" placeholder=\"Name here..\"></form><p><b>REGISTRATION PROCESS IS IN PROGRESS</b></p> <p><b>PLEASE PLACE YOUR FINGER WHEN LIGHT BECOMES STABLE</b> </p> <p><b>YOU WILL HAVE TO PLACE YOUR FINGER TWICE</b></p></form></center></body></html> ");
      //We replaced withName with enrollPhase
      }
}

void handleReg(){
  
  //For automatic ID generation
  if(server.arg("regchoice") == "1"){
        int z;
      for(z=1;z<51;z++){
       if((EEPROM.read(600+z))!='Y'){
        id=z;
        break;
       }
      }
      // We replaced withName with enrollPhase
      server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <p><b>YOU ARE REGISTERING FOR ID: </b></p>" + String(id) + "<form action=\"/withName\" method=\"POST\"><p><b>ENTER THE NAME FOR THE ID:</b></p><input type=\"text\" name=\"uname\" placeholder=\"Name here..\"></form><p> <b>REGISTRATION PROCESS IS IN PROGRESS</b></p> <p><b>PLEASE PLACE YOUR FINGER WHEN LIGHT BECOMES STABLE</b> </p> <p><b>YOU WILL HAVE TO PLACE YOUR FINGER TWICE</b></p></form></center></body></html> ");
        }
        //For explicit ID generation
     else  if(server.arg("regchoice") == "2"){

        server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><h3>PLEASE ENTER THE ID:</h3><form action=\"/manualID\" method=\"POST\"><input type=\"text\" name=\"uid\" placeholder=\"ID here..\"></form></center></body></html>");          
      
      }
     
     
     }



void handlechoice1(){
  if(server.arg("choice") == "1") { // If both the username and the password are correct
    server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center><h1>|||----------REGISTRATION PHASE----------|||</h1></br><p><b>PRESS 1 FOR AUTOMATIC ID GENERATION or ELSE PRESS 2 FOR MANUAL ENTERING</b></p><form action=\"/registchoice\" method=\"POST\"><input type=\"text\" name=\"regchoice\" ></center></body></html>");
  }
else if(server.arg("choice") == "2") { // If both the username and the password are correct
    server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center><p><b>PLEASE ENTER THE ID YOU WANT TO DELETE:</b> </p><form action=\"/delete\" method=\"POST\"><input type=\"text\" name=\"delbut\" ></form></center></body></html>");
  }

  else if(server.arg("choice") == "3") {
   String rec="";
   int sl=0;
   for(a=1;a<51;a++){
   C=EEPROM.read(600+a);
    if(C=='Y'){
       sl++;
       String matchn="";
       int locn=100+(10*a)+1;
       matchn=read_String(locn); 
       rec=rec+String(sl)+"."+ matchn+ " ID:"+String(a)+"</br>";
      }
    }
server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center><p><b>THE REGISTERED USERS ARE : </b></p>" + rec +"<form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");   
  }   
  }




void handleP(){
    
  if(server.arg("password") == "123456") { // If both the username and the password are correct
    server.send(200, "text/html", hp());
  } 
  //If Wrong Password redirect to Home page
 else if(server.arg("password") != "123456"){
  server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <p><b>INVALID PASSWORD</b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\" style=\"font: bold 14px Arial;\"  value=\"Redirect\"></form></center></body></html>");
  }
  }
  
void matched() { 
server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"></br></br></br></br><center><b>PRESS THE REQUIRED BUTTON</b></br></br><form action=\"/login\" method=\"POST\"><input type=\"submit\" align=\"centre\" style=\"font: bold 14px Arial;\"  value=\"ADMIN MODE\"></form></center></br></br><center> <form action=\"/scan\" method=\"POST\"><input type=\"submit\"  align=\"middle\" style=\"font: bold 14px Arial;\"  value=\"USER MODE\"></form></center></body></html>");
}

void p1(){
server.send(200, "text/html", "<html><body bgcolor=\"#E6E6FA\"><center> <p><b>PLEASE ENTER THE PASSWORD TO CONTINUE: </b></p></br><form action=\"/entered\" method=\"POST\"><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" style=\"font: bold 14px Arial;\"  value=\"Login\"></form></center></body></html>");
}
  

void p2(){
search();
}

String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

 void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
    EEPROM.commit();
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}


uint8_t search(){
  uint8_t r;
  Serial.println("Scanning mode");
 while (r != FINGERPRINT_OK) {
    r = finger.getImage();
    Serial.println("1st error");
 }
  if (r != FINGERPRINT_OK){  
    Serial.println("2nd error");
  return 1 ;
  }

  r = finger.image2Tz();
  if (r != FINGERPRINT_OK) { 
    Serial.println("3rd error");
    return 1 ;
  }

  r = finger.fingerFastSearch();
  if (r != FINGERPRINT_OK){ 
  Serial.println("Not a registered finger!!");
   server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <p><b>NOT A REGISTERED FINGER!!</b></p><form action=\"/ \" method=\"POST\"><p><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\"  style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");
  delay(3000);
  return -1;

  
  }
 
  String match="";
  int loc=100+(10*(finger.fingerID))+1;
  match=read_String(loc);  
   //found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); Serial.print(" of  "); 
  Serial.println(match);  
   String pname="";
   pname=match; 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);


  server.send(200,"text/html","<html><body bgcolor=\"#E6E6FA\"><center> <h3>WELCOME MR.</h3></P>" + match + "<form action=\"/ \" method=\"POST\"><p></br></br><b>PRESS BUTTON BELOW TO REDIRECT TO HOME PAGE</b></p><input type=\"submit\" style=\"font: bold 14px Arial;\" value=\"Redirect\"></form></center></body></html>");

    HTTPClient http;

    //Below put the IP of the NodeMCU with lock
    http.begin("http://10.208.42.154/?checked=done");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
    String payload = http.getString();   //Get the request response payload
    Serial.print("payload : ");
    Serial.println(payload);//Print the response payload

    }
 
  http.end();  

delay(4000);
 
//check if above line works for redirect
//return finger.fingerID;

}





String hp(){
  String ptr = "<!DOCTYPE html>\n";
  ptr +="<html>\n";
  ptr +="<head>\n";
  ptr +="<title>Choice page</title>\n";
  ptr +="</head>\n";
  ptr +="<body bgcolor=\"#E6E6FA\">\n";
  ptr +="<center>\n";
  ptr +="<h1>|||----------ADMIN MODE----------|||</h1>\n";
  ptr +="<p> <b>1.REGISTER</b></p>\n";
  ptr +="<p><b>2.DELETE</b></p>\n";
  ptr +="<p><b>3.VIEW THE REGISTERED IDS</b></p>\n";
  ptr +="<p><b>PLEASE ENTER YOUR CHOICE IN THE BOX BELOW</b></p>\n";
  ptr +="<form action=\"/choice1\" method=\"POST\">\n";
  ptr +="<input type=\"text\" name=\"choice\" >\n";
  ptr +="</form>\n";
   ptr +="</center>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
