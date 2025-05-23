////////////////////////////////Hardware Helper//////////////////////////////////
/*
    This file contains the necessary helpers functions that have
    something to do with hardware parts

    -checkForCard()
    -cardInfo()
    -detectHardware()
    -detectCable()
*/

//------------------------------------------------------------------------------------------------

void checkForCard() {
  //This function check if a SD card is inserted

  Serial.print("\nInitializing SD card...");
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_FULL_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
    SD.begin(chipSelect);
  }


}

//------------------------------------------------------------------------------------------------

void cardInfo() {
  //This function prints the card info to the serial port

 // print the type of card
 Serial.print("\nCard type: ");
 switch (card.type()) {
   case SD_CARD_TYPE_SD1:
     Serial.println("SD1");
     break;
   case SD_CARD_TYPE_SD2:
    Serial.println("SD2");
     break;
   case SD_CARD_TYPE_SDHC:
     Serial.println("SDHC");
     break;
   default:
     Serial.println("Unknown");
     break;
 }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
 Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  if (volumesize < 8388608ul) {
    Serial.print("Volume size (bytes): ");
    Serial.println(volumesize * 512);        // SD card blocks are always 512 bytes
  }
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 2;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
 
 // root.openRoot(volume);

  // list all files in the card with date and size
//  root.ls(LS_R | LS_DATE | LS_SIZE);

}

//------------------------------------------------------------------------------------------------

void detectHardware() {

  // This function verify if hardware to communicate with the computer is connected

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
   // Serial.println("Ethernet shield was not found.");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  else if (Ethernet.hardwareStatus() == EthernetW5100) {
   // Serial.println("W5100 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5200) {
  //  Serial.println("W5200 Ethernet controller detected.");
  }
  else if (Ethernet.hardwareStatus() == EthernetW5500) {
  //  Serial.println("W5500 Ethernet controller detected.");
  }
}

//------------------------------------------------------------------------------------------------

void detectCable() {
  // This function verify is a cable is connected to the hardware

  if (Ethernet.linkStatus() == LinkOFF) {
 //   Serial.println("Ethernet cable is not connected.");
  } else {
  //  Serial.println("Ethernet cable is connected!");
  }
}
