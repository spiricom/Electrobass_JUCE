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
        int start1, start2, blockSize1, blockSize2;
        
        lockFreeFifo->prepareToWrite(numToWrite, start1, blockSize1, start2, blockSize2);
        
        if (blockSize1 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + start1, writeData, blockSize1);
        if (blockSize2 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + start2, writeData + blockSize1, blockSize2);
        
        // move fif write head
        lockFreeFifo->finishedWrite(numToWrite);
    }
    
    void readFrom (float* readData, int numToRead) {
        int start1, blockSize1, start2, blockSize2;
        lockFreeFifo->prepareToRead(numToRead, start1, blockSize1, start2, blockSize2);
        if(blockSize1 > 0)
        {
            FloatVectorOperations::copy(readData, data.getRawDataPointer() + start1, blockSize1);
            lastReadPos = start1 + blockSize1;
        }
        
        if (blockSize2 > 0)
        {
            FloatVectorOperations::copy(readData + blockSize1, data.getRawDataPointer() + start2, blockSize2);
            lastReadPos = start2 + blockSize2;
        }
        
        lockFreeFifo->finishedRead(blockSize1 + blockSize2);
    }
    
    void readMostRecent(float * readData, int numToRead) 
    {
        
    }
    
    int writeToArray(Array<float>* dest, int destPos)
    {
        //drain the excess
        while (getNumReady() > dest->size())
        {
            lockFreeFifo->finishedRead(getNumReady() - dest->size());
        }
        
        //read latest data from queue
        const int numToAppend = getNumReady();
        
        // add the tail to the output
        
        if(destPos + numToAppend < dest->size())
        {
            readFrom(&dest->getRawDataPointer()[destPos], numToAppend);
        }
        else
        {
            int toTheEnd = dest->size() - destPos;
            readFrom(&dest->getRawDataPointer()[destPos], toTheEnd);
            readFrom(&dest->getRawDataPointer()[0], numToAppend - toTheEnd);
        }
        
    }
    
    int getNumReady()
    {
        return lockFreeFifo->getNumReady();
    }
};
