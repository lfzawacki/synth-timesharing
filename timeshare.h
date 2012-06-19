// Monophonic Synth Multiplier
//-------------------------------------------------------------------------------------------------------

#ifndef __JohannMSM__
#define __JohannMSM__
#define JOHAMMOND_FILTER_SIZE 10

#include "audioeffectx.h"
#include <vector>

using namespace std;

//------------------------------------------------------------------------------------------
enum
{
    // Global
    JMSM_POLYPHONY = 12,
    sliceSize = 3036,	// was 1536, roughly 1/32 sec for 44.1KHz
    memSize = 35362,		// gbreater than 10 times sliceSize

    kNumPrograms = 128,
    kNumInputs = 3,
    kNumOutputs = 2,

    // Parameters Tags
    kDrawbar32 = 0, // This identifies it as the first slider
    kDrawbar16,
    kDrawbar8,
    kDrawbar4,
    kDrawbar223,
    kDrawbar2,
    kDrawbar135,
    kDrawbar113,
    kDrawbar1,

    kVolume,

    kNumParams
};

//------------------------------------------------------------------------------------------
// VstXSynthProgram
//------------------------------------------------------------------------------------------
class VstXSynthProgram
{
    friend class VstXSynth;
public:
    VstXSynthProgram ();
    ~VstXSynthProgram () {}

private:
    float fDrawbar32;
    float fDrawbar16;
    float fDrawbar8;
    float fDrawbar4;
    float fDrawbar223;
    float fDrawbar2;
    float fDrawbar135;
    float fDrawbar113;
    float fDrawbar1;

    float fVolume;
    char name[kVstMaxProgNameLen+1];
};

//------------------------------------------------------------------------------------------
// VstXSynth
//------------------------------------------------------------------------------------------
class VstXSynth : public AudioEffectX
{
public:
    VstXSynth (audioMasterCallback audioMaster);
    ~VstXSynth ();

    virtual VstPlugCategory getPlugCategory	( );

    virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
    virtual VstInt32 processEvents (VstEvents* events);

    virtual void setProgram (VstInt32 program);
    virtual void setProgramName (char* name);
    virtual void getProgramName (char* name);
    virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);

    virtual void setParameter (VstInt32 index, float value);
    virtual float getParameter (VstInt32 index);
    virtual void getParameterLabel (VstInt32 index, char* label);
    virtual void getParameterDisplay (VstInt32 index, char* text);
    virtual void getParameterName (VstInt32 index, char* text);

    virtual void setSampleRate (float sampleRate);
    virtual void setBlockSize (VstInt32 blockSize);

    virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);

    virtual bool getEffectName (char* name);
    virtual bool getVendorString (char* text);
    virtual bool getProductString (char* text);
    virtual VstInt32 getVendorVersion ();
    virtual VstInt32 canDo (char* text);

    virtual VstInt32 getNumMidiInputChannels ();
    virtual VstInt32 getNumMidiOutputChannels ();

    virtual VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName);
    virtual VstInt32 getCurrentMidiProgram (VstInt32 channel, MidiProgramName* currentProgram);
    virtual VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category);
    virtual bool hasMidiProgramsChanged (VstInt32 channel);
    virtual bool getMidiKeyName (VstInt32 channel, MidiKeyName* keyName);

private:
    float fDrawbar32;
    float fDrawbar16;
    float fDrawbar8;
    float fDrawbar4;
    float fDrawbar223;
    float fDrawbar2;
    float fDrawbar135;
    float fDrawbar113;
    float fDrawbar1;
    float fVolume;

    VstXSynthProgram* programs;
    VstInt32 channelPrograms[16];

    void initProcess ();
    void noteOn (VstInt32 note, VstInt32 velocity, VstInt32 delta);
    void noteOff (VstInt32 note);
    void allNotesOff ();
    int getNextNote ();
    VstEvent *noteCreate (VstInt32 note,bool noteon,VstInt32 delta);
    void fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn);
    float filter (float origin);
    float coef[JOHAMMOND_FILTER_SIZE];
    float history[JOHAMMOND_FILTER_SIZE];

    // Johann MSM Parameters
    float SliceFreq;
    float sliceTime;
    float samplingFreq ;
    float MSMemory1[memSize];
    float MSMemory2[memSize];
    VstInt32 myBlockSize;
    VstInt32 sliceCounter;
    // VstInt32 memSize;
    float singleNoteLevel;
    VstInt32 last;
    VstInt32 lastNotesOn;
    VstInt32 currentNotesOn;
    int noteList[JMSM_POLYPHONY];
    bool noteStatus[JMSM_POLYPHONY];
    VstInt32 nextNoteToSend;
};

#endif

