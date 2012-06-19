// Monophonic Synth Multiplier
//-----------------------------------------------------------------------------------------

#include "timeshare.h"
#include "math.h"

//-----------------------------------------------------------------------------------------
// VstXSynth
//-----------------------------------------------------------------------------------------
void VstXSynth::setSampleRate (float sampleRate)
{
    AudioEffectX::setSampleRate (sampleRate);
    samplingFreq = sampleRate;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setBlockSize (VstInt32 blockSize)
{
    AudioEffectX::setBlockSize (blockSize);
    myBlockSize = blockSize;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::initProcess ()
{
// we don't know the sample rate yet
// sliceFreq = 32
// sliceTime = 1/sliceFreq = 31.25ms
// samplingFreq = 44.1 KHz
// blockSIze = 512 samples
// sliceSize = samplingFreq / sliceFreq = +- 1378    VstInt32 i;
//  VstInt32 memSize = 10*sliceSize;
    singleNoteLevel = (float) 0.5f;
    last = 0; // Last used postition in our Memory
    lastNotesOn = 0;
    currentNotesOn = 0;
    nextNoteToSend = 0;
    for (int i=0; i < JMSM_POLYPHONY; ++i)
    {
        noteList[i]=0;
        noteStatus[i]=false;
    }

    VstInt32 i;
    // Init Filter
    for (i=0; i<JOHAMMOND_FILTER_SIZE; ++i)
        history[i]=0;
    coef[0]=0.0923178;
    coef[1]=0.0943105;
    coef[2]=0.1229104;
    coef[3]=0.1412244;
    coef[4]=0.1452334;
    coef[5]=0.1342807;
    coef[6]=0.1112105;
    coef[7]=0.0814226;
    coef[8]=0.0511975;
    coef[9]=0.0258920;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* out1 = outputs[0];
    float* out2 = outputs[1];
    float *input1 = inputs[0];
    float *input2 = inputs[1];
    int MIDIposition = 0;
    int i = 0;
    int j = 0;
    static VstEvents eList;

    // Logic for MIDI control

    if (sliceCounter > sampleFrames)
        sliceCounter = sliceCounter - sampleFrames;
    else
    {
        MIDIposition = sliceCounter;
        if (currentNotesOn > 1 || (lastNotesOn ==0 && currentNotesOn==1))
        {
            // create MIDI event
            eList.numEvents = 1;
            eList.reserved = 0;
            eList.events[0] = noteCreate(getNextNote(),true,MIDIposition);
            eList.events[1] = NULL;
            // send to the Host
            sendVstEventsToHost(&eList);
        }
        if (currentNotesOn == 0)
        {
            // create MIDI event
            eList.numEvents = 1;
            eList.reserved = 0;
            eList.events[0] = noteCreate(getNextNote(),false,MIDIposition);
            eList.events[1] = NULL;
            // send to the Host
            sendVstEventsToHost(&eList);
        }
        sliceCounter = sliceCounter - sampleFrames + sliceSize;
    }

    // Audio Multiplication

    for (i=0; i<MIDIposition; ++i)
    {
        last = (last + 1) % memSize;
        MSMemory1[last] = input1[i];
        MSMemory2[last] = input2[i];
        out1[i] = 0;
        out2[i] = 0;
        for (j=0; j<=lastNotesOn; ++j)
            out1[i] += MSMemory1[((last - j*sliceSize)+memSize)%memSize] *singleNoteLevel;
        for (j=0; j<=lastNotesOn; ++j)
            out2[i] += MSMemory2[((last - j*sliceSize)+memSize)%memSize] *singleNoteLevel;
    }
    for (i=MIDIposition; i<sampleFrames; ++i)
    {
        last = (last + 1) %memSize;
        MSMemory1[last] = input1[i];
        MSMemory2[last] = input2[i];
        out1[i] = 0;
        out2[i] = 0;
        for (j=0; j<=currentNotesOn; ++j)
            out1[i] += MSMemory1[((last - j*sliceSize)+memSize)%memSize] *singleNoteLevel;
        for (j=0; j<=currentNotesOn; ++j)
            out2[i] += MSMemory2[((last - j*sliceSize)+memSize)%memSize] *singleNoteLevel;
    }

    lastNotesOn = currentNotesOn;
}

//-----------------------------------------------------------------------------------------
float VstXSynth::filter (float input)
{
    int n = JOHAMMOND_FILTER_SIZE;
    int i;
    float *hist_ptr,*hist1_ptr,*coef_ptr;
    float output;

    hist_ptr = history;
    hist1_ptr = hist_ptr; /* use for history update */
    coef_ptr = coef + n - 1; /* point to last coef */

    /* form output accumulation */
    output = *hist_ptr++ * (*coef_ptr--);

    for(i = 2 ; i < n ; i++)
    {
        *hist1_ptr++ = *hist_ptr; /* update history array */
        output += (*hist_ptr++) * (*coef_ptr--);
    }

    output += input * (*coef_ptr); /* input tap */
    *hist1_ptr = input; /* last history */
    return(output);
}

//-----------------------------------------------------------------------------------------
VstInt32 VstXSynth::processEvents (VstEvents* ev)
{
    for (VstInt32 i = 0; i < ev->numEvents; i++)
    {
        if ((ev->events[i])->type != kVstMidiType)
            continue;

        VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
        char* midiData = event->midiData;
        VstInt32 status = midiData[0] & 0xf0;    // ignoring channel
        if (status == 0x90 || status == 0x80)    // we only look at notes
        {
            VstInt32 note = midiData[1] & 0x7f;
            VstInt32 velocity = midiData[2] & 0x7f;
            if (velocity!=0 && status == 0x90)
                noteOn (note, velocity, event->deltaFrames);
            else
                noteOff(note);
        }
        else if (status == 0xb0)
        {
            if (midiData[1] == 0x7e || midiData[1] == 0x7b)    // all notes off
                allNotesOff ();
        }
        event++;
    }
    return 1;
}

//-----------------------------------------------------------------------------------------
VstEvent * VstXSynth::noteCreate (VstInt32 note,bool noteon,VstInt32 delta)
{
    static VstMidiEvent event;

    //Create first event
    event.type=kVstMidiType;
    event.byteSize=24L;
    event.deltaFrames=delta;  //; here goes the sync
    event.flags=0L;
    event.noteLength=0L;
    event.noteOffset=0L;

    if (noteon)
        event.midiData[0]=(char)0x90;    //status & channel CHANNEL 1 NOTEON
    else
        event.midiData[0]=(char)0x80;    //status & channel CHANNEL 1 NOTEOFF
    event.midiData[1]=(char)note;    // note number
    event.midiData[2]=(char)127;    // velocity
    event.midiData[3]=(char)0x00;    //MIDI byte #4 - blank

    event.detune=0x00;
    event.noteOffVelocity=0x00;
    event.reserved1=0x00;
    event.reserved2=0x00;

    return (VstEvent*) &event;
}


//-----------------------------------------------------------------------------------------
void VstXSynth::noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta)
{
    // JMSM
    ++currentNotesOn;
    for (int i=0; i<JMSM_POLYPHONY; ++i)
        if (noteStatus[i] == false)
        {
            noteList[i]= note;
            noteStatus[i]= true;
            break;
        }
    /*
    noteList.push_back(note);
    */
}

//-----------------------------------------------------------------------------------------
void VstXSynth::noteOff (VstInt32 note)
{
    // JMSM
    --currentNotesOn;
    for (int i=0; i<JMSM_POLYPHONY; ++i)
        if (noteList[i] == note)
            noteStatus[i]= false;
    /*
    vector<VstInt32>::iterator i;
    for (i=noteList.begin(); i!=noteList.end(); i++)
      {
      if ((*i) == note)
        {
        //noteList.erase(i);
        break;
        }
      }
      */
}

//-----------------------------------------------------------------------------------------
void VstXSynth::allNotesOff ( )
{
    // JMSM
    currentNotesOn = 0;
    for (int i=0; i<JMSM_POLYPHONY; ++i)
        noteStatus[i]= false;
}

//-----------------------------------------------------------------------------------------
int VstXSynth::getNextNote ()
{
    // JMSM
    int i=(nextNoteToSend + 1)%JMSM_POLYPHONY;
    for (; i != nextNoteToSend; i = (i+1)%JMSM_POLYPHONY)
        if (noteStatus[i] == true)
        {
            nextNoteToSend = i;
            return noteList[i];
        }
    nextNoteToSend = i;
    return noteList[i];
}
