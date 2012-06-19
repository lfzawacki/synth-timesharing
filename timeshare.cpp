// Monophonic Synth Multiplier
//-------------------------------------------------------------------------------------------------------

#include "timeshare.h"

//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
    return new VstXSynth (audioMaster);
}

//-----------------------------------------------------------------------------------------
// VstXSynthProgram
//-----------------------------------------------------------------------------------------
VstXSynthProgram::VstXSynthProgram ()
{
    fDrawbar32 = 0.0f;
    fDrawbar16 = 0.0f;
    fDrawbar8  = 0.0f;
    fDrawbar4  = 0.0f;
    fDrawbar223 = 0.0f;
    fDrawbar2   = 0.0f;
    fDrawbar135 = 0.0f;
    fDrawbar113   = 0.0f;
    fDrawbar1 = 0.0f;

    fVolume    = .9f;
    vst_strncpy (name, "Basic", kVstMaxProgNameLen);
}

VstPlugCategory VstXSynth::getPlugCategory( )
{
    return kPlugCategEffect;
}

//-----------------------------------------------------------------------------------------
// VstXSynth
//-----------------------------------------------------------------------------------------
VstXSynth::VstXSynth (audioMasterCallback audioMaster)
    : AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
    // initialize programs
    programs = new VstXSynthProgram[kNumPrograms];
    for (VstInt32 i = 0; i < 16; i++)
        channelPrograms[i] = i;

    if (programs)
        setProgram (0);

    if (audioMaster)
    {
        setNumInputs (kNumInputs);        // monophonic input
        setNumOutputs (kNumOutputs);    // stereo outputs (why?)
        canProcessReplacing ();
        //isSynth ();
        setUniqueID ('J');            // <<<! *must* change this!!!!
    }

    initProcess ();
    suspend ();
}

//-----------------------------------------------------------------------------------------
VstXSynth::~VstXSynth ()
{
    if (programs)
        delete[] programs;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setProgram (VstInt32 program)
{
    if (program < 0 || program >= kNumPrograms)
        return;

    VstXSynthProgram *ap = &programs[program];
    curProgram = program;

    fDrawbar32 = ap->fDrawbar32;
    fDrawbar16 = ap->fDrawbar16;
    fDrawbar8  = ap->fDrawbar8;
    fDrawbar4  = ap->fDrawbar4;
    fDrawbar223 = ap->fDrawbar223;
    fDrawbar2   = ap->fDrawbar2;
    fDrawbar135 = ap->fDrawbar135;
    fDrawbar113 = ap->fDrawbar113;
    fDrawbar1 = ap->fDrawbar1;

    fVolume    = ap->fVolume;
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setProgramName (char* name)
{
    vst_strncpy (programs[curProgram].name, name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getProgramName (char* name)
{
    vst_strncpy (name, programs[curProgram].name, kVstMaxProgNameLen);
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterLabel (VstInt32 index, char* label)
{
    switch (index)
    {
        case kDrawbar32:
        case kDrawbar16:
        case kDrawbar8:
        case kDrawbar4:
        case kDrawbar223:
        case kDrawbar2:
        case kDrawbar135:
        case kDrawbar113:
        case kDrawbar1:
            vst_strncpy (label, "stop", kVstMaxParamStrLen);
            break;
    }
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterDisplay (VstInt32 index, char* text)
{
    text[0] = 0;
    switch (index)
    {
        case kDrawbar32:
            float2string (fDrawbar32, text, kVstMaxParamStrLen);
            break;
        case kDrawbar16:
            float2string (fDrawbar16, text, kVstMaxParamStrLen);
            break;
        case kDrawbar8:
            float2string (fDrawbar8, text, kVstMaxParamStrLen);
            break;
        case kDrawbar4:
            float2string (fDrawbar4, text, kVstMaxParamStrLen);
            break;
        case kDrawbar223:
            float2string (fDrawbar223, text, kVstMaxParamStrLen);
            break;
        case kDrawbar2:
            float2string (fDrawbar2, text, kVstMaxParamStrLen);
            break;
        case kDrawbar135:
            float2string (fDrawbar135, text, kVstMaxParamStrLen);
            break;
        case kDrawbar113:
            float2string (fDrawbar113, text, kVstMaxParamStrLen);
            break;
        case kDrawbar1:
            float2string (fDrawbar1, text, kVstMaxParamStrLen);
            break;
        case kVolume:
            dB2string (fVolume, text, kVstMaxParamStrLen);
            break;
    }
}

//-----------------------------------------------------------------------------------------
void VstXSynth::getParameterName (VstInt32 index, char* label)
{
    switch (index)
    {
        case kDrawbar32:
            vst_strncpy (label, "32\'", kVstMaxParamStrLen);
            break;
        case kDrawbar16:
            vst_strncpy (label, "16\'", kVstMaxParamStrLen);
            break;
        case kDrawbar8:
            vst_strncpy (label, "8\'", kVstMaxParamStrLen);
            break;
        case kDrawbar4:
            vst_strncpy (label, "4\'", kVstMaxParamStrLen);
            break;
        case kDrawbar223:
            vst_strncpy (label, "2\'2/3", kVstMaxParamStrLen);
            break;
        case kDrawbar2:
            vst_strncpy (label, "2\'", kVstMaxParamStrLen);
            break;
        case kDrawbar135:
            vst_strncpy (label, "1\'3/5", kVstMaxParamStrLen);
            break;
        case kDrawbar113:
            vst_strncpy (label, "1\'1/3", kVstMaxParamStrLen);
            break;
        case kDrawbar1:
            vst_strncpy (label, "1\'", kVstMaxParamStrLen);
            break;
        case kVolume:
            vst_strncpy (label, "Volume", kVstMaxParamStrLen);
            break;
    }
}

//-----------------------------------------------------------------------------------------
void VstXSynth::setParameter (VstInt32 index, float value)
{
    VstXSynthProgram *ap = &programs[curProgram];
    switch (index)
    {
    case kDrawbar32:
        fDrawbar32 = ap->fDrawbar32 = value;
        break;
    case kDrawbar16:
        fDrawbar16 = ap->fDrawbar16    = value;
        break;
    case kDrawbar8:
        fDrawbar8 = ap->fDrawbar8 = value;
        break;
    case kDrawbar4:
        fDrawbar4 = ap->fDrawbar4 = value;
        break;
    case kDrawbar223:
        fDrawbar223  = ap->fDrawbar223 = value;
        break;
    case kDrawbar2:
        fDrawbar2 = ap->fDrawbar2 = value;
        break;
    case kDrawbar135:
        fDrawbar135 = ap->fDrawbar135 = value;
        break;
    case kDrawbar113:
        fDrawbar113 = ap->fDrawbar113 = value;
        break;
    case kDrawbar1:
        fDrawbar1 = ap->fDrawbar1 = value;
        break;
    case kVolume:
        fVolume = ap->fVolume = value;
        break;
    }
}

//-----------------------------------------------------------------------------------------
float VstXSynth::getParameter (VstInt32 index)
{
    float value = 0;
    switch (index)
    {
        case kDrawbar32:
            value = fDrawbar32;
            break;
        case kDrawbar16:
            value = fDrawbar16;
            break;
        case kDrawbar8:
            value = fDrawbar8;
            break;
        case kDrawbar4:
            value = fDrawbar4;
            break;
        case kDrawbar223:
            value = fDrawbar223;
            break;
        case kDrawbar2:
            value = fDrawbar2;
            break;
        case kDrawbar135:
            value = fDrawbar135;
            break;
        case kDrawbar113:
            value = fDrawbar113;
            break;
        case kDrawbar1:
            value = fDrawbar1;
            break;
        case kVolume:
            value = fVolume;
            break;
    }
    return value;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getOutputProperties (VstInt32 index, VstPinProperties* properties)
{
    if (index < kNumOutputs)
    {
        vst_strncpy (properties->label, "Vstx ", 63);
        char temp[11] = {0};
        int2string (index + 1, temp, 10);
        vst_strncat (properties->label, temp, 63);

        properties->flags = kVstPinIsActive;
        if (index < 2)
            properties->flags |= kVstPinIsStereo;    // make channel 1+2 stereo
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text)
{
    if (index < kNumPrograms)
    {
        vst_strncpy (text, programs[index].name, kVstMaxProgNameLen);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getEffectName (char* name)
{
    vst_strncpy (name, "Synth TimeSharing", kVstMaxEffectNameLen);
    return true;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getVendorString (char* text)
{
    vst_strncpy (text, "UFRGS Audio Lab", kVstMaxVendorStrLen);
    return true;
}

//-----------------------------------------------------------------------------------------
bool VstXSynth::getProductString (char* text)
{
    vst_strncpy (text, "Monophonic Synth Multiplier 9000", kVstMaxProductStrLen);
    return true;
}

//-----------------------------------------------------------------------------------------
VstInt32 VstXSynth::getVendorVersion ()
{
    return 1000;
}

//-----------------------------------------------------------------------------------------
VstInt32 VstXSynth::canDo (char* text)
{
    if (!strcmp (text, "receiveVstEvents"))
        return 1;
    if (!strcmp (text, "sendVstMidiEvent"))
        return 1;
    if (!strcmp (text, "receiveVstMidiEvent"))
        return 1;
    if (!strcmp (text, "midiProgramNames"))
        return 1;
    return -1;
}

//-----------------------------------------------------------------------------------------
VstInt32 VstXSynth::getNumMidiInputChannels ()
{
    return 1; // it is a single instrument
}

//-----------------------------------------------------------------------------------------
VstInt32 VstXSynth::getNumMidiOutputChannels ()
{
    return 1; // it is a single instrument

}

//------------------------------------------------------------------------
VstInt32 VstXSynth::getMidiProgramName (VstInt32 channel, MidiProgramName* mpn)
{
    VstInt32 prg = mpn->thisProgramIndex;
    if (prg < 0 || prg >= 128)
        return 0;
    fillProgram (channel, prg, mpn);
    if (channel == 9)
        return 1;
    return 128L;
}

//------------------------------------------------------------------------
VstInt32 VstXSynth::getCurrentMidiProgram (VstInt32 channel, MidiProgramName* mpn)
{
    if (channel < 0 || channel >= 16 || !mpn)
        return -1;
    VstInt32 prg = channelPrograms[channel];
    mpn->thisProgramIndex = prg;
    fillProgram (channel, prg, mpn);
    return prg;
}

//------------------------------------------------------------------------
void VstXSynth::fillProgram (VstInt32 channel, VstInt32 prg, MidiProgramName* mpn)
{
}

//------------------------------------------------------------------------
VstInt32 VstXSynth::getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* cat)
{
    return 1;
}

//------------------------------------------------------------------------
bool VstXSynth::hasMidiProgramsChanged (VstInt32 channel)
{
    return false;
}

//------------------------------------------------------------------------
bool VstXSynth::getMidiKeyName (VstInt32 channel, MidiKeyName* key)
{
    key->keyName[0] = 0;
    key->reserved = 0;
    key->flags = 0;
    return false;
}
