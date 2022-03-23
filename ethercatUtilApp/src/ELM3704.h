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

    // Generic method to write a single N/A option to MBBI/MBBO record via asynParameter
    void writeNAOption(const int &param);

    // Methods for updating the MBBI/O subtype options when changing core type
    void writeNASubTypeOption(const unsigned int &channel);
    void writeVoltageSubTypeOptions(const unsigned int &channel);
    void writeCurrentSubTypeOptions(const unsigned int &channel);
    void writePotentiometerSubTypeOptions(const unsigned int &channel);
    void writeThermocoupleSubTypeOptions(const unsigned int &channel);
    void writeIEPESubTypeOptions(const unsigned int &channel);
    void writeStrainGaugeFBSubTypeOptions(const unsigned int &channel);
    void writeStrainGaugeHBSubTypeOptions(const unsigned int &channel);
    void writeStrainGaugeQB2WireSubTypeOptions(const unsigned int &channel);
    void writeStrainGaugeQB3WireSubTypeOptions(const unsigned int &channel);
    void writeRTDSubTypeOptions(const unsigned int &channel);

    // Methods for updating secondary settings
    void writeNASensorSupplyOption(const unsigned int &channel);
    void writeStrainGaugeSensorSupplyOptions(const unsigned int &channel);
    void writeIEPESensorySupplyOption(const unsigned int &channel);
    void writeNARTDElementPageOption(const unsigned int &channel);
    void writeRTDElementPageOptions(const unsigned int &channel);
    void writeNARTDElementOption(const unsigned int &channel);
    void writeRTDElementOptions(const unsigned int &channel, const unsigned int &page = 1);
    void writeNATCElementPageOption(const unsigned int &channel);
    void writeTCElementPageOptions(const unsigned int &channel);
    void writeNATCElementOption(const unsigned int &channel);
    void writeTCElementOptions(const unsigned int &channel, const unsigned int &page = 1);
    void writeDefaultScalerOptions(const unsigned int &channel);
    void writeThermocoupleScalerOptions(const unsigned int &channel);

    // Generic method for writing to the SDO port via asynPortClient
    asynStatus writeInt32SdoPortClient(const std::string &paramName, const epicsInt32 &value);

    // Methods for writing to the SDO port via the generic method above
    asynStatus setChannelParameter(const unsigned int &channel, const std::string &paramName, const unsigned int &value);
    asynStatus setChannelInterface(const unsigned int &channel, const unsigned int &value);
    asynStatus setChannelSensorSupply(const unsigned int &channel, const unsigned int &value);
    asynStatus setChannelRTDElement(const unsigned int &channel, const unsigned int &value);
    asynStatus setChannelTCElement(const unsigned int &channel, const unsigned int &value);
    asynStatus setChannelScaler(const unsigned int &channel, const unsigned int &value);

    // Methods for reading current measurement settings (e.g. after interface change)
    asynStatus readChannelSubSetting(const unsigned int &channel, const std::string &paramName, epicsInt32 &paramValue);
    asynStatus readCurrentChannelSubSettings(const unsigned int &channel);

    // Method to call after changing measurement type
    void setFirstSubTypeAfterTypeChanged(const unsigned int &channel, const int &value, const std::string &statusString);

    // Methods for handling asynParameter changes
    bool checkIfMeasurementTypeChanged(const int &param, const epicsInt32 &value);
    bool checkIfMeasurementSubTypeChanged(const int &param, const epicsInt32 &value);
    bool checkIfSensorSupplyOptionChanged(const int &param, const epicsInt32 &value);
    bool checkIfRTDPageChanged(const int &param, const epicsInt32 &value);
    bool checkIfRTDOptionChanged(const int &param, const epicsInt32 &value);
    bool checkIfTCPageChanged(const int &param, const epicsInt32 &value);
    bool checkIfTCOptionChanged(const int &param, const epicsInt32 &value);
    bool checkIfScalerOptionChanged(const int &param, const epicsInt32 &value);

    // Methods for special handling of asynParameter changes
    void checkIfSubTypeOptionChangingBetweenTCTypes(unsigned int channel, const epicsInt32 &value);

    // Method for updating channel status string
    void updateChannelStatusString(const unsigned int &channel, const std::string &string, const epicsAlarmSeverity &severity);

    // asynPortClient to talk to the SDO port when setting channel parameters
    SdoPortClient sdoPortClient;

};


#endif /* ELM3704_H */
