/* Chibi for Arduino, Example 3
This example is the same as the previous examples except that it 
changes the channel to one that is not occupied by any 802.11 Wi-Fi
device. Its basically just to show how to change the channel.
*/

#include <chibi.h>

byte msg[] = "Hello World";
byte buf[CHB_MAX_PAYLOAD];

void setup()
{
  Serial.begin(57600);
  Serial.setTimeout(2000);
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
    chibiGetData(buf);
    Serial.println((char *)buf);
  }
  if(Serial.available()>0)
  {
    String s=Serial.readString();
    Serial.println(s);
    SendZB(s);
  }
  // delay half a second between transmission
  delay(100);
}
void SendZB(String s)
{
  #ifdef zigbee
  byte data[CHB_MAX_PAYLOAD];
  unsigned int len;
  
 
  len=byte(s.length());
  s.toCharArray(data,len);
//  #ifdef crc_calc
//    byte value = 0;
//    value=CRC8((char *)data,len);
//    #ifdef debug
//    Log.Verbose(F("\nCrc:"));Log.Verbose(String(value));Log.Verbose(F("\n"));
//    #endif
//    s+=String(value);
//  #endif
//  s+='\0';
//  len=byte(s.length());
//  s.toCharArray(data,len);
  chibiTx(BROADCAST_ADDR, (byte *)data,len);
  #endif
  return;
}
