#include <SoftwareSerial.h>
 
#define DEBUG true
#define FAN_PIN 7
#define RED_PIN 9
#define BLUE_PIN 10
#define GREEN_PIN 11
#define FANSPEED 1023

#define BLUE_LIGHT_DIM 1  //turn on the blue light
#define BLUE_LIGHT_BRIGHT 6 //turn on the blue light brightly
#define GREEN_LIGHT 2  //turn on the green light
#define PURPLE_LIGHT 3  //turn on the purple light
#define FAN 4         //turn on the fan
#define TURN_OFF 5    //turn off all the things

#define DELAY_LIGHT 1000 // delay for turn the light brightly
 
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3
                             
long rgb[3];
long rgbval;
float hue=0, saturation=1, value=1;
/*
chosen LED SparkFun sku: COM-09264rgbval
 has Max Luminosity (RGB): (2800, 6500, 1200)mcd
 so we normalize them all to 1200 mcd -
 R  250/600  =  107/256
 G  250/950  =   67/256
 B  250/250  =  256/256
 */
long bright[3] = { 107, 67, 256};



void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200); // your esp's baud rate might be different
 
  pinMode(FAN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  //analogWrite(FANPIN_1, LOW); // By changing values from 0 to 255 you can control motor speed
  //analogWrite(FANPIN_2, LOW); // By changing values from 0 to 255 you can control motor speed
  //analogWrite(FANPIN_3, LOW); // By changing values from 0 to 255 you can control motor speed

  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 1234
  
  Serial.println("first arduino start!");
}
 
void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {
    Serial.println("Someone come in.....");

    if(esp8266.find("+IPD,"))
    {    
      const int headSize = 4;
      int currentPosition = 0;
      int modeId;
      while(esp8266.available())
      {
        if(currentPosition == headSize)
        {
          modeId = esp8266.read() - 48;
          break;        
        }  
        char c = esp8266.read();
        Serial.print(c);               
        currentPosition++;
      }

      Serial.println(modeId);
      
          
      switch (modeId) {

        case BLUE_LIGHT_DIM:
          //start the blue light very darkly
          Serial.println("turn on the blue light dimly");
          hue = 0.2;
          rgbval = HSV_to_RGB(hue, saturation, value);
          rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
          rgb[1] = (rgbval & 0x0000FF00) >> 8;
          rgb[2] = rgbval & 0x000000FF;
          analogWrite(BLUE_PIN, rgb[1] * bright[1]/256);          
          break;
          
        case BLUE_LIGHT_BRIGHT:
          //turn on the light brightly
          Serial.println("turn on the blue light brightly");
          hue = 1.0;
          rgbval = HSV_to_RGB(hue, saturation, value);
          rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
          rgb[1] = (rgbval & 0x0000FF00) >> 8;
          rgb[2] = rgbval & 0x000000FF;
          analogWrite(BLUE_PIN, rgb[1] * bright[1]/256);   
          break;

        case GREEN_LIGHT:
          //start the green light
          Serial.println("turn on the green light");
          hue = 0.5;
          rgbval = HSV_to_RGB(hue, saturation, value);
          rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
          rgb[1] = (rgbval & 0x0000FF00) >> 8;
          rgb[2] = rgbval & 0x000000FF;
          analogWrite(GREEN_PIN, rgb[2] * bright[2]/256);          
          break;
          
        case PURPLE_LIGHT:
          //start the light purple
          Serial.println("turn on the purple light");
          analogWrite(FAN_PIN, FANSPEED);
          //start the green light
          hue = 0.5;
          rgbval = HSV_to_RGB(hue, saturation, value);
          rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
          rgb[1] = (rgbval & 0x0000FF00) >> 8;
          rgb[2] = rgbval & 0x000000FF;
          analogWrite(RED_PIN, rgb[0] * bright[0]/256); 
          analogWrite(BLUE_PIN, rgb[1] * bright[1]/256);         
          break;

        case FAN:
          //start fan
          Serial.println("start the fan");
          analogWrite(FAN_PIN, FANSPEED);
          break;

        case TURN_OFF:
          //turn off everything
          Serial.println("turn off everything");
          //turn off the fan
          analogWrite(FAN_PIN, LOW);
          analogWrite(BLUE_PIN, LOW);
          analogWrite(GREEN_PIN, LOW);
          analogWrite(RED_PIN, LOW);
          break;
      }


     // delay(1000);
 
      // int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     
      // String webpage = "<h1>Hello</h1>&lth2>World!</h2><button>LED1</button>";
//
//      Serial.println(webpage);
// 
//      String cipSend = "AT+CIPSEND=";
//      cipSend += connectionId;
//      cipSend += ",";
//      cipSend +=webpage.length();
//      cipSend +="\r\n";
//     
//      sendData(cipSend,1000,DEBUG);
//      sendData(webpage,1000,DEBUG);
     
     // webpage="<button>LED2</button>";
     
     // cipSend = "AT+CIPSEND=";
     // cipSend += connectionId;
     // cipSend += ",";
     // cipSend +=webpage.length();
     // cipSend +="\r\n";
     
     // sendData(cipSend,1000,DEBUG);
     // sendData(webpage,1000,DEBUG);
 
     // String closeCommand = "AT+CIPCLOSE="; 
     // closeCommand+=connectionId; // append connection id
     // closeCommand+="\r\n";
     
     // sendData(closeCommand,3000,DEBUG);
    }
  }
}
 
 
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

long HSV_to_RGB( float h, float s, float v ) {
  /* modified from Alvy Ray Smith's site: http://www.alvyray.com/Papers/hsv2rgb.htm */
  // H is given on [0, 6]. S and V are given on [0, 1].
  // RGB is returned as a 24-bit long #rrggbb
  int i;
  float m, n, f;

  // not very elegant way of dealing with out of range: return black
  if ((s<0.0) || (s>1.0) || (v<1.0) || (v>1.0)) {
    return 0L;
  }

  if ((h < 0.0) || (h > 6.0)) {
    return long( v * 255 ) + long( v * 255 ) * 256 + long( v * 255 ) * 65536;
  }
  i = floor(h);
  f = h - i;
  if ( !(i&1) ) {
    f = 1 - f; // if i is even
  }
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i) {
  case 6:
  case 0: 
    return long(v * 255 ) * 65536 + long( n * 255 ) * 256 + long( m * 255);
  case 1: 
    return long(n * 255 ) * 65536 + long( v * 255 ) * 256 + long( m * 255);
  case 2: 
    return long(m * 255 ) * 65536 + long( v * 255 ) * 256 + long( n * 255);
  case 3: 
    return long(m * 255 ) * 65536 + long( n * 255 ) * 256 + long( v * 255);
  case 4: 
    return long(n * 255 ) * 65536 + long( m * 255 ) * 256 + long( v * 255);
  case 5: 
    return long(v * 255 ) * 65536 + long( m * 255 ) * 256 + long( n * 255);
  }
}
