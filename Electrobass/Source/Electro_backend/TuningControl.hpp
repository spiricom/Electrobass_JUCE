//
//  Microtonal.hpp
//  Electrobass
//
//  Created by Davis Polito on 1/20/22.
//  Copyright © 2022 Snyderphonics. All rights reserved.
//

#ifndef TuningControl_hpp
#define TuningControl_hpp

#include "MTS-ESP/Client/libMTSClient.h"
#include "Utilities.h"
#include "tuning-library/include/Tunings.h"

#include <stdio.h>
/*
 * This class uses a member pointer. To avoid querying which mtof function to use within the process loop
 *  https://web.archive.org/web/20120723071248/http://www.dulcineatech.com/tips/code/c++/member-pointers.html
 */
class TuningControl
{
public:
    TuningControl() : client(nullptr), isMTS(false)
    {
        mtofptr = setMtoFFunction(isMTS);
        currentScale = Tunings::evenTemperament12NoteScale();
        currentKBM = Tunings::KeyboardMapping();
    }
    ~TuningControl()
    {
        if(MTS_HasMaster(client))
            MTS_DeregisterClient(client);
    }
    String loadScala(std::string fname, float* arr);
    String loadKBM(std::string fname, float* arr);
    void setIsMTS(bool f)
    {
        isMTS = f;
        MTSOnOff();
        mtofptr = setMtoFFunction(isMTS);
    };
    float mtof (float mn,float* mtofTable);
    auto const getIsMTS() {return isMTS;};
    String getCurrentScalaString() {
        return String(currentScale.rawText);
    }
    
    String getCurrentKBMString() {
        return String(currentKBM.rawText);
    }
    
    String resetKBM(float *arr);
    
    String resetSCL(float *arr);

private:
    void loadTuning(float *arr);
    typedef float (TuningControl::*MidiToFreq)(float,float*);
    static MidiToFreq setMtoFFunction(bool);
    MidiToFreq mtofptr;
    float _MTS_mtof(float mn,float* mtofTable);
    float _mtof(float mn,float* mtofTable);
    MTSClient *client;
    bool isMTS;
    void MTSOnOff();
    Tunings::Scale currentScale;
    Tunings::KeyboardMapping currentKBM;
    

};
#endif /* TuningControl_hpp */
