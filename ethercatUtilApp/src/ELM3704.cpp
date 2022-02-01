#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>

#include <stdexcept>

#include <string>

// For logging
static const char *driverName = "ELM3704";


// Constructor
ELM3704::ELM3704(const char* portName, const char* sdoPortName) : asynPortDriver(
    portName,  /* asyn port name for this driver*/
    1, /* maxAddr */
    asynInt32Mask | asynEnumMask | asynOctetMask | asynDrvUserMask, /* Interface mask */
    asynInt32Mask | asynEnumMask | asynOctetMask,  /* Interrupt mask */
    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
    1, /* Autoconnect */
    0, /* Default priority */
    0), /* Default stack size*/
    sdoPortClient(sdoPortName, 1.0) /* Connect our client to the SDO asyn port */
{
    /* Asyn parameter creation */

    // For each channel
    static const int NBUFF = 255;
    char str[NBUFF];
    for (unsigned int ch=0; ch<4; ch++)
    {
        // Measurement type
        epicsSnprintf(str, NBUFF, "CH%d:TYPE", ch+1);
        createParam(str, asynParamInt32, &measurementType[ch]);
        // Measurement subtype
        epicsSnprintf(str, NBUFF, "CH%d:SUBTYPE", ch+1);
        createParam(str, asynParamInt32, &measurementSubType[ch]);
        // Measurement type is loaded
        epicsSnprintf(str, NBUFF, "CH%d:LOADED", ch+1);
        createParam(str, asynParamInt32, &measurementTypeLoaded[ch]);

        // Measurement sensor supply
        epicsSnprintf(str, NBUFF, "CH%d:SENSOR_SUPPLY", ch+1);
        createParam(str, asynParamInt32, &measurementSensorSupply[ch]);

        // Measurement scaler
        epicsSnprintf(str, NBUFF, "CH%d:SCALER", ch+1);
        createParam(str, asynParamInt32, &measurementScaler[ch]);

        // Status message
        epicsSnprintf(str, NBUFF, "CH%d:STATUS", ch+1);
        createParam(str, asynParamOctet, &channelStatusMessage[ch]);
    }

    /* Initialise asyn parameters */
    for (unsigned int ch=0; ch<4; ch++)
    {
        // Set status message string
        setStringParam(channelStatusMessage[ch], "OK");

        // TODO: get current parameter values and set interface options appropriately
    }

    printf("Statuses: %d, %d, %d, %d, %d\n", asynError, asynDisconnected, asynDisabled, asynParamBadIndex, asynParamUndefined);

}


// Empty the subtype options list
void ELM3704::writeNAOption(const int &param)
{
    static const char *strings[1] = { "N/A" };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[1] = { 0 };
    static int severities[1] = { 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 1, param, 0);
}


// Called to set channel to first valid subtype after changing measurement type
void ELM3704::setFirstSubTypeAfterTypeChanged(const unsigned int &channel, const int &value, const std::string &statusString)
{
    // Update asynParameter
    setIntegerParam(measurementSubType[channel], value);
    // Write the interface value to the SDO port
    setChannelInterface(channel, value);
    // Update the channel status string
    updateChannelStatusString(channel, statusString, epicsSevNone);
}

// Empty the subtype options list
void ELM3704::writeNASubTypeOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementType[channel]);
    // Set interface to 0
    setFirstSubTypeAfterTypeChanged(channel, 0, "Channel turned off");
}


// Write the voltage subtype options
void ELM3704::writeVoltageSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[13] = {
        "+/- 60V",
        "+/- 10V",
        "+/- 5V",
        "+/- 2.5V",
        "+/- 1.25V",
        "+/- 640mV",
        "+/- 320mV",
        "+/- 160mV",
        "+/- 80mV",
        "+/- 40mV",
        "+/- 20mV",
        "0-10V",
        "0-5V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[13] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15 };
    static int severities[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Voltage set. Range: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 13, measurementSubType[channel], 0);
}


// Write current subtype options
void ELM3704::writeCurrentSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[4] = {
        "+/- 20mA",
        "0-20mA",
        "4-20mA",
        "4-20mA NAMUR",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[4] = { 17, 18, 19, 20 };
    static int severities[4] = { 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Current set. Range: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 4, measurementSubType[channel], 0);
}


// Write potentiometer subtype options
void ELM3704::writePotentiometerSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[2] = {
        "3 wire",
        "5 wire",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[2] = { 65, 66 };
    static int severities[2] = { 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Potentiometer set. Range: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 2, measurementSubType[channel], 0);
}


// Write thermocouple subtype options
void ELM3704::writeThermocoupleSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[3] = {
        "80mV",
        "CJC",
        "CJC RTD",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[3] = { 81, 86, 87 };
    static int severities[3] = { 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Thermocouple set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 3, measurementSubType[channel], 0);
}


// Write integrated electronics piezo-electric subtype options
void ELM3704::writeIEPESubTypeOptions(const unsigned int &channel)
{
    static const char *strings[5] = {
        "+/- 10V",
        "+/- 5V",
        "+/- 2.5V",
        "0-20V",
        "0-10V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[5] = { 97, 98, 99, 107, 108 };
    static int severities[5] = { 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "IEPE set. Range: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 5, measurementSubType[channel], 0);
}


// Write strain gauge full bridge subtype options
void ELM3704::writeStrainGaugeFBSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[6] = {
        "4 wire 2mV/V",
        "4 wire 4mV/V",
        "4 wire 32mV/V",
        "6 wire 2mV/V",
        "6 wire 4mV/V",
        "6 wire 32mV/V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[6] = { 259, 261, 268, 291, 293, 300 };
    static int severities[6] = { 0, 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "FB strain gauge set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 6, measurementSubType[channel], 0);
}


// Write strain gauge half bridge subtype options
void ELM3704::writeStrainGaugeHBSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[4] = {
        "3 wire 2mV/V",
        "3 wire 16mV/V",
        "5 wire 2mV/V",
        "5 wire 16mV/V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[4] = { 323, 329, 355, 361 };
    static int severities[4] = { 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "HB strain gauge set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 4, measurementSubType[channel], 0);
}


// Write strain gauge quarter bridge 2 wire subtype options
void ELM3704::writeStrainGaugeQB2WireSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[8] = {
        "120R 2mV/V comp",
        "120R 4mV/V comp",
        "120R 8mV/V",
        "120R 32mV/V",
        "350R 2mV/V comp",
        "350R 4mV/V comp",
        "350R 8mV/V",
        "350R 32mV/V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[8] = { 388, 390, 391, 396, 452, 454, 455, 460 };
    static int severities[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "QB 2wire strain gauge set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 8, measurementSubType[channel], 0);
}


// Write strain gauge quarter bridge 3 wire subtype options
void ELM3704::writeStrainGaugeQB3WireSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[8] = {
        "120R 2mV/V comp",
        "120R 4mV/V comp",
        "120R 8mV/V",
        "120R 32mV/V",
        "350R 2mV/V comp",
        "350R 4mV/V comp",
        "350R 8mV/V",
        "350R 32mV/V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[8] = { 420, 422, 423, 428, 484, 486, 487, 492 };
    static int severities[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "QB 3wire strain gauge set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 8, measurementSubType[channel], 0);
}


// Write strain gauge quarter bridge 3 wire subtype options
void ELM3704::writeRTDSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[15] = {
        "2 wire 5k",
        "3 wire 5k",
        "4 wire 5k",
        "2 wire 2k",
        "3 wire 2k",
        "4 wire 2k",
        "2 wire 500R",
        "3 wire 500R",
        "4 wire 500R",
        "2 wire 200R",
        "3 wire 200R",
        "4 wire 200R",
        "2 wire 50R",
        "3 wire 50R",
        "4 wire 50R",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[15] = { 785, 786, 787, 800, 801, 802, 821, 822, 823, 830, 831, 832, 848, 849, 850 };
    static int severities[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "RTD set: " + std::string(strings[0]));
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 15, measurementSubType[channel], 0);
}


// Empty the sensor supply options list
void ELM3704::writeNASensorSupplyOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementSensorSupply[channel]);
}


// Write the sensor supply options for strain gauge measurements
void ELM3704::writeStrainGaugeSensorSupplyOptions(const unsigned int &channel)
{
    static const char *strings[12] = {
        "0.0V",
        "1.0V",
        "1.5V",
        "2.0V",
        "2.5V",
        "3.0V",
        "3.5V",
        "4.0V",
        "4.5V",
        "5.0V",
        "Local control",
        "External supply",
    };
    // Map values based to the corresponding 0x80n01:2E scaler value
    static int values[12] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 65534, 65535 };
    static int severities[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 12, measurementSensorSupply[channel], 0);
}


// Write default scaler options
void ELM3704::writeDefaultScalerOptions(const unsigned int &channel)
{
    static const char *strings[2] = {
        "Extended range",
        "Legacy range",
    };
    // Map values based to the corresponding 0x80n01:2E scaler value
    static int values[2] = { 0, 3 };
    static int severities[2] = { 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 2, measurementScaler[channel], 0);
}


// Write scaler options when measuring a thermocouple
void ELM3704::writeThermocoupleScalerOptions(const unsigned int &channel)
{
    static const char *strings[5] = {
        "Extended range",
        "Legacy range",
        "Celsius",
        "Kelvin",
        "Fahrenheit",
    };
    // Map values based to the corresponding 0x80n01:2E scaler value
    static int values[5] = { 0, 3, 6, 7, 8 };
    static int severities[5] = { 0, 0, 0, 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 5, measurementScaler[channel], 0);
}


// Generic wrapper method for writing to SDO port via asynPortClient
asynStatus ELM3704::writeInt32SdoPortClient(const std::string &paramName, const epicsInt32 &value)
{
    asynStatus status = sdoPortClient.write(paramName, value);

    // Check status
    if (status)
    {
        printf("ERROR: failed to set asynPortClient parameter %s to value %d\n", paramName.c_str(), value);
    }
    else
    {
        // Poll the readback until it matches or we time out
        epicsInt32 readbackValue;
        double parameterSetTime = 0.0;
        static const double paramaterSetTimeout = 5.0;
        static const double parameterPollInterval = 0.1;
        asynStatus readStatus = sdoPortClient.read(paramName, &readbackValue);
        if (readStatus)
        {
            printf(
                "ERROR: could not read asynPortClient parameter %s (status %d)\n",
                paramName.c_str(),
                readStatus
            );
        }
        else
        {
            // Readback value doesn't match, wait and see if it updates
            while (readbackValue != value)
            {
                epicsThreadSleep(parameterPollInterval);
                readStatus = sdoPortClient.read(paramName, &readbackValue);
                parameterSetTime += parameterPollInterval;

                // Check if we exceed a timeout limit
                if (parameterSetTime > paramaterSetTimeout)
                {
                    // Throw an exception which should be caught by writeInt32
                    throw std::runtime_error("ERROR: timeout waiting for " + paramName + " to update");
                }
                else if (readStatus)
                {
                    // TODO: check if we should throw a custom error here and set status message
                    printf("ERROR: could not read asynPortClient parameter %s\n", paramName.c_str());
                    break;
                }
            }
            printf(
                "Set asynPortClient parameter %s to value %d (%d) after %fs\n",
                paramName.c_str(),
                value,
                readbackValue,
                parameterSetTime
            );
        }
    }
    return status;
}


// Change the Interface value of a channel via the asynPortClient
asynStatus ELM3704::setChannelInterface(const unsigned int &channel, const unsigned int &value)
{
    // Add one to channel as the asyn parameter is numbered 1-4
    std::string parameterString = "CH" + std::to_string(channel+1) + ":Interface";
    asynStatus status = writeInt32SdoPortClient(parameterString, (epicsInt32) value);

    // When we change interface we also need to check if sub-settings change based on
    // allowed values
    readCurrentChannelSubSettings(channel);

    // Set channel status string
    updateChannelStatusString(channel, "Updated interface setting", epicsSevNone);

    return status;
}


// Change the sensor supply value of a channel via the asynPortClient
asynStatus ELM3704::setChannelSensorSupply(const unsigned int &channel, const unsigned int &value)
{
    // Add one to channel as the asyn parameter is numbered 1-4
    std::string parameterString = "CH" + std::to_string(channel+1) + ":SensorSupply";
    asynStatus status = writeInt32SdoPortClient(parameterString, (epicsInt32) value);

    // Set channel status string
    updateChannelStatusString(channel, "Updated sensor supply", epicsSevNone);

    return status;
}


// Change the Scaler value of a channel via the asynPortClient
asynStatus ELM3704::setChannelScaler(const unsigned int &channel, const unsigned int &value)
{
    asynStatus status;

    // Add one to channel as the asyn parameter is numbered 1-4
    std::string parameterString = "CH" + std::to_string(channel+1) + ":Scaler";
    try
    {
        status = writeInt32SdoPortClient(parameterString, (epicsInt32) value);
    } catch (const std::runtime_error &e)
    {
        // Update to bad channel status message and rethrow exception
        updateChannelStatusString(channel, "Failed to set scaler parameter", epicsSevMajor);
        throw e;
    }
    // Set channel status message
    updateChannelStatusString(channel, "Updated scaler setting", epicsSevNone);

    return status;
}


// Method for reading a parameter using the SDO port client
asynStatus ELM3704::readChannelSubSetting(const unsigned int &channel, const std::string &paramName, epicsInt32 &paramValue)
{
    std::string paramString = "CH" + std::to_string(channel+1) + ":" + paramName;
    asynStatus status = sdoPortClient.read(paramString, &paramValue);
    if (status)
    {
        printf(
            "Unable to read channel %d parameter %s (status %d, value %d)\n",
            channel,
            paramName.c_str(),
            status,
            paramValue);
        status = sdoPortClient.write(paramString, 0);
    }
    return status;
}

// Method for reading sub-settings of a module (e.g. after interface change)
asynStatus ELM3704::readCurrentChannelSubSettings(const unsigned int &channel)
{
    // Track overall status
    asynStatus status = asynSuccess;

    // Reused parameters
    epicsInt32 parameterValue;

    // Sensor supply
    if (readChannelSubSetting(channel, "SensorSupply", parameterValue) == asynSuccess)
    {
        setIntegerParam(measurementSensorSupply[channel], parameterValue);
    }
    else status = asynError;
    // Scaler
    if (readChannelSubSetting(channel, "Scaler", parameterValue) == asynSuccess)
    {
        setIntegerParam(measurementScaler[channel], parameterValue);
    }
    else status = asynError;

    return status;
}


// Check and handle changes to the measurement type
bool ELM3704::checkIfMeasurementTypeChanged(const int &param, const epicsInt32 &value)
{
    bool measurementTypeChanged = false;
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementType[ch])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[ch], 1);
            // Force the load
            callParamCallbacks();

            /* Update other options based on the current selected type
             *   - Measurement subtype
             *   - Scaler options
             */
            switch (value)
            {
                case Type::None:
                    printf("Channel %d measurement type changed to None\n", ch);
                    writeNASubTypeOption(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::Voltage:
                    printf("Channel %d measurement type changed to Voltage\n", ch);
                    writeVoltageSubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::Current:
                    printf("Channel %d measurement type changed to Current\n", ch);
                    writeCurrentSubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::Potentiometer:
                    printf("Channel %d measurement type changed to Potentiometer\n", ch);
                    writePotentiometerSubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::Thermocouple:
                    printf("Channel %d measurement type changed to Thermocouple\n", ch);
                    writeThermocoupleSubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeThermocoupleScalerOptions(ch);
                    break;

                case Type::IEPiezoElectric:
                    printf("Channel %d measurement type changed to IEPE\n", ch);
                    writeIEPESubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::StrainGaugeFullBridge:
                    printf("Channel %d measurement type changed to Strain gauge FB\n", ch);
                    writeStrainGaugeFBSubTypeOptions(ch);
                    writeStrainGaugeSensorSupplyOptions(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::StrainGaugeHalfBridge:
                    printf("Channel %d measurement type changed to Strain gauge HB\n", ch);
                    writeStrainGaugeHBSubTypeOptions(ch);
                    writeStrainGaugeSensorSupplyOptions(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::StrainGaugeQuarterBridge2Wire:
                    printf("Channel %d measurement type changed to Strain gauge QB 2 wire\n", ch);
                    writeStrainGaugeQB2WireSubTypeOptions(ch);
                    writeStrainGaugeSensorSupplyOptions(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::StrainGaugeQuarterBridge3Wire:
                    printf("Channel %d measurement type changed to Strain gauge QB 3 wire\n", ch);
                    writeStrainGaugeQB3WireSubTypeOptions(ch);
                    writeStrainGaugeSensorSupplyOptions(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                case Type::RTD:
                    printf("Channel %d measurement type changed to RTD\n", ch);
                    writeRTDSubTypeOptions(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;

                default:
                    printf("Channel %d measurement type changed to unimplemented type %d\n", ch, value);
                    writeNASubTypeOption(ch);
                    writeNASensorSupplyOption(ch);
                    writeDefaultScalerOptions(ch);
                    break;
            }

            // Set the loaded parameter to done
            setIntegerParam(measurementTypeLoaded[ch], 0);

            // Signal that it was the type that changed
            measurementTypeChanged = true;

            // Break out of the channel loop
            break;
        }
    }
    return measurementTypeChanged;
}


// Check and handle changes to the measurement subtype
bool ELM3704::checkIfMeasurementSubTypeChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementSubType[ch])
        {
            printf("Channel %d measurement subtype changed to %d\n", ch, value);
            setChannelInterface(ch, value);
            return true;
        }
    }
    return false;
}



// Check and handle changes to the scaler option
bool ELM3704::checkIfSensorSupplyOptionChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementSensorSupply[ch])
        {
            printf("Channel %d sensor supply option changed to %d\n", ch, value);
            setChannelSensorSupply(ch, value);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the scaler option
bool ELM3704::checkIfScalerOptionChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementScaler[ch])
        {
            printf("Channel %d scaler option changed to %d\n", ch, value);
            setChannelScaler(ch, value);
            return true;
        }
    }
    return false;
}


// AsynPortDriver::writeInt32 override
asynStatus ELM3704::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    // For logging
    static const char *functionName = "writeInt32";

    // Status
    asynStatus status = asynSuccess;

    // Updated parameter
    const int param = pasynUser->reason;

    // Check if we need to take any action via the SDO asynPortClient
    // TODO: return status values here via parameter reference and decide whether to update parameter
    // TODO: break out these checks into sub method if grow too large
    // TOOD: if type or subtype changed, add method to update current setting readbacks
    try
    {
        if (checkIfMeasurementTypeChanged(param, value)) {}
        else if (checkIfSensorSupplyOptionChanged(param, value)) {}
        else if (checkIfMeasurementSubTypeChanged(param, value)) {}
        else if (checkIfScalerOptionChanged(param, value)) {}
        else
        {
            printf("%s: parameter: %d not handled in ELM3704 class\n", functionName, param);
        }
    } catch (const std::runtime_error &e)
    {
        printf("%s: caught runtime error: %s\n", functionName, e.what());
        status = asynError;
    }

    // Check status
    if (status)
    {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,"%s::%s: Error setting param %d to %d (status %d)\n",
                  driverName, functionName, param, value, status);
        // Callback on parameters that may have been un-set as part of errors
        callParamCallbacks();
    }
    else
    {
        // Call the parent class method for handling parameter update and callback
        status = asynPortDriver::writeInt32(pasynUser, value);
    }

    return status;   
}


// Method for updating channel status string
void ELM3704::updateChannelStatusString(const unsigned int &channel, const std::string &string, const epicsAlarmSeverity &severity)
{
    setStringParam(channelStatusMessage[channel], string);
    setParamAlarmSeverity(channelStatusMessage[channel], severity);
}

extern "C"
{

    /** EPICS iocsh callable function to call constructor for the ELM3704 class.
      * \param[in] portName The name of the asyn port created in this driver.
      * \param[in] sdoPortName The name of the sdo port for the slave module (slave port name + "_SDO")
      */
    int ELM3704DriverConfigure(const char *portName, const char *sdoPortName)
    {
        new ELM3704(portName, sdoPortName);
        return(asynSuccess);
    }


    /* EPICS iocsh shell commands */

    static const iocshArg initArg0 = { "portName", iocshArgString };
    static const iocshArg initArg1 = { "sdoPortName", iocshArgString };
    static const iocshArg * const initArgs[] = { &initArg0, &initArg1 };
    static const iocshFuncDef initFuncDef = { "ELM3704DriverConfigure", 2, initArgs };

    static void initCallFunc(const iocshArgBuf *args)
    {
        ELM3704DriverConfigure(args[0].sval, args[1].sval);
    }

    void ELM3704DriverRegister(void)
    {
        iocshRegister(&initFuncDef, initCallFunc);
    }

    epicsExportRegistrar(ELM3704DriverRegister);

}
