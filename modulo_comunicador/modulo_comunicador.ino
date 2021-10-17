#include <ArduinoWebsockets.h>

const char* ssid = "RedeWifidoSistema"; // Nome da rede wi-fi
const char* password = "Batatinha1,2,3"; // Senha da rede  wi-fi
char* ssid_mesa = ""; //ID da mesa
const char* websockets_server_host = "192.168.1.107"; // IP local do servidor websocket
const int websockets_server_port = 8091; // Porta de conexão do servidor (deve ser a mesma do back-end!)

// Utilizamos o namespace de websocket para podermos utilizar a classe WebsocketsClient
using namespace websockets;

// Objeto websocket client
WebsocketsClient client;

//retorna o RSSI da rede buscada (caso não ache, retorna zero)
int32_t getRSSI(const char* target_ssid) {
  byte available_networks = WiFi.scanNetworks(); //escaneia o numero de redes

  for (int network = 0; network < available_networks; network++) {
//    Serial.print(WiFi.SSID(network)); Serial.print(" | "); Serial.println(WiFi.RSSI(network));
    if (WiFi.SSID(network).compareTo(target_ssid) == 0) { //compara se alguma das redes encontradas é a que desejamos  
      return WiFi.RSSI(network); //retorna o SSID da rede
    }
  }
  return 0;
}

void callBack(void){ //char* ssid_mesa){
    // Iniciamos o callback onde as mesagens serão recebidas
    client.onMessage([&](WebsocketsMessage message){        
    ssid_mesa = (char*)(message.data());
    // Exibimos a mensagem recebida na serial
    Serial.print("Mensagem recebida: ");
    Serial.println(ssid_mesa);
    Serial.print("ssid_mesa: ");
    Serial.println(ssid_mesa);
    if(message.data().equalsIgnoreCase(ssid_mesa)){
        int32_t rssi_mesa = getRSSI(ssid_mesa); //busca o RSSI da mesa a servir
        client.send("Servindo a mesa: ");
        client.send(ssid_mesa);
        // For debugging purpose only
        Serial.print("Sinal de ");
        Serial.print(ssid_mesa);
        Serial.print(" força: ");
        Serial.print(rssi_mesa);
        Serial.println("dBm");
      }
    else{
        Serial.print("Robo diz: Nao sei para onde ir");
    }
  });
  return;     
}

void setup() 
{
    // Iniciamos a serial com velocidade de 115200
    Serial.begin(115200);
    // Definimos o pino como saída
//    pinMode(led, OUTPUT);

    //busca o RSSI da rede desejada
    int32_t rssi_back_end = getRSSI(ssid);  
    // For debugging purpose only
    Serial.print("Signal strength: ");
    Serial.print(rssi_back_end);
    Serial.println("dBm");   
    
    // Conectamos o wifi
    WiFi.begin(ssid, password);
    // Enquanto não conectar printamos um "."
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(1000);
    }
    // Exibimos "WiFi Conectado"
    Serial.println("Connected to Wifi, Connecting to server.");

    // Tentamos conectar com o websockets server
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    // Se foi possível conectar
    if(connected){
        // Exibimos mensagem de sucesso
        Serial.println("Conectado!");
        // Enviamos uma msg "Hello Server" para o servidor
        client.send("Robo garçom a servico");
    }   // Se não foi possível conectar
    else{
        // Exibimos mensagem de falha
        Serial.println("Not Connected!");
        return;
    }

    callBack();
}

void loop(){
  //  De tempo em tempo, o websockets client checa por novas mensagens recebidas
  if(client.available()) 
    client.poll();
        
  delay(500);
}
