
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer servidor(80);
void principal(){
  servidor.send(200, "text/html", "DEU BOM EU ACHO");
}

//=========================================================================//
void adicionawifi(){

  String pagina;
  int totalredes = WiFi.scanNetworks();   
  String redes[totalredes];
  
  for (int i = 0; i < totalredes; ++i) {     
    redes[i] = WiFi.SSID(i);            
  }
  
  pagina += "<html>"
             "<head>"
             "<title>CADASTRANDO SENHA</title>"
             "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
             "</head>"
             "<body>"
             "<form action='/fgwtyk' method='POST'>"
             "<label for='ssid'>ESCOLHA UMA REDE: </label>"
             "<select id='ssid' name='ssid'>";
  for(int i = 0; i < totalredes; ++i){
    
    pagina += "<option value='";
    pagina += redes[i];
    pagina += "'>";
    pagina += redes[i];
    pagina +="</option>";
  }      
  pagina += "</select><input type='text' name='senha'"
            "placeholder='DIGITE A SENHA AQUI'>"
            "<input type='submit' value='CADASTRAR'>"
            "</form>"
            "</body>"
            "</html>";
  
  servidor.send(200, "text/html", pagina);

}
//=========================================================================//
void fgwtyk(){
  
  String ssid = servidor.arg("ssid");
  String senha = servidor.arg("senha"); 
  escrevefrase(200, ssid);
  escrevefrase(250, senha);
  servidor.close();
  servidor.stop();
  WiFi.softAPdisconnect(true);
  delay(100);
  ESP.restart();

}
//=========================================================================//
void wifiespertim(){ 
  
  String retorno = verifica();
  if (retorno == "SSID" || retorno == "SENHA"){
    Serial.println(retorno);
    novosave();
  }
  else{
    return;
  }
}
//=========================================================================//
void novosave(){
  int cont = 0; 
  String nomeap = "SALVANDO_WIFI";
  while(true){
    if(cont < 1){  
      WiFi.disconnect(true);
      WiFi.softAP(nomeap);
      servidor.on("/", adicionawifi);
      servidor.on("/fgwtyk", fgwtyk);
      servidor.begin();
      Serial.print("criei nesse IP: ");
      Serial.println(WiFi.softAPIP());
      cont = cont + 1;
    }
    servidor.handleClient();
  }
}
//=========================================================================//

void escrevefrase(int endebase, String frase){
  
  for (int i = 0;i < frase.length(); i++){
    EEPROM.write(endebase, frase[i]);
    endebase++;
  }
  EEPROM.write(endebase, '\0');
  EEPROM.commit();
}
//=========================================================================//
String montafrase(int endebase){ 
  bool vai = true; 
  char letra = EEPROM.read(endebase);
  String frase = "";
  
  while(vai){  
    if (letra != '\0'){  
      frase = frase + letra;
      endebase++;
      letra = EEPROM.read(endebase);
     }
    else{
      vai = false;
    }
  }
  return frase;
}

//=========================================================================//
String verifica(){
  String nome = montafrase(200);
  String senha = montafrase(250);
  Serial.println(nome);
  Serial.println(senha);

  WiFi.begin(nome, senha);
  
  int retorno = WiFi.waitForConnectResult();
  if (retorno == 3){
    return "CONECTADO";
  }
  else if (retorno == 1){
    return "SSID";
  }
  else{
    return "SENHA";
  }
}
//=========================================================================//
void setup() {
  
  EEPROM.begin(300);
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  
  wifiespertim();
  
  servidor.on("/", principal);
  servidor.begin();
  
}

void loop() {
  servidor.handleClient();
  }
