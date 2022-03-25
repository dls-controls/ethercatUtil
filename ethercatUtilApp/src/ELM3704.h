/*
 * ELM3704.h
 *
 * Benjamin Bradnick
 *
 * Class for handling the logic for configuring the ELM3704 multi-purpose
 * channels.
 *
*/

#ifndef ELM3704_H
#define ELM3704_H

#include <thread>

#include "asynPortDriver.h"
#include "SdoPortClient.h"
#include <alarm.h>


class ELM3704 : public asynPortDriver
{

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
    int measurementSensorSupply[4];
    int measurementRTDElementPage[4];
    int measurementRTDElement[4];
    int measurementTCElementPage[4];
    int measurementTCElement[4];
    int measurementScaler[4];
    int channelStatusMessage[4];

    // Measurement type enum
    enum Type {
        None,
        Voltage,
        Current,
        Potentiometer,
        Thermocouple,
        IEPiezoElectric,
        StrainGaugeFullBridge,
        StrainGaugeHalfBridge,
        StrainGaugeQuarterBridge2Wire,
        StrainGaugeQuarterBridge3Wire,
        RTD,
    };

private:
    // Method to initialise values
    void initialiseValues();

    // Generic method to write a single N/A option to MBBI/MBBO record via asynParameter
    void writeNAOption(int param);

    // Methods for updating the MBBI/O subtype options when changing core type
    void writeNASubTypeOption(unsigned int channel);
    void writeVoltageSubTypeOptions(unsigned int channel);
    void writeCurrentSubTypeOptions(unsigned int channel);
    void writePotentiometerSubTypeOptions(unsigned int channel);
    void writeThermocoupleSubTypeOptions(unsigned int channel);
    void writeIEPESubTypeOptions(unsigned int channel);
    void writeStrainGaugeFBSubTypeOptions(unsigned int channel);
    void writeStrainGaugeHBSubTypeOptions(unsigned int channel);
    void writeStrainGaugeQB2WireSubTypeOptions(unsigned int channel);
    void writeStrainGaugeQB3WireSubTypeOptions(unsigned int channel);
    void writeRTDSubTypeOptions(unsigned int channel);

    // Methods for updating secondary settings
    void writeNASensorSupplyOption(unsigned int channel);
    void writeStrainGaugeSensorSupplyOptions(unsigned int channel);
    void writeIEPESensorySupplyOption(unsigned int channel);
    void writeNARTDElementPageOption(unsigned int channel);
    void writeRTDElementPageOptions(unsigned int channel);
    void writeNARTDElementOption(unsigned int channel);
    void writeRTDElementOptions(unsigned int channel, unsigned int page = 1);
    void writeNATCElementPageOption(unsigned int channel);
    void writeTCElementPageOptions(unsigned int channel);
    void writeNATCElementOption(unsigned int channel);
    void writeTCElementOptions(unsigned int channel, unsigned int page = 1);
    void writeDefaultScalerOptions(unsigned int channel);
    void writeThermocoupleScalerOptions(unsigned int channel);

    // Generic method for writing to the SDO port via asynPortClient
    asynStatus writeInt32SdoPortClient(const std::string &paramName, const epicsInt32 &value);

    // Methods for writing to the SDO port via the generic method above
    asynStatus setChannelParameter(unsigned int channel, const std::string &paramName, unsigned int value);
    asynStatus setChannelInterface(unsigned int channel, unsigned int value);
    asynStatus setChannelSensorSupply(unsigned int channel, unsigned int value);
    asynStatus setChannelRTDElement(unsigned int channel, unsigned int value);
    asynStatus setChannelTCElement(unsigned int channel, unsigned int value);
    asynStatus setChannelScaler(unsigned int channel, unsigned int value);

    // Methods for reading current measurement settings (e.g. after interface change)
    asynStatus readChannelSubSetting(unsigned int channel, const std::string &paramName, epicsInt32 &paramValue);
    asynStatus readCurrentChannelSubSettings(unsigned int channel);

    // Method to call after changing measurement type
    void setFirstSubTypeAfterTypeChanged(unsigned int channel, int value, const std::string &statusString);

    // Methods for handling asynParameter changes
    bool checkIfMeasurementTypeChanged(int param, const epicsInt32 &value);
    bool checkIfMeasurementSubTypeChanged(int param, const epicsInt32 &value);
    bool checkIfSensorSupplyOptionChanged(int param, const epicsInt32 &value);
    bool checkIfRTDPageChanged(int param, const epicsInt32 &value);
    bool checkIfRTDOptionChanged(int param, const epicsInt32 &value);
    bool checkIfTCPageChanged(int param, const epicsInt32 &value);
    bool checkIfTCOptionChanged(int param, const epicsInt32 &value);
    bool checkIfScalerOptionChanged(int param, const epicsInt32 &value);

    // Methods for special handling of asynParameter changes
    void checkIfSubTypeOptionChangingBetweenTCTypes(unsigned int channel, const epicsInt32 &value);

    // Method for updating channel status string
    void updateChannelStatusString(unsigned int channel, const std::string &string, const epicsAlarmSeverity &severity);

    // asynPortClient to talk to the SDO port when setting channel parameters
    SdoPortClient sdoPortClient;

    // Initialise values thread
    std::thread initialiseThread;

};


#endif /* ELM3704_H */
