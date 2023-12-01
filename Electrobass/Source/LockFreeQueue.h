/*
  ==============================================================================

    LockFreeQueue.h
    Created: 30 Nov 2023 12:46:57pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class LockFreeQueue
{
    
public:
    ScopedPointer<AbstractFifo> lockFreeFifo;
    Array<float> data;
    int lastReadPos = 0;
    
    LockFreeQueue()  {
        lockFreeFifo = new AbstractFifo(512);
        data.ensureStorageAllocated(512);
        FloatVectorOperations::clear(data.getRawDataPointer(), 512);
        
        while(data.size() < 512)
        {
            data.add(0.f);
        }
    }
//    void setTotalSize(in newSize){
//        lockFreeFi
//    }
    
    void writeTo(const float* writeData, int numToWrite){
        auto writeHandle = lockFreeFifo->write(numToWrite);
        if (writeHandle.blockSize1 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + writeHandle.startIndex1, writeData, writeHandle.blockSize1);
        if (writeHandle.blockSize2 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + writeHandle.startIndex2, writeData + writeHandle.blockSize1, writeHandle.blockSize2);
        
    }
    
    void readFrom (float* readData, int numToRead) {
        
        auto readHandle = lockFreeFifo->read(numToRead);

        if(readHandle.blockSize1 > 0)
        {
            FloatVectorOperations::copy(readData, data.getRawDataPointer() + readHandle.startIndex1, readHandle.blockSize1);
            lastReadPos = readHandle.startIndex1 + readHandle.blockSize1;
        }
        
        if (readHandle.blockSize2 > 0)
        {
            FloatVectorOperations::copy(readData + readHandle.blockSize1, data.getRawDataPointer() + readHandle.startIndex2, readHandle.blockSize2);
            lastReadPos = readHandle.startIndex2 + readHandle.blockSize2;
        }
        
    }
    
    void readMostRecent(float * readData, int numToRead) 
    {
        
    }
    
    int writeToArray(float *dest)
    {
//        //drain the excess
//        while (getNumReady() > dest->size())
//        {
//            lockFreeFifo->finishedRead(getNumReady() - dest->size());
//        }
        
        //read latest data from queue
        const int numToAppend = getNumReady();
        
        // add the tail to the output
        
        if(numToAppend)
        {
            readFrom(dest, numToAppend);
        }
        
        return numToAppend;
    }
    
    int getNumReady()
    {
        return lockFreeFifo->getNumReady();
    }
};
