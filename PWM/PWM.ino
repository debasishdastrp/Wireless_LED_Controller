#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid="ClicksAndBits_IOT"; //SSID for Wifi Access Point
const char* pass="12345678"; //Password for Wifi Access Point

int pwm_dc, red, green, blue;
String colr;

//IP Configurations for Access Point
IPAddress local_IP(192,168,2,1);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);  //Web Server Configuration

//RGB LED pin Configuration
uint8_t LED1_R=D5;
uint8_t LED1_G=D6;
uint8_t LED1_B=D7;
bool LED1_R_status=LOW;
bool LED1_G_status=LOW;
bool LED1_B_status=LOW;

//Dimming LED Configuration
uint8_t LED2_G=D4;
bool LED2_G_status=LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED1_R,OUTPUT);
  pinMode(LED1_G,OUTPUT);
  pinMode(LED1_B,OUTPUT);

  pinMode(LED2_G,OUTPUT);
  
  // WiFi Access Point Initiation
  WiFi.softAP(ssid,pass);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  delay(100);
  // HTTP request Handler Configuration
  server.on("/", handle_OnConnect);
  server.on("/pwm",handle_pwm);
  server.on("/rgb",handle_rgb);
  server.begin();

  analogWriteRange(255);

  pwm_dc=254;
  colr = "#000000";

  analogWrite(LED2_G,pwm_dc);
  delay(50);

  red=0;
  green=0;
  blue=0;
  analogWrite(LED1_R,red);
  analogWrite(LED1_G,green);
  analogWrite(LED1_B,blue);
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() //Function for Landing or Home Page
{
  analogWrite(LED2_G,500);
  server.send(200,"text/html",HTMLgen(pwm_dc,colr,red,green,blue));
}

void handle_pwm() //Function For Handling Dimming LED
{
  String a="";
  Serial.println(server.args());
  a +=server.argName(0);
  a +=" : ";
  a +=server.arg(0);
  Serial.println(a);
  String pwm = server.arg(0);
  pwm_dc= pwm.toInt();
  server.send(200,"text/html",HTMLgen(pwm_dc,colr,red,green,blue));
  Serial.println(pwm_dc);
}


void handle_rgb() // Function For Handling RGB LED
{
  String a="";
  Serial.println(server.args());
  a +=server.argName(0);
  a +=" : ";
  a +=server.arg(1);
  Serial.println(a);
  String rgb = server.arg(1);
  colr=server.arg(0);
 
  int commaCounter=0;
  String scolr="";
  for(int i=0;i<=rgb.length()-1;i++)
  {
    if(rgb[i]==',')
    {
      
      if(commaCounter==0)
      {
        red=scolr.toInt();
        Serial.println(red);
        scolr="";
        commaCounter++;
      }
      else if(commaCounter==1)
      {
        green=scolr.toInt();
        Serial.println(green);
        scolr="";
      }
    }
    else
    {
      scolr +=rgb[i];
    }
  }
  blue=scolr.toInt();
  Serial.println(blue);

  server.send(200,"text/html",HTMLgen(pwm_dc,colr,red,green,blue));
  
}


String HTMLgen(int pwm, String colr, int r, int g, int b) //Function for generating HTML page content accodinng to Request
{
  
  String s="";
  s +="<html>\n";
    s +="<head>\n";
        s +="<title>Clicks and Bits</title>\n";
        s +="<meta charset=\"UTF-8\">\n";
        s +="<meta name=\"viewport\" content=\"width=device-width, user-scalable=no, initial-scale=1.0\">";
        s +="<style>";
            s +="body{";
                s +="font-family: calibri;";
                s +="text-align: center;";
                s +="color: #009999;";
            s +="}";
        s +="</style>";
        s +="<script type=\"text/javascript\">";
            s +="function hex2rgb(hex) {";
  s +="document.getElementById('rgb').value= ['0x' + hex[1] + hex[2] | 0, '0x' + hex[3] + hex[4] | 0, '0x' + hex[5] + hex[6] | 0];";
  s +="document.getElementById('colr').submit();";
s +="}";
        s +="</script>";
    s +="</head>";
    s +="<body>";
    s +="<div style=\"padding:5px; background-color: #00cccc; color: white; border-radius: 10px; margin: 5px\"><h2>Wireless LED Controller</h2></div>";
        s +="<form action=\"/pwm\" method=\"POST\" id=\"pwm\" name=\"pwm\">";
            s +="<h3>Brightness Control</h3>";
            s +="<input type=\"range\" min=\"0\" max=\"254\" id=\"slider\" name=\"slider\" value=\""+String(pwm)+"\" onchange=\"javascript:pwm.submit()\">";
            analogWrite(LED2_G,pwm);
            delay(50);
        s +="</form>";
        s +="<hr style=\"height:1px;border-width:0;background-color:#eeeeee\">";
        s +="<form action=\"/rgb\" method=\"POST\" id=\"colr\" name=\"colr\">";
            s +="<h3>Color Control</h3>";
            s +="<input type=\"color\" id=\"colBtn\" name=\"colBtn\" value=\""+colr+"\" onchange=\"javascript:hex2rgb(document.getElementById('colBtn').value)\">";
            s +="<input type=\"hidden\" name=\"rgb\" id=\"rgb\" value=\"0\">";
            analogWrite(LED1_R,red);
            analogWrite(LED1_G,green);
            analogWrite(LED1_B,blue);
            delay(50);
        s +="</form>";
        s +="<div style=\"padding-top: 80px; color:#aaaaaa\">by<br><b>Clicks and Bits</b></div>";
    s +="</body>";
s +="</html> ";

return s;
}
