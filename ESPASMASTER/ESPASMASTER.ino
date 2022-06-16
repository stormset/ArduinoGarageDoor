#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <Streaming.h>
#include <Wire.h>
int CLOSEDLimit = 14;
int OPENEDLimit = 12;
int GarageEnd = 16;
int GarageStart = 13;
int numBytes = 100;
String letters[42] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "#", "_"};
String randString1;
String randString2;
String indexa = "etg_dji#oijsidsfaztaopl65ssa";
String indexb = "#_djsksp545s##sjiossopsodk_s";
String service_psw = "1953";
String psw;
int currentpsw;
String LightTime;
int currentLightTime;
String LightTimeout;
int currentLightTimeout;
int currentSPS;
int currentSPSLight;
int currentInLight;
int currentOutLight;
String AutoClose;
int currentAutoClose;
String XML;
unsigned long wait000 = 0UL, wait001 = 1000UL;
ESP8266WebServer server(80);

bool is_authentified_1() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf(indexa) != -1) {
      return true;
    }
  }
  return false;
}
bool is_authentified_2() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
    if (cookie.indexOf(indexb) != -1) {
      return true;
    }
  }
  return false;
}


void setup(void) {
  Serial.begin(115200);
  Wire.begin(D1, D2);
  pinMode(CLOSEDLimit, INPUT);
  pinMode(OPENEDLimit, INPUT);
  pinMode(GarageEnd, OUTPUT);
  pinMode(GarageStart, INPUT);
  WiFiManager wifiManager;
  wifiManager.autoConnect("SmartGarage");
  EEPROM.begin(32);
  server.on("/user", handleUsers);
  server.on("/", handleLogin);
  server.on("/settings", handleSettings);
  server.on("/xml", handleXML);
  server.on("/dynamicoc", DynamicOChandle);
  server.on("/inline", []() {
    server.send(200, "text/plain", "Segítségre van szükségesd?");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
}


void handleLogin() {
  if (server.hasArg("DISCONNECT")) {
    if (server.hasArg("NEWPASSWORD")) {
      EEPROM.begin(512);

      EEPROM.put(0, server.arg("NEWPASSWORD").toInt());
      EEPROM.commit();

      EEPROM.put(5, server.arg("lightTime").toInt());
      EEPROM.commit();

      EEPROM.put(10,  server.arg("lightTimeout").toInt());
      EEPROM.commit();

      EEPROM.put(15, server.arg("SPS").toInt());
      EEPROM.commit();

      EEPROM.put(23, server.arg("SPSLight").toInt());
      EEPROM.commit();

      EEPROM.put(30, server.arg("InsideLightEvent").toInt());
      EEPROM.commit();

      EEPROM.put(40, server.arg("OutsideLightEvent").toInt());
      EEPROM.commit();

      EEPROM.put(50, server.arg("CloseTime").toInt());
      EEPROM.commit();

      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      return;
    }
    return;
  }
  if (server.hasArg("DISCONNECTuser")) {
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  if (is_authentified_1()) {
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ";
    header += indexa;
    header += "\r\nLocation: /user\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);

  }
  if (is_authentified_2()) {
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ";
    header += indexb;
    header += "\r\nLocation: /settings\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);

  }

  String msg;
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");
  }


  if (server.hasArg("PASSWORD")) {
    EEPROM.get(0, currentpsw);
    psw = String(currentpsw);
    if (server.arg("PASSWORD") == psw) {
      numGenerator1();
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ";
      header += indexa;
      header += "\r\nLocation: /user\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      return;
    }
    if (server.arg("PASSWORD") == service_psw) {
      numGenerator2();
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ";
      header += indexb;
      header += "\r\nLocation: /settings\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      return;
    }

  }


  String content = "<!DOCTYPE HTML>\n";
  content += "<meta name='viewport' charset=\"UTF-8\" content='width=device-width'><html><head> <link rel=\"stylesheet\"href=\"https://cdnjs.cloudflare.com/ajax/libs/normalize/5.0.0/normalize.min.css\"> <style> @import url(\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600,700\"); body { background: #252455; background: -webkit-linear-gradient(left, #443b58, #252455); background: -o-linear-gradient(right, #443b58, #252455); background: -moz-linear-gradient(right, #443b58, #252455); background: linear-gradient(to right, #443b58, #252455); background-size: 100%; background-position: 0 0; background-attachment: fixed; padding: 10px; height: 100vh; color: #fff; font-family: \"Open Sans\", sans-serif; box-sizing: border-box; overflow: hidden; } input::-webkit-input-placeholder { color: #8bc4ea !important; } input:-moz-placeholder { color: #8bc4ea !important; } input::-moz-placeholder { color: #8bc4ea !important; } input:-ms-input-placeholder { color: #8bc4ea !important; } .container { width: 100%; height: 100vh; margin: 0 auto; display: -webkit-box; display: -ms-flexbox; display: flex; -webkit-box-orient: vertical; -webkit-box-direction: normal; -ms-flex-direction: column; flex-direction: column; -webkit-box-align: center; -ms-flex-align: center; align-items: center; } .container p { font-size: 24px; font-weight: 100; } .container h1 { font-size: 32px; font-weight: 600; } .inputs input { width: 30px; height: 60px; line-height: 100%; background-color: transparent; border: 0; outline: 0; color: white; font-size: 46px; word-spacing: 0px; overflow: hidden; text-align: center; } @import url(https://fonts.googleapis.com/css?family=Open+Sans:300); html { margin: 0; padding: 0; font-family: 'Open Sans', sans serif; font-weight: 200; -webkit-tap-highlight-color: transparent; } html, body { margin: 0; padding: 0; font-family: 'Open Sans', sans serif; font-weight: 200; -webkit-tap-highlight-color: transparent; } .keypadwrapper { text-align: center; width: 100%; color: #FFFFFF; .inputwrapper { margin: 2em; line-height: 0.61em; vertical-align: middle; } .numberinput { display: inline-block; height: 15px; width: 15px; border-radius: 50px; border: 1px solid #FFFFFF; margin-right: 2%; margin-left: 2%; font-size: 2em; } .keypad { .numberline { width: 100%; } #linefour { position: absolute; bottom: 10%; } #linethree { position: absolute; bottom: 30%; } #linetwo { position: absolute; bottom: 50%; } #lineone { position: absolute; bottom: 70%; } .content { display: inline-block; margin: 0 8%; div { width: 4em; height: 4em; text-align: center; border: 1px solid #FFFFFF; border-radius: 70px; display: inline-block; cursor: pointer; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; span:nth-child(1) { display: block; font-size: 1.8em; height: 1em; margin-top: 0.2em; } span:nth-child(2) { font-size: 0.6em; } } div:hover { background-color: #ffffff; color: #000000; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; } } } } .nocircle { width: auto !important; border: none !important; height: auto !important; } @media all and (max-width: 470px) { .content { margin: 0 5% !important; } } @media all and (max-width: 350px) { .content { margin: 0 2% !important; } } @media all and (max-height: 470px) { #linefour { bottom: 2% !important; } #linethree { bottom: 23% !important; } #linetwo { bottom: 43% !important; } #lineone { bottom: 63% !important; } } @import url(https://fonts.googleapis.com/css?family=Open+Sans:300); html, body { margin: 0; padding: 0; font-family: 'Open Sans', sans serif; font-weight: 200; -webkit-tap-highlight-color: transparent; } } .keypadwrapper { text-align: center; width: 100%; color: #FFFFFF; } .keypadwrapper .inputwrapper { margin: 2em; line-height: 0.61em; vertical-align: middle; } .keypadwrapper .numberinput { display: inline-block; height: 15px; width: 15px; border-radius: 50px; border: 1px solid #FFFFFF; margin-right: 2%; margin-left: 2%; font-size: 2em; } .keypadwrapper .keypad .numberline { width: 100%; } .keypadwrapper .keypad #linefour { position: absolute; bottom: 10%; } .keypadwrapper .keypad #linethree { position: absolute; bottom: 30%; } .keypadwrapper .keypad #linetwo { position: absolute; bottom: 50%; } .keypadwrapper .keypad #lineone { position: absolute; bottom: 70%; } .keypadwrapper .keypad .content { display: inline-block; margin: 0 8%; } .keypadwrapper .keypad .content div { width: 4em; height: 4em; text-align: center; border: 1px solid #FFFFFF; border-radius: 70px; display: inline-block; cursor: pointer; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; } .keypadwrapper .keypad .content div span:nth-child(1) { display: block; font-size: 1.8em; height: 1em; margin-top: 0.2em; } .keypadwrapper .keypad .content div span:nth-child(2) { font-size: 0.6em; } .keypadwrapper .keypad .content div:hover { background-color: #ffffff; color: #000000; -webkit-touch-callout: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; } .nocircle { width: auto !important; border: none !important; height: auto !important; } @media all and (max-width: 470px) { .content { margin: 0 5% !important; } } @media all and (max-width: 350px) { .content { margin: 0 2% !important; } } @media all and (max-height: 470px) { #linefour { bottom: 2% !important; } #linethree { bottom: 23% !important; } #linetwo { bottom: 43% !important; } #lineone { bottom: 63% !important; } } </style> </head>";
  content += "<body>\n";
  content += " <img id='status' src='' style='object-fit: cover;max-width: 102%'ondblclick='OC()'>";
  content += "<form action='/' method='POST'  name=\"inputpassword\"><input id='codeForLGN' type='hidden' name='PASSWORD'></form><div class=\"container\"></form> <div class=\"inputs\" id=\"inputs\"> <input id=\"1\" maxlength=\"2\" placeholder=\"•\" value=\"\" disabled/> <input id=\"2\" maxlength=\"2\" placeholder=\"•\" value=\"\" disabled/> <input id=\"3\" maxlength=\"2\" placeholder=\"•\" value=\"\" disabled/> <input id=\"4\" maxlength=\"1\" placeholder=\"•\" value=\"\" disabled/> </div> </div>   <div class=\"keypadwrapper\"> <div class=\"keypad\"> <div id=\"lineone\" class=\"numberline\"> <div class=\"content\"> <div onclick=\"input(document.getElementById('oneInp'))\"> <span class=\"number\" id='oneInp'>1</span> <span>___</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('twoInp'))\"> <span class=\"number\" id='twoInp'>2</span> <span>ABC</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('threeInp'))\"> <span class=\"number\" id='threeInp'>3</span> <span>DEF</span> </div> </div> </div> <div id=\"linetwo\" class=\"numberline\"> <div class=\"content\"> <div onclick=\"input(document.getElementById('fourInp'))\"> <span class=\"number\" id='fourInp'>4</span> <span>GHI</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('fiveInp'))\"> <span class=\"number\" id='fiveInp'>5</span> <span>JKL</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('sixInp'))\"> <span class=\"number\" id='sixInp'>6</span> <span>MNO</span> </div> </div> </div> <div id=\"linethree\" class=\"numberline\"> <div class=\"content\"> <div onclick=\"input(document.getElementById('sevenInp'))\"> <span class=\"number\" id='sevenInp'>7</span> <span>PQRS</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('eightInp'))\"> <span class=\"number\" id='eightInp'>8</span> <span>TUV</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('nineInp'))\"> <span class=\"number\" id='nineInp'>9</span> <span>WXYZ</span> </div> </div> </div> <div id=\"linefour\" class=\"numberline\"> <div class=\"content\"> <div onclick=deleteBtn();> <span class=\"number\"><</span> <span>___</span> </div> </div> <div class=\"content\"> <div onclick=\"input(document.getElementById('nullInp'))\"> <span class=\"number\" id='nullInp'>0</span> <span>___</span> </div> </div> <div class=\"content\"> <div> <span class=\"number\">*</span> <span>___</span> </div> </div> </div> </div> </div> <script> function input(e) { var tbInput = document.getElementById('codeForLGN'); tbInput.value = tbInput.value + e.innerHTML;  var focusDot = ((tbInput.value).length); document.getElementById(focusDot).value = e.innerHTML; if((tbInput.value).length == 4){document.inputpassword.submit();} } function deleteBtn(){document.getElementById('codeForLGN').value ='';document.getElementById('1').value =''; document.getElementById('2').value =''; document.getElementById('3').value =''; document.getElementById('4').value =''; } </script> ";
  content += "</body>\n";
  content += "</html>\n";
  server.send(200, "text/html", content);

}
void numGenerator1() {
  randString1 = "";
  for (int i = 0; i < numBytes; i++)
  {
    randString1 = randString1 + letters[random(0, 42)];
  }
  indexa = "ESPSESSIONID=";
  indexa += randString1;
}

void numGenerator2() {
  randString2 = "";
  for (int i = 0; i < numBytes; i++)
  {
    randString2 = randString2 + letters[random(0, 42)];
  }
  indexb = "ESPSESSIONID=";
  indexb += randString2;
}

void handleSettings() {
  if (!is_authentified_1() && !is_authentified_2) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  if (is_authentified_2()) {
    EEPROM.begin(512);
    EEPROM.get(5, currentLightTime);
    LightTime = String(currentLightTime);
    EEPROM.get(10, currentLightTimeout);
    LightTimeout = String(currentLightTimeout);
    EEPROM.get(15, currentSPS);
    EEPROM.get(23, currentSPSLight);
    EEPROM.get(30, currentInLight);
    EEPROM.get(40, currentOutLight);
    EEPROM.get(50, currentAutoClose);

    String content = "<html> <meta name=\"viewport\" charset=\"UTF-8\" > <style> body { background: #252455; background: -webkit-linear-gradient(left, #443b58, #252455); background: -o-linear-gradient(right, #443b58, #252455); background: -moz-linear-gradient(right, #443b58, #252455); background: linear-gradient(to right, #443b58, #252455); background-size: 100%; background-position: 0 0; background-attachment: fixed; padding: 10px; height: 100vh; color: #fff; font-family: \"Open Sans\", sans-serif; box-sizing: border-box; overflow: hidden; } .slidecontainer { width: 100%; } .slider { -webkit-appearance: none; width: 33%; height: 15px; border-radius: 8px; background: rgb(171,157,219); outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; } .slider:hover { opacity: 1; } .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 20px; height: 20px; border-radius: 50%; border: 2px solid white;background: rgb(171,157,219); cursor: pointer; } .slider::-moz-range-thumb { width: 60px; height: 25px; border-radius: 50%; background: #FFFFFF; cursor: pointer; } .switch1 input {display:none;} .switch1 { position: relative; top: 4px; display: inline-block;width: 50px;height: 20px;}.sliderbutton {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;bottom: 0;background-color:rgb(138,126,179);-webkit-transition: .4s;transition: .4s;}.sliderbutton:before {position: absolute;content: \"\";height: 20px;width: 20px;background-color: white;-webkit-transition: .4s;transition: .4s;}input:checked + .sliderbutton {background-color: #8E5AB4;}input:focus + .sliderbutton {box-shadow: 0 0 1px #8E5AB4;}input:checked + .sliderbutton:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(30px);}.sliderbutton.round {border-radius: 32px;}.sliderbutton.round:before {border-radius: 50%;}</style>  ";
    content += "<body onload=\"checkboxes();loadSelection()\"><script src='https://simeydotme.github.io/jQuery-ui-Slider-Pips/dist/js/jquery-plus-ui.min.js'></script><p style=\"font-size:36px; text-align:center;font-weight: bold; color:white;\">Beállítások</p><p style=\"font-size:16px; font-weight: bold; color:white;\">Alapbeállítások</p><div style=\"margin: 20px;\"><p style=\"color:white\">Jelszó:   </p><form action='/?DISCONNECT=YES' method='POST' id='settings'> <input type=\"text\" name=\"NEWPASSWORD\" autocomplete=\"off\"  required title=\"A kód nem kezdődhet 0-val, továbbá 4 jegyűnek kell lennie\" maxlength=\"4\" minlength=\"4\" inputmode=\"numeric\" size=\"6\" pattern=\"^[1-9]\\d*$\" style=\"-webkit-border-radius: 50px; -moz-border-radius: 50px; border-radius: 50px; border-color:white;height: 35px; font-size: 23px; background-color:rgba(0, 0, 0, 0); color:white; text-align: center;\"><br><br><h style=\"color:white\">Beltéri lámpa ébren tartása:   </h><h style=\"font-weight: bold;\" id=\"lightKeepOnLabel\">";
    content += LightTime;
    content += "</h><h style=\"font-weight: bold;\">  percig</h><br><br><div class=\"slidecontainer\"> <input type=\"range\" min=\"0\" max=\"15\" value=\"";
    content += LightTime;
    content += "\" class=\"slider\" id=\"lightKeepOnSlider\" name=\"lightTime\"></div><script> var slider = document.getElementById(\"lightKeepOnSlider\"); slider.oninput = function() { document.getElementById(\"lightKeepOnLabel\").innerHTML=slider.value; } </script>  <h><br><br>Kültéri lámpa ébren tartása:   </h><h style=\"font-weight: bold;\" id=\"lightKeepOnLabelout\">";
    content += LightTimeout;
    content += "</h><h style=\"font-weight: bold;\">  percig</h><br><br><div class=\"slidecontainer\"> <input type=\"range\" min=\"0\" max=\"15\" value=\"";
    content += LightTimeout;
    content += "\" class=\"slider\" id=\"lightKeepOnSliderout\" name=\"lightTimeout\"><script> var sliderout = document.getElementById(\"lightKeepOnSliderout\"); sliderout.oninput = function() { document.getElementById(\"lightKeepOnLabelout\").innerHTML=sliderout.value; } </script> </div><br><br><h>Beltéri lámpa bekapcsolása, amikor</h> <select name='InsideLightEvent' id='InLightVal' style='font-size:17px; border-style: none;background-color:transparent; color:white;'> <option value='1' style='color: #000000'>a garázs még be van reteszelve</option> <option value='2' style='color: #000000'>a garázs kinyílt</option> <option value='3' style='color: #000000'>beálltam</option> </select><br><br><h>Kültéri lámpa bekapcsolása, amikor</h> <select name='OutsideLightEvent' id='OutLightVal' style='font-size:17px; border-style: none;background-color:transparent; color:white;'> <option value='1' style='color: #000000'>a garázs még be van reteszelve</option> <option value='2' style='color: #000000'>a garázs kinyílt</option> <option value='3' style='color: #000000'>beálltam</option> </select>";
    content += "</div><br><h style=\"font-size:16px; font-weight: bold; color:white;\">SmartParking System:   </h><input id=\"SPHidden\" type=\"hidden\" value=\"1\" name=\"SPS\"><label class=\"switch1\"><input id=\"SP\" type=\"checkbox\" onclick=\"checkboxes()\" value=\"2\" name=\"SPS\"";
    if (currentSPS == 2) {
      content += "checked";
    }
    else {
      content += "unchecked";
    }
    content += "><div class=\"sliderbutton round\"></div></label>";
    content += "<div style='margin: 20px;'><h>Jelzőlámpák:   <h/><input id=\"SPLightHidden\" type=\"hidden\" value=\"1\" name=\"SPSLight\"><label class=\"switch1\"><input id=\"SPLight\" type=\"checkbox\"  value=\"2\" name=\"SPSLight\"";
    if (currentSPSLight == 2) {
      content += "checked";
    }
    else {
      content += "unchecked";
    }
    content += "><div class=\"sliderbutton round\"></div></label><script>function loadSelection(){document.getElementById('InLightVal').selectedIndex = \"";
    content += currentInLight - 1;
    content += "\"; document.getElementById('OutLightVal').selectedIndex = \"";
    content += currentOutLight - 1;
    content += "\"; } function checkboxes(){ if (document.getElementById('SP').checked == true){ document.getElementById('SPLight').disabled=false; } else { document.getElementById('SPLight').checked = false; document.getElementById('SPLight').disabled=true; }} $('#settings').submit(function() { if(document.getElementById(\"SP\").checked) { document.getElementById(\"SPHidden\").disabled = true; }if(document.getElementById(\"SPLight\").checked) { document.getElementById(\"SPLightHidden\").disabled = true; }return true; });</script><br><br></div><h style=\"font-size:16px; font-weight: bold; color:white;\">SmartOC System:</h><h style=\"color:white\"><div style=\"margin: 20px;\">Automatikus bereteszelés:   </h><h style=\"font-weight: bold;\" id=\"AutoCloseLabel\">";
    content += currentAutoClose;
    content += "</h><h style=\"font-weight: bold;\">  másodperc múlva</h><div class=\"slidecontainer\"><br> <input type=\"range\" min=\"15\" max=\"300\" value=\"";
    content += currentAutoClose;
    content += "\" class=\"slider\" id=\"AutoCloseSlider\" name=\"CloseTime\"></div><script> var sliderclose = document.getElementById(\"AutoCloseSlider\"); sliderclose.oninput = function() { document.getElementById(\"AutoCloseLabel\").innerHTML=sliderclose.value; } </script></div> <input type='submit' value='Mentés' ></form><br></body></html>";   //window.location = '/?DISCONNECT=YES';
    server.send(200, "text/html", content);

  }
}
void handleUsers() {
  if (!(is_authentified_1()) && !(is_authentified_2)) {
    String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }


  if (is_authentified_1()) {

    if (millis() > wait000) {
      buildXML();
      wait000 = millis() + 1000UL;
    }
    String content = "<!DOCTYPE HTML>\n";
    content += "<meta name='viewport' content='width=device-width, user-scalable=no'><html>\n";


    content += "<body style='background-color:black;' onload='processtest()'>\n";
    content += " <center><img id='status' src='' style='object-fit: cover;max-width: 102%'ondblclick='OCtest()'></center>";
    content += " <img src='https://r1ueoq-am3pap001.files.1drv.com/y4mRxqOgHm5ySCziy_vnXqHkkmPGkdk7NV-qgqRYF_FCGVIyzAOLDszC-eV-wuzx5nXwfym7DcwOf0K3X3-2PLBPUIK-4PosCo2MoYXWw6d1GnP01sgMEzoTzCmdkN8wiY6OQX1Wuq17lniDs1O-Wb0F9IZNSYWcSQ6EtvsFOVGB49ARjfXSkx1p-Hn3m0vYlbQHivaX6uAsEhDxhKCL3I_Wg?width=450&height=8&cropmode=none' style='object-fit: cover;width: 100%; '>";

    content += "<SCRIPT>\n";
    content += "xmlHttp=createXmlHttpObject();\n";
    content += "function createXmlHttpObject(){\n";
    content += "  if(window.XMLHttpRequest){\n";
    content += "    xmlHttp=new XMLHttpRequest();\n";
    content += "  }else{\n";
    content += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
    content += "  }\n";
    content += "  return xmlHttp;\n";
    content += "}\n";

    content += "function processtest(){\n";
    content += "  if(xmlHttp.readyState==0||xmlHttp.readyState==4){\n";
    content += "    xmlHttp.onreadystatechange=function(){\n";
    content += "      if(xmlHttp.readyState==4&&xmlHttp.status==200){\n";
    content += "        xmlDoc=xmlHttp.responseXML;\n";
    content += "        xmlmsg=xmlDoc.getElementsByTagName('CLOSEDLimit')[0].firstChild.nodeValue;\n";
    content += "        xmlmsg2=xmlDoc.getElementsByTagName('OPENEDLimit')[0].firstChild.nodeValue;\n";
    content += "        xmlmsg3=xmlDoc.getElementsByTagName('GarageEnd')[0].firstChild.nodeValue;\n";
    content += "        xmlmsg4=xmlDoc.getElementsByTagName('GarageStart')[0].firstChild.nodeValue;\n";

    content += "  if (xmlmsg == 1 && xmlmsg2 == 0 && xmlmsg3 == 0) { document.getElementById('status').src = 'https://pejf9g-am3pap001.files.1drv.com/y4mu0tced8WJQyV2Dj0nEJJvuYF9F2QjronFMEnTzMlAwXFxTCnV9Lxl6J9W6fVaOq6HArcKa1TDYJ1jnX3GTPmmlkjPXeGjYNGnbEC1Sdaa7jsbuI0HzosxG6SzCiokHX39yFR8fW1bYwtfMlmTTq2UaI5AXnv4eKqYgI5SIZg6ZyxYzHbnDC8hxhnpp2eACRJ7F4rcQQs1OoYuAW-CNxLkA?width=607&height=800&cropmode=none'; } if (xmlmsg ==0 && xmlmsg2 == 1 && xmlmsg3 == 1) { document.getElementById('status').src = 'https://preview.ibb.co/i3ufPw/nyit_res.gif'; } if (xmlmsg == 1 && xmlmsg2 == 0 && xmlmsg3 == 1) { document.getElementById('status').src = 'https://pel6ag-am3pap001.files.1drv.com/y4mPnHZEM9_vrCdjXpv0zO7A4u0pPKoYQYFZGnMIou05n-9jul5ldKTvzpmGZTgve_FSySs7owS2SXtlHDnUu-cj485zagPJag-DLl-P2TbblYQb9Ta-yn-ouWExdp4AxwOFjk_bTgFXWehzBH3UiLAkDcLy2BYHUF6ZaLdlDJDwCkbx_RscbGckBZa5T4h8dbvAwRXUEnDQ1PWOMfYJRZBuw?width=607&height=800&cropmode=none'; } if (xmlmsg == 0 && xmlmsg2 == 1 && xmlmsg3 == 0) { document.getElementById('status').src = 'https://peiwva-am3pap001.files.1drv.com/y4m53eCWHDou8jUpzVzsLgfY5ezxcr815aG8NCgy4prCYq_SjLM_-plj-PlZLw1f-GryN6_taUIj2cid19NEl04aIL9jh9Q8wcH4Im4Js8iv6OlSAGliQ1nmUmkuLXRDludrvfb1wJnCqy92N_ewtwvBys4rmaY2COSqkuRuxLntQ0AjnxResicJIKr_WXZaOeY1bJt5Pv_MU7xnIn2b2psew?width=607&height=800&cropmode=none'; } if (xmlmsg == 0 && xmlmsg2 == 0 && document.getElementById('status').src == 'https://pejf9g-am3pap001.files.1drv.com/y4mu0tced8WJQyV2Dj0nEJJvuYF9F2QjronFMEnTzMlAwXFxTCnV9Lxl6J9W6fVaOq6HArcKa1TDYJ1jnX3GTPmmlkjPXeGjYNGnbEC1Sdaa7jsbuI0HzosxG6SzCiokHX39yFR8fW1bYwtfMlmTTq2UaI5AXnv4eKqYgI5SIZg6ZyxYzHbnDC8hxhnpp2eACRJ7F4rcQQs1OoYuAW-CNxLkA?width=607&height=800&cropmode=none') { document.getElementById('status').src = 'https://media.giphy.com/media/xULW8m9SRwKJTgswNy/giphy.gif'; } if (xmlmsg == 0 && xmlmsg2 == 0 && document.getElementById('status').src == 'https://peiwva-am3pap001.files.1drv.com/y4m53eCWHDou8jUpzVzsLgfY5ezxcr815aG8NCgy4prCYq_SjLM_-plj-PlZLw1f-GryN6_taUIj2cid19NEl04aIL9jh9Q8wcH4Im4Js8iv6OlSAGliQ1nmUmkuLXRDludrvfb1wJnCqy92N_ewtwvBys4rmaY2COSqkuRuxLntQ0AjnxResicJIKr_WXZaOeY1bJt5Pv_MU7xnIn2b2psew?width=607&height=800&cropmode=none' ) { document.getElementById('status').src = 'https://media.giphy.com/media/3oFzm3lcbU3AF96iZO/giphy.gif'; } if (xmlmsg == 0 && xmlmsg2 == 0 && document.getElementById('status').src == 'https://pel6ag-am3pap001.files.1drv.com/y4mPnHZEM9_vrCdjXpv0zO7A4u0pPKoYQYFZGnMIou05n-9jul5ldKTvzpmGZTgve_FSySs7owS2SXtlHDnUu-cj485zagPJag-DLl-P2TbblYQb9Ta-yn-ouWExdp4AxwOFjk_bTgFXWehzBH3UiLAkDcLy2BYHUF6ZaLdlDJDwCkbx_RscbGckBZa5T4h8dbvAwRXUEnDQ1PWOMfYJRZBuw?width=607&height=800&cropmode=none' ) { document.getElementById('status').src = 'https://media.giphy.com/media/3oFzlUDkMik1dcH7MY/giphy.gif'; } if (xmlmsg == 0 && xmlmsg2 == 0 && document.getElementById('status').src == 'https://preview.ibb.co/i3ufPw/nyit_res.gif') { document.getElementById('status').src = 'https://media.giphy.com/media/l49JRwW2GF2YMVHeU/giphy.gif'; }\n";

    content += "      }\n";
    content += "    }\n";
    content += "    xmlHttp.open('PUT','xml',true);\n";
    content += "    xmlHttp.send(null);\n";
    content += "  }\n";
    content += "  setTimeout('processtest()',1000);\n";
    content += "}\n";

    content += "function OCtest(){\n";
    // document.getElementById('status').style= 'pointer-events:none;object-fit: cover;max-width: 102%';
    content += "  if(xmlHttp.readyState==0||xmlHttp.readyState==4){\n";
    content += "    xmlHttp.open('PUT','dynamicoc',true);\n";
    content += "    xmlHttp.send(null);\n";
    content += "  }\n";
    content += "  }\n";



    content += "</SCRIPT>\n";
    content += "</body>\n";
    content += "</html>\n";
    server.send(200, "text/html", content);
  }



}

void handleNotFound() {
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void buildXML() {
  XML = "<?xml version='1.0'?>";
  XML += "<xml>";
  XML += "<CLOSEDLimit>";
  XML += digitalRead(CLOSEDLimit);
  XML += "</CLOSEDLimit>";

  XML += "<OPENEDLimit>";
  XML += digitalRead(OPENEDLimit);
  XML += "</OPENEDLimit>";

  XML += "<GarageEnd>";
  XML += digitalRead(GarageEnd);
  XML += "</GarageEnd>";

  XML += "<GarageStart>";
  XML += digitalRead(GarageStart);
  XML += "</GarageStart>";

  XML += "</xml>";
}

void handleXML() {
  buildXML();
  server.send(200, "text/xml", XML);
}

void DynamicOChandle() {
  if (is_authentified_1()) {
    EEPROM.begin(512);
    EEPROM.get(5, currentLightTime);
    EEPROM.get(10, currentLightTimeout);
    EEPROM.get(30, currentInLight);
    EEPROM.get(40, currentOutLight);
    EEPROM.get(15, currentSPS);
    EEPROM.get(23, currentSPSLight);
    EEPROM.get(50, currentAutoClose);
    digitalWrite(16, HIGH);
    delay(1000);
    digitalWrite(16, LOW);
    Wire.beginTransmission(8);
    Wire.write(currentAutoClose);
    Wire.write(currentLightTime);
    Wire.write(currentLightTimeout);
    Wire.write(currentInLight);
    Wire.write(currentOutLight);
    Wire.write(currentSPS);
    Wire.write(currentSPSLight);
    Wire.endTransmission();




  }
  String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
  server.sendContent(header);
}

void loop() {
  server.handleClient();
}
