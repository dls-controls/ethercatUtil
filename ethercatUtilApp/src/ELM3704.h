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
#include <asynPortClient.h>


class ELM3704 : public asynPortDriver {

public:
    // Constructor
    ELM3704(const char* portName, const char* sdoPortName);

    // Overidden methods from asynPortDriver
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);

protected:
    // Channel asyn parameter indices
    int measurementType[4];
    int measurementSubType[4];
    int measurementTypeLoaded[4];

    // Measurement type enum
    enum Type {
        None,
        Voltage,
        Current,
        Potentiometer,
        Thermocouple,
        IEPE,
        StrainGaugeFullBridge,
        StrainGaugeHalfBridge,
        StrainGaugeQuarterBridge2Wire,
        StrainGaugeQuarterBridge3Wire,
        RTD
    };

private:
    // Methods for updating the MBBI/O subtype options
    void writeNoneSubTypeOptions(const unsigned int &channel);
    void writeVoltageSubTypeOptions(const unsigned int &channel);
    void writeCurrentSubTypeOptions(const unsigned int &channel);


    asynStatus setChannelInterface(const unsigned int &channel, const unsigned int &value);
    bool checkIfMeasurementTypeChanged(const int &param, const epicsInt32 &value);
    bool checkIfMeasurementSubTypeChanged(const int &param, const epicsInt32 &value);

    // Variables
    asynPortClient sdoPortClient;

};


#endif /* DIRECTORY_MANAGER_H */
