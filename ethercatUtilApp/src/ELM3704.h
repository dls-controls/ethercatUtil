/*
 * ELM3704.h
 *
 * Benjamin Bradnick
 *
 * Class for handling the logic for configuring the ELM3704 multi-purpose
 * channels.
 *
 */

#ifndef DIRECTORY_MANAGER_H
#define DIRECTORY_MANAGER_H


#include "asynPortDriver.h"


class ELM3704 : public asynPortDriver {

public:
    // Constructor
    ELM3704(const char* portName);

    // Overidden methods from asynPortDriver
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);

protected:
    // Channel asyn parameter indices
    int measurementType[4];
    int measurementSubType[4];
    int measurementTypeLoaded[4];

private:
    // Methods
    void writeVoltageStrings(const unsigned int &channel);
    void writeNoneStrings(const unsigned int &channel);

};


#endif /* DIRECTORY_MANAGER_H */
