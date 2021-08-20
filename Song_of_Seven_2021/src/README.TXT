BIOSYNTH MIDI FILE PLAYER


Description :

In this version of the project the biosynth reads the notes to play from midi files on the sd card.
It's reading the note on the 4 first channels (0-3) and play them on the synth linked to it.

ch0 -> Heart Synth 
ch1 -> GSR1 Synth
ch2 -> GSR2 Synth
ch3 -> Temp Synth

The rest of the channels in the midifile are not used.

This is an early stage version of the feature. Right now none of the sensor data is linked to the synths.


It is possible to cycle through 4 different files on the SD card with the encoder like on the previous version 
of the biosynth ( Song of Seven ). The next file will load once the that is playing is done. The currently selected
file will play in loop until another one is selected


HOW TO MAKE IT WORK: 
 
 1. Make sure all the dependencies are installed 

        https://github.com/MajicDesigns/MD_MIDIFile
        https://github.com/SofaPirate/Chrono
        https://github.com/little-scale/mtof

2. Design your audio pipeline in the teensy audio design tool (https://www.pjrc.com/teensy/gui/index.html ) and 
    replace  the output code  in the file Global.h at the line 73 with your new pipeline. In order for this to work
    each synth that you want to be played by the midi file needs an envelope and they must be called ( envelopeHeart,
    envelopeGSR1 , envelopeGSR2 , envelopeTemp ) to be linked with the channels in the midi file.

3. Write the code to setup the initial state of the synths and the envelopes. You can replace te code in the function
   setupSounds() to set all the synths of the performance to their default states. If you want to initialize some envelopes
   with specifics ADSR timmings modify the setupEnvelopes() function. Both functions can be found in the Helpers.h file
   at the line 186 ( setupSounds() ) and 224 ( setupEnvelopes() ).

4. Place your midi files at the root of the SD card. There can be more than 4 midi files in the SD card but at the moment
   you can only switch between the 4 midi files you'll select on the next step.

5.  In order to be able to play / select midi files from the SD card you'll have to add their name in the array at 
    the line 37 in the file Global.h . It's easier if the files have simple names. Don't forget to add the .mid extension. 
    
    !*!*!*! The order in which the filenames are written in the array is the order in which you'll be able to select them.
    !*!*!*! The first name written will be the first file that will play.

6.  To link the information extracted from the midi file you'll need to go in the Biosynth.h file in the playNote() metodh
    at the line 528. The function receives the channel, the note number and its velocity as parameters. You'll have to 
    link the name of each synth receiving information from the coresponding channel. 
        ex: 
            the audio object Synth1 need to play the note comming from the ch3 in the midifile.

            in the 4th case in the switch statement in the playNote function. replace the code that is There
            with: 

            Synth1.frequency((float)mtof.toFrequency(midiNote))) //the argument here converts the midi note number in its corresponding frequency
            Synth1.amplitude(thinsToDOWithTheAmplitude)

            Do not remove the envelopeXXXX.noteOn() because it is what triggers the sound.


Make sure the Midi files created only have 1 track

This is v1 of this feature. It is not the most straight forward use it could be but it will be improved. Feel free to 
take note of what you'd want to modify in this workflow.


TODO : 
    .Add name of the selected file on screen
    .Autopopulate the tuneList array reading all the files contained in a folder
    .Make it possible to have more than 4 files per performances
    .Add a blinking metronome on the screen??? Do we want that???
    .Find a better way to link the synth to the note comming from the midi file
    .Comment the code
    
