//
//  TuningControl.cpp
//  Electrobass
//
//  Created by Davis Polito on 1/20/22.
//  Copyright © 2022 Snyderphonics. All rights reserved.
//

#include "TuningControl.hpp"
TuningControl::MidiToFreq TuningControl::setMtoFFunction(bool isMTSOn)
{
    if (isMTSOn)
    {
        return &TuningControl::_MTS_mtof;
    }
    else
    {
        return &TuningControl::_mtof;
    }
}

float TuningControl::mtof(float mn, float* mtofTable)
{
    return (this->*mtofptr)(mn, mtofTable);
}

float TuningControl::_mtof(float f, float* mtofTable)
{

   
    float tempIndexF =((f * 100.0f) + 16384.0f);
    int tempIndexI = (int)tempIndexF;
    tempIndexF = tempIndexF -tempIndexI;

    float freqToSmooth1 = mtofTable[tempIndexI & 32767];
    float freqToSmooth2 = mtofTable[(tempIndexI + 1) & 32767];
    float finalFreq = (freqToSmooth1 * (1.0f - tempIndexF)) + (freqToSmooth2 * tempIndexF);
    return finalFreq;
    
}



float TuningControl::_MTS_mtof(float mn, float *mtofTable)
{
    return MTS_NoteToFrequency(client, (char)mn, -1);
}
void TuningControl::MTSOnOff()
{
    if (isMTS && (!MTS_HasMaster(client) || client == nullptr))
    {
        client = MTS_RegisterClient();
    } else if (!isMTS && MTS_HasMaster(client))
    {
        //Deregister just deletes the client so we must set it to null so that we know it is deleted;
        MTS_DeregisterClient(client);
        client = nullptr;
    }
}
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SCALA READING ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
String TuningControl::loadScala(std::string fname, float* arr)
{
    Tunings::Scale s;
    try {
        s = Tunings::readSCLFile(fname);
    } catch (Tunings::TuningError t) {
        AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Scala Loading Error"),TRANS(t.what()));
        return String(currentScale.rawText);
    }
    currentScale = s;
    try
    {
        loadTuning(arr);
    } catch (Tunings::TuningError t)
    {
        AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"),TRANS(t.what()));
        currentScale = Tunings::evenTemperament12NoteScale();
    }
    return String(currentScale.rawText);
}
String TuningControl::loadKBM(std::string fname, float* arr)
{
    Tunings::KeyboardMapping k;
    try {
        k = Tunings::readKBMFile(fname);
    } catch (Tunings::TuningError t) {
        AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("KBM Loading Error"),TRANS(t.what()));
        return String(currentKBM.rawText);
    }
    currentKBM = k;
    try
    {
        loadTuning(arr);
    } catch (Tunings::TuningError t)
    {
        AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, TRANS("Error"),TRANS(t.what()));
        currentKBM = Tunings::KeyboardMapping();
    }
    return String(currentKBM.rawText);
}
void TuningControl::loadTuning(float *arr)
{
    Tunings::Tuning t(currentScale,currentKBM);

    for (int i = 0; i < NUM_MIDI_NOTES; i++)
    {
        arr[i] = ftom(t.frequencyForMidiNote(i));
        DBG(String(i) + ":" + String(arr[i]));
    }
}
String TuningControl::resetKBM(float *arr)
{
    currentKBM = Tunings::KeyboardMapping();
    loadTuning(arr);
    return currentKBM.rawText;
}

String TuningControl::resetSCL(float *arr)
{
    currentScale = Tunings::evenTemperament12NoteScale();
    loadTuning(arr);
    return currentScale.rawText;
}
