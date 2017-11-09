/* Chibi for Arduino, Example 3
This example is the same as the previous examples except that it 
changes the channel to one that is not occupied by any 802.11 Wi-Fi
device. Its basically just to show how to change the channel.
*/

#include <chibi.h>

byte msg[] = "Hello World";
byte buf[CHB_MAX_PAYLOAD];
String msgToSend="";
void setup()
{
  Serial.begin(38400);
  Serial.setTimeout(1000);
  // Init the chibi wireless stack
  chibiInit();
  
  // set the channel to channel 20. channel 20 is out of band of
  // 802.11 wi-fi channels
  chibiSetChannel(20);
}

void loop()
{ 
  // We're going to automatically calculate the length
  // of the message string using strlen(). We add 1 to the
  // length to handle the terminating character '/0'.
  // We're also sending a broadcast so any node in listening
  // range will hear the message. 
  //chibiTx(BROADCAST_ADDR, msg, 12);

  // We're going to print anything we receive in case there are others 
  // transmitting.
  if (chibiDataRcvd() == true)
  {
    //Serial.print("(");
    int len, rssi, src_addr;
    byte buf[CHB_MAX_PAYLOAD];  // this is where we store the received data
    
    // retrieve the data and the signal strength
    len = chibiGetData(buf);
    //Serial.print(len);
    // discard the data if the length is 0. that means its a duplicate packet
    if (len > 0){    

//    rssi = chibiGetRSSI();
    src_addr = chibiGetSrcAddr();
    
    // Print out the message and the signal strength
//    Serial.print("Message received from node 0x");
//    Serial.print(src_addr, HEX);
    //Serial.print("> "); 
    Serial.print((char *)buf); //CharToStr(buf));//
    }
  }
  if(Serial.available()>0)
  {
    String s=Serial.readString();
    Serial.print("<");
    Serial.println(s);
    SendZB(s);
  }
  // delay half a second between transmission
  delay(100);
}
void SendZB(String s)
{
 
  byte data[CHB_MAX_PAYLOAD];
  unsigned int len;
  msgToSend+=s;
  //Serial.print("\n-");Serial.println(msgToSend);
  len=msgToSend.length();
  char ce=msgToSend[len-1];
  
  if ((ce=='\n')|(ce=='\0'))
  {
    //Serial.print("\n--");Serial.println(String(ce));
//  len=byte(s.length());
//  s.toCharArray(data,len);
//  #ifdef crc_calc
//    byte value = 0;
//    value=CRC8((char *)data,len);
//    #ifdef debug
//    Log.Verbose(F("\nCrc:"));Log.Verbose(String(value));Log.Verbose(F("\n"));
//    #endif
//    s+=String(value);
//  #endif
  //s+='\0';
  //len=byte(s.length());
  //Serial.print("Len=");Serial.println(len);
  //msgToSend.remove(len-1,1);
  msgToSend+='\0';
  len++;
  msgToSend.toCharArray(data,len);
  msgToSend="";
  chibiTx(BROADCAST_ADDR, (byte *)data,len);
  }
  
  return;
}
