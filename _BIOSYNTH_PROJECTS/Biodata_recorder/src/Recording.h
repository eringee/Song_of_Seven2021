////////////////////////////////Recording format Class//////////////////////////////////
/*
    This file contains the recording class. It format the data
    in a EDF way and handle the the recording state of the device.
    
    -Recording() <- Constructor
    -clearHeaderBuffer()
    -formatHeader1() //formatHeader2() // formatHeader3() // formatHeader4()
    -isIdle() // isReadyToStart() // isReadyToStop() //isRecording()
    -placeMarker() //resetMarkerBool()
    -readyToStartAgain()
    -setupRecording()
    -startRecording()
    -stopRecording()
    -channelNames()
    -formatData()
    -setLocation() // getLocation()
    -setSubjectName() // getSubjectName()
    -setSignals() // getSignals()
    -setRecRate() // getRecRate()
    -setDate() // getDate()
    -setDuration() // getDuration()
    -startEndDelay() // stopEndDelay() // updateEndDelay()
    -startCountdown() // stopCountdown() // updateCountdown() // resetCountdown()
    -recordingLength()
*/

//Header Example
//################################################################
//#Name: Etienne Montenegro                         Date: 23/06/20
//#Duration: 1:00:00                            Location: Montreal
//#Start time: 13:40:34                         End Time: 13:40:34
//#Signals: heart , gsr1 , gsr2 , resp        Sample Rate: 1000 Hz
//################################################################

#ifndef RECORDING_H
#define RECORDING_H

class Recording {

  public:

    //Header arrays
    String headerLine = "#"; //String buffer to write header


    //Title used in header formating
    String nameTitle = "Name: ";
    String dateTitle = "Date: ";
    String durationTitle = "Duration: ";
    String locationTitle = "Location: ";
    String startTimeTitle = "Start time: ";
    String endTimeTitle = "End Time: ";
    String signalsTitle = "Signals: ";
    String sampleRateTitle = " Sample Rate: ";


    String date; //date of the recording formated dd/mm/yy
    String location ; // location where the recording took place --- Can allocate more space if needed
    String subjectName ; //name of the subject ------ Can allocate more space if needed
    String signals[10]; // format ["heart, "gsr1" , "gsr2" ,"resp"]
    int rate ; //refresh rate of the sensors
    int mode ; //current mode the object is in 0 = ready to start 1 =  recording 2 = ending recording -1 = idle
    int numSignals; //number of sampled signal

    //MODIFIY THIS WHEN RTC IS IMPLEMENTED
    //try to implement with rtc or send this info via OSC
    String duration = "00:00:00";
    String startTime = "00:00:00";
    String endTime = "00:00:00";

    //Need RTC TO IMPLEMENT THIS
    int startHour ; //starting hour of the recording
    int startMin ; //starting min of the recording
    int startSec ; //starting sec of the recording

    int endHour ; //end hour of the recording
    int endMin ; //end min of the recording
    int endSec ; //end sec of the recording

    char dataBuff[100]; //Buffer used to format strings when using sprintf()

    bool headerPrinted; //boolean to prevent the header to print twice

    bool stopProcess ; //Dont know if this is used in code
    bool marker ; //boolean to let the program know if a marker needs to be placed or not
    bool updateCtdwn;
    Chrono startDelay;
    Chrono endDelay;
    Chrono seconds;
    int ctdwnIndex;
    bool endDelayNotStarted = true;
    ////////////////////////METHODS/////////////////////////////
    Recording(int _numSignals) {


      numSignals = _numSignals;
      rate = 1000000 / REFRESH_RATE;

      ctdwnIndex = 10;
      mode = -1 ;
      headerPrinted = false;
      stopProcess = false;
      marker = false;
      updateCtdwn = false;
      startDelay.restart();
      seconds.restart();
      endDelay.restart();

      startDelay.stop();
      seconds.stop();
      endDelay.stop();

    }; //constructor

    //------------------------------------------------------------------------------------------------


    void clearHeaderBuffer() {
      //Empty the string buffer to be able to properly write the next header line

      headerLine.remove(1, headerLine.length()); //starts at 1 to keep the # in the string
    }

    //------------------------------------------------------------------------------------------------
    //methods to correctly format each header lines

    String formatHeader1() {
      //Format the first line of the header correctly

      int spaceLength = 64 - nameTitle.length() - subjectName.length() - dateTitle.length() - date.length();
      String spaces;

      for ( int i = 0 ; i < spaceLength ; i++ ) {
        spaces.concat(" ");
      }
      headerLine.concat(nameTitle);
      headerLine.concat(subjectName);
      headerLine.concat(spaces);
      headerLine.concat(dateTitle);
      headerLine.concat(date);

      return headerLine;
    }

    String formatHeader2() {
      //Format the second line of the header correctly

      int spaceLength = 64 - durationTitle.length() - duration.length() - locationTitle.length() - location.length();
      String spaces;

      for ( int i = 0 ; i < spaceLength ; i++ ) {
        spaces.concat(" ");
      }
      headerLine.concat(durationTitle);
      headerLine.concat(duration);
      headerLine.concat(spaces);
      headerLine.concat(locationTitle);
      headerLine.concat(location);

      return headerLine;
    }

    String formatHeader3() {
      //Format the third line of the header correctly

      int spaceLength = 64 - startTimeTitle.length() - startTime.length() - endTimeTitle.length() - endTime.length();
      String spaces;

      for ( int i = 0 ; i < spaceLength ; i++ ) {
        spaces.concat(" ");
      }
      headerLine.concat(startTimeTitle);
      headerLine.concat(startTime);
      headerLine.concat(spaces);
      headerLine.concat(endTimeTitle);
      headerLine.concat(endTime);

      return headerLine;
    }

    String formatHeader4() {
      //Format the fourth line of the header correctly
      //61 instead of 64 because adding " Hz" at the end

      int signalsLength = 0 ;

      for ( int i = 0 ; i < numSignals ; i++ ) {
        signalsLength += getSignal(i).length();
      }

      int spaceLength = 75 - signalsTitle.length() - signalsLength - (numSignals - 1) - sampleRateTitle.length() - String(rate).length();
      String spaces;

      for ( int i = 0 ; i < spaceLength ; i++ ) {
        spaces.concat(" ");
      }
      headerLine.concat(signalsTitle);
      for ( int i = 0 ; i < numSignals ; i++ ) {
        headerLine.concat(getSignal(i));
        if ( i != numSignals - 1) headerLine.concat(",");
      }
      headerLine.concat(spaces);
      headerLine.concat(sampleRateTitle);
      headerLine.concat(rate);
      headerLine.concat(" Hz");

      return headerLine;
    }
    //------------------------------------------------------------------------------------------------
    //methods to check if the device is in a specifc mode

    bool isIdle() {
      //returns true if the recording is ready to start if not returns false

      if ( mode == -1) {
        return true;

      } else {
        return false;
      }
    }

    bool isReadyToStart() {
      //returns true if the recording is ready to start if not returns false

      if ( mode == 0) {
        return true;

      } else {
        return false;
      }
    }

    bool isReadyToStop() {
      //returns true if the recording is ready to stop if not returns false

      if ( mode == 2) {
        return true;

      } else {
        return false;
      }
    }

    bool isRecording() {
      //returns true if the device is recording if not returns false

      if ( mode == 1 ) {
        return true;

      } else {
        return false;
      }
    }

    //------------------------------------------------------------------------------------------------
    //methods used to place a marker at the current timestamp in the csv file
    void placeMarker() {
      marker = true;
    }

    void resetMarkerBool() {
      marker = false;
    }

    //------------------------------------------------------------------------------------------------

    void readyToStartAgain() {
      //sets the device to the idle mode

      mode = -1;
      stopProcess = false;
      endDelayNotStarted = true;
      endDelay.restart();
      endDelay.stop();
    }

    //------------------------------------------------------------------------------------------------

    void setupRecording() {
      //sets the device in setup mode

      mode = 0;
    };

    //------------------------------------------------------------------------------------------------

    void startRecording() {
      //sets the device in recording mode

      mode = 1;
    };

    //------------------------------------------------------------------------------------------------

    void stopRecording() {
      // sets the device in stopping mode
      mode = 2;
    };


    //------------------------------------------------------------------------------------------------

    String channelNames() {
      // format the csv columns header

      String label = "timestamp";
      for ( int i = 0 ; i < numSignals ; i++) {
        label.concat(",");
        label.concat(signals[i]);
      }

      return label ;

    };

    //------------------------------------------------------------------------------------------------

    //passing int instead of unsigned long could cause to truncate the timestamp
    //try to pass two argument ( unsigned long timestamp  , int data[3] )

    String formatData(unsigned long timestamp, float data[8]) { // format the sampled data to csv
      //Formats the logging of data

      // each log of data will be formated like this :
      // timestamp HEART HEARTAMP HEARTBPM SCR SCL RESP RESPAMP RESPBPM
      // 987753,0,1023,1023,1023

      sprintf(dataBuff , "%ld %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f;" , timestamp, data[0], data[1], data[2],data[3],data[4], data[5], data[6],data[7]);
      return dataBuff ;
    };


    //--------------------------------------------------------------

    void setLocation(String _Location) { //set subject name
      location = _Location;
    };

    String getLocation() { //return subject name as string
      return location;
    };

    //--------------------------------------------------------------

    void setSubjectName(String _SubjectName) { //set subject name
      subjectName = _SubjectName;
    };

    String getSubjectName() { //return subject name as string
      return subjectName;
    };

    //--------------------------------------------------------------

    void setSignals(String _Signals[4]) { //set subject name
      for ( int i = 0 ; i < numSignals ; i++ ) {
        signals[i] = _Signals[i];
      }
    };

    String getSignal(int index) { //return subject name as string
      return signals[index];
    };

    //--------------------------------------------------------------

    void setRecRate(int _Rate) { //set subject name
      rate = _Rate;
    };

    int getRecRate() { //return subject name as string
      return rate;
    };

    //--------------------------------------------------------------

    void setDate(String dd , String mm , String yy) { //set date
      date = dd + "/" + mm + "/" + yy;
    };

    String getDate() { //return the date as string
      return date;
    };

    //--------------------------------------------------------------

    void setDuration(String h , String m , String s) { //set date
      duration = h + ":" + m + ":" + s;
    };

    String getDuration() { //return the date as string
      return duration;
    };

    //--------------------------------------------------------------
    // start/stop/update methods for the delay at the end
    // of each recording sessions

    void startEndDelay() {
      endDelayNotStarted = false;
      endDelay.resume();
    };

    void stopEndDelay() {
      endDelay.stop();
    }

    bool updateEndDelay() {
      bool test = false;


      if (endDelay.hasPassed(5000)) {
        test = true;
      }

      return test ;
    }

    //--------------------------------------------------------------
    // start/stop/update and reset methods for the delay at the beginning
    // of each recording sessions

    void startCountdown() {
      startDelay.resume();
      seconds.resume();
    }

    void stopCountdown() {
      startDelay.stop();
      seconds.stop();
    }

    bool updateCountdown() {
      bool boolToReturn = false;



      if (startDelay.hasPassed(10000)) {
        boolToReturn = true;
        startDelay.restart();

      }


      if (seconds.hasPassed(1000)) {
        seconds.restart();
        ctdwnIndex--;

      }
      return boolToReturn ;
    };

    void resetCountdown() {
      ctdwnIndex = 10;
    };

    //--------------------------------------------------------------

    void recordingLength( unsigned long timestamp) {
      //calculate the lenght of the recording session based on the timestamps
      int timeInSec = timestamp / 1000;

      int numOfHour = timeInSec / 3600;

      int numOfMin = (timeInSec - (numOfHour * 3600)) / 60;

      int numOfSec = (timeInSec - (numOfHour * 3600) - ( numOfMin * 60));

      String h = numOfHour;
      String m = numOfMin;
      String s = numOfSec;

      setDuration(h , m, s);
    };



};


#endif
