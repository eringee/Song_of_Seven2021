////////////////////////////////OSC Helpers//////////////////////////////////
/*
    This file contains all the helpers functions necessary for OSC communication:
    - routeConnect()
    -routeName()
    -routeLocation()
    -routeDate()
    -routeRate()
    -routeSignals()
    -routeStart()
    -routeStop()
    -routeMarker()
    -oscUpdate()   
*/

//------------------------------------------------------------------------------------------------

void routeConnect(OSCMessage &msg ) {
  //This function is a callback function for the /connect message
  //It sets the computerIp variable and send a reply to the computer
  //telling it its connected

  //Serial.println("Connection to computer");
  computerIP = Udp.remoteIP();
  Connected = true;

  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/connected").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message

}

//------------------------------------------------------------------------------------------------

void routeName(OSCMessage &msg) {
  //This function is a callback function for the /name message
  //It sets the subject name in the header and send a reply to
  //the computer telling it its received

  Serial.println("Received Name");

  char stringBuffer[32];
  String message;
  msg.getString(0, stringBuffer, 32);
  message = stringBuffer;
  r.setSubjectName(message );
  nameReceived = true;

  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/namereceived").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message
}

//------------------------------------------------------------------------------------------------

void routeLocation(OSCMessage &msg) {
  //This function is a callback function for the /location message
  //It sets the location in the header and send a reply to
  //the computer telling it its received

  Serial.println("Received Location");
  char stringBuffer[32];
  String message;
  msg.getString(0, stringBuffer, 32);
  message = stringBuffer;
  r.setLocation( message );
  locReceived = true;

  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/locationreceived").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message
}

//------------------------------------------------------------------------------------------------

void routeDate(OSCMessage &msg) {
  //This function is a callback function for the /date message
  //It sets the date of the recording in the header and sends a
  //reply to the computer telling it that it is received

  Serial.println("Received Date");

  char stringBuffer[32];
  String message;
  msg.getString(0, stringBuffer, 32);
  message = stringBuffer;

  //Serial.println(d);
  String dd = message.substring(0, 2);
  //Serial.println(dd);
  String mm = message.substring(2, 4);
  //Serial.println(mm);
  String yy = message.substring(4, 6);
  //Serial.println(yy);
  r.setDate(dd, mm, yy);

  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/datereceived").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message
}

//------------------------------------------------------------------------------------------------

void routeRate(OSCMessage &msg) {
  //This function is a callback function for the /rate message
  //It sets the sampling rate of the recording in the header and sends a
  //reply to the computer telling it that it is received

  Serial.println("Received Sample Rate");
  char stringBuffer[32];
  String message;
  msg.getString(0, stringBuffer, 32);
  message = stringBuffer;

  r.setRecRate(message.toInt() );


  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/ratereceived").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message
}

//------------------------------------------------------------------------------------------------

void routeSignals(OSCMessage &msg) {
  //This function is a callback function for the /signals message
  //It sets the signal types of the recording in the header and send a
  //reply to the computer telling it its received

  Serial.println("Received Signals types");
  char stringBuffer[32];
  String message;
  msg.getString(0, stringBuffer, 32);
  message = stringBuffer;
  String signalsArray[NUM_SIGNALS] ;
  int idx = 0;
  int substringLastIdx = 0;



  for ( unsigned int i = 0 ; i < message.length() ; i++ ) {
    char current = message[i];
    //Serial.println(current);
    if ( i == message.length() - 1 ) {

      signalsArray[idx] = message.substring(substringLastIdx, i + 1);
      //Serial.println(signalsArray[idx]);
    }
    if ( current == 44) {
      signalsArray[idx] = message.substring(substringLastIdx, i);
      //Serial.println(signalsArray[idx]);
      substringLastIdx = i + 1 ;
      idx++;
    }
  }


  r.setSignals(signalsArray);
  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/signalreceived").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message
}

//------------------------------------------------------------------------------------------------

void routeStart(OSCMessage &msg) {
  //This function is a callback function for the /start message
  //It sets the recording to start in the next loop and send a
  //reply to the computer telling it its received
  Serial.println("Setting up Recording");
  r.setupRecording();
  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/recStarted").add(value);
  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message


}

//------------------------------------------------------------------------------------------------

void routeStop(OSCMessage &msg) {
  //This function is a callback function for the /stop message
  //It sets the recording to stop in the next loop and send a
  //reply to the computer telling it its received
  r.stopProcess = true;
  Serial.println("Stop Recording");

  //sends reply
  int value = 1;
  OSCBundle reply;
  reply.add("/recStop").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message


}

//------------------------------------------------------------------------------------------------

void routeMarker(OSCMessage &msg) {
  //This function is a callback function for the /marker message
  //It places a marker at th corresponding timetag and send a
  //reply to the computer telling it its received

  Serial.println("Place Marker");
  r.placeMarker();



  //sends reply
  int value = millis() - stamp;
  OSCBundle reply;
  reply.add("/placed").add(value);

  Udp.beginPacket(computerIP, computerPort);
  reply.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  reply.empty(); // free space occupied by message

}

//------------------------------------------------------------------------------------------------

void oscUpdate() {
  //This function check every loop if it received a message
  //If one is received it routes it to the associated callback
  //Add all routing option in this function
  OSCBundle bundleIN;
  int size;

  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      // Serial.println(Udp.read());
      bundleIN.fill(Udp.read());


    if (!bundleIN.hasError()) {

      //PLACE ELEMENTS TO ROUTE HERE:
      bundleIN.route("/connect" , routeConnect);
      bundleIN.route("/name" , routeName);
      bundleIN.route("/location" , routeLocation);
      bundleIN.route("/date" , routeDate);
      bundleIN.route("/rate" , routeRate);
      bundleIN.route("/signals" , routeSignals);
      bundleIN.route("/start" , routeStart);
      bundleIN.route("/stop" , routeStop);
      bundleIN.route("/marker" , routeMarker);
    } else if (bundleIN.hasError()) {
      Serial.println("error");
    }
  }
}
