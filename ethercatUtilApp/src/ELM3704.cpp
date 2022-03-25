#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>

#include <stdexcept>
#include <string>
#include <thread>
#include <iostream>

#include "ELM3704Properties.h"

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
    sdoPortClient(sdoPortName) /* Create SdoPortClient instance */
{
    /* Asyn parameter creation */

    // For each channel
    static const int NBUFF = 255;
    char str[NBUFF];
    for (unsigned int channel=0; channel<4; channel++)
    {
        // Measurement type
        epicsSnprintf(str, NBUFF, "CH%d:TYPE", channel+1);
        createParam(str, asynParamInt32, &measurementType[channel]);

        // Measurement subtype
        epicsSnprintf(str, NBUFF, "CH%d:SUBTYPE", channel+1);
        createParam(str, asynParamInt32, &measurementSubType[channel]);

        // Measurement type is loaded
        epicsSnprintf(str, NBUFF, "CH%d:LOADED", channel+1);
        createParam(str, asynParamInt32, &measurementTypeLoaded[channel]);

        // Measurement sensor supply
        epicsSnprintf(str, NBUFF, "CH%d:SENSOR_SUPPLY", channel+1);
        createParam(str, asynParamInt32, &measurementSensorSupply[channel]);

        // RTD element
        epicsSnprintf(str, NBUFF, "CH%d:RTD_ELEMENT_PAGE", channel+1);
        createParam(str, asynParamInt32, &measurementRTDElementPage[channel]);
        epicsSnprintf(str, NBUFF, "CH%d:RTD_ELEMENT", channel+1);
        createParam(str, asynParamInt32, &measurementRTDElement[channel]);

        // TC element
        epicsSnprintf(str, NBUFF, "CH%d:TC_ELEMENT_PAGE", channel+1);
        createParam(str, asynParamInt32, &measurementTCElementPage[channel]);
        epicsSnprintf(str, NBUFF, "CH%d:TC_ELEMENT", channel+1);
        createParam(str, asynParamInt32, &measurementTCElement[channel]);

        // Measurement scaler
        epicsSnprintf(str, NBUFF, "CH%d:SCALER", channel+1);
        createParam(str, asynParamInt32, &measurementScaler[channel]);

        // Status message
        epicsSnprintf(str, NBUFF, "CH%d:STATUS", channel+1);
        createParam(str, asynParamOctet, &channelStatusMessage[channel]);
    }

    // Initialise asyn parameters using a thread
    initialiseThread = std::thread(&ELM3704::initialiseValues, this);
}


// Initialise the asynParameter values based on current module settings
void ELM3704::initialiseValues()
{
    std::cout << portName << ": initialising values" << std::endl;
    // Wait for SDO asynPort to be ready
    unsigned int tries = 0;
    int parameterValue;
    while (true)
    {
        if (readChannelSubSetting(0, "Interface", parameterValue) == asynSuccess)
        {
            printf("%s: SDO connection is up\n", portName);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            break;
        }
        else
        {
            tries++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Now fetch actual values
    int interface, scaler, sensorSupply, RTDElement, TCElement;
    int status = asynSuccess;
    for (unsigned int channel=0; channel<4; channel++)
    {
        // Get current channel settings
        status |= readChannelSubSetting(channel, "Interface", interface);
        status |= readChannelSubSetting(channel, "Scaler", scaler);
        status |= readChannelSubSetting(channel, "SensorSupply", sensorSupply);
        status |= readChannelSubSetting(channel, "RTDElement", RTDElement);
        status |= readChannelSubSetting(channel, "TCElement", TCElement);

        printf(
            "%s: channel %d settings: %d, %d, %d, %d, %d\n",
            portName,
            channel,
            interface,
            scaler,
            sensorSupply,
            RTDElement,
            TCElement
        );

        // Set channel status string
        if (status)
        {
            setStringParam(channelStatusMessage[channel], "Initialisation failed");
        }
        else
        {
            setStringParam(channelStatusMessage[channel], "OK");
        }
    }
    // Reflect changes in asynParameter values
    callParamCallbacks();
    std::cout << portName << ": initialising values complete" << std::endl;
}


// Empty the subtype options list
void ELM3704::writeNAOption(int param)
{
    static const char *strings[1] = { "N/A" };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[1] = { 0 };
    static int severities[1] = { 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 1, param, 0);
}


// Called to set channel to first valid subtype after changing measurement type
void ELM3704::setFirstSubTypeAfterTypeChanged(unsigned int channel, int value, const std::string &statusString)
{
    // Update asynParameter
    setIntegerParam(measurementSubType[channel], value);
    // Write the interface value to the SDO port
    setChannelInterface(channel, value);
    // Update the channel status string
    updateChannelStatusString(channel, statusString, epicsSevNone);
}

// Empty the subtype options list
void ELM3704::writeNASubTypeOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementSubType[channel]);
    // Set interface to 0
    setFirstSubTypeAfterTypeChanged(channel, 0, "Channel turned off");
}


// Write the voltage subtype options
void ELM3704::writeVoltageSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::voltageStrings,
        ELM3704Properties::voltageValues,
        ELM3704Properties::severities,
        13,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::voltageValues[0],
        "Voltage set. Range: " + std::string(ELM3704Properties::voltageStrings[0])
    );
}


// Write current subtype options
void ELM3704::writeCurrentSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::currentStrings,
        ELM3704Properties::currentValues,
        ELM3704Properties::severities,
        4,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::currentValues[0],
        "Current set. Range: " + std::string(ELM3704Properties::currentStrings[0])
    );
}


// Write potentiometer subtype options
void ELM3704::writePotentiometerSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::potStrings,
        ELM3704Properties::potValues,
        ELM3704Properties::severities,
        2,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::potValues[0],
        "Potentiometer set. Type: " + std::string(ELM3704Properties::potStrings[0])
    );
}


// Write thermocouple subtype options
void ELM3704::writeThermocoupleSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::TCStrings,
        ELM3704Properties::TCValues,
        ELM3704Properties::severities,
        3,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::TCValues[0],
        "Thermocouple set. Type: " + std::string(ELM3704Properties::TCStrings[0])
    );
}


// Write integrated electronics piezo-electric subtype options
void ELM3704::writeIEPESubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::IEPEStrings,
        ELM3704Properties::IEPEValues,
        ELM3704Properties::severities,
        5,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::IEPEValues[0],
        "IEPE set. Range: " + std::string(ELM3704Properties::IEPEStrings[0])
    );
}


// Write strain gauge full bridge subtype options
void ELM3704::writeStrainGaugeFBSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::StrainGaugeFBStrings,
        ELM3704Properties::StrainGaugeFBValues,
        ELM3704Properties::severities,
        6,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::StrainGaugeFBValues[0],
        "Strain gauge FB set. Type: " + std::string(ELM3704Properties::StrainGaugeFBStrings[0])
    );
}


// Write strain gauge half bridge subtype options
void ELM3704::writeStrainGaugeHBSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::StrainGaugeHBStrings,
        ELM3704Properties::StrainGaugeHBValues,
        ELM3704Properties::severities,
        4,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::StrainGaugeHBValues[0],
        "Strain gauge HB set. Type: " + std::string(ELM3704Properties::StrainGaugeHBStrings[0])
    );
}


// Write strain gauge quarter bridge 2 wire subtype options
void ELM3704::writeStrainGaugeQB2WireSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::StrainGaugeQB2WStrings,
        ELM3704Properties::StrainGaugeQB2WValues,
        ELM3704Properties::severities,
        8,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::StrainGaugeQB2WValues[0],
        "Strain gauge QB 2wire set. Type: " + std::string(ELM3704Properties::StrainGaugeQB2WStrings[0])
    );
}


// Write strain gauge quarter bridge 3 wire subtype options
void ELM3704::writeStrainGaugeQB3WireSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::StrainGaugeQB3WStrings,
        ELM3704Properties::StrainGaugeQB3WValues,
        ELM3704Properties::severities,
        8,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::StrainGaugeQB3WValues[0],
        "Strain gauge QB 2wire set. Type: " + std::string(ELM3704Properties::StrainGaugeQB3WStrings[0])
    );
}


// Write strain gauge quarter bridge 3 wire subtype options
void ELM3704::writeRTDSubTypeOptions(unsigned int channel)
{
    // Update strings and values
    doCallbacksEnum(
        (char **)ELM3704Properties::RTDStrings,
        ELM3704Properties::RTDValues,
        ELM3704Properties::severities,
        8,
        measurementSubType[channel],
        0
    );
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(
        channel,
        ELM3704Properties::RTDValues[0],
        "RTD set. Type: " + std::string(ELM3704Properties::RTDStrings[0])
    );
}


// Empty the sensor supply options list
void ELM3704::writeNASensorSupplyOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementSensorSupply[channel]);
}


// Write the sensor supply options for strain gauge measurements
void ELM3704::writeStrainGaugeSensorSupplyOptions(unsigned int channel)
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
    // Map values based to the corresponding 0x80n01:02 sensor supply value
    static int values[12] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 65534, 65535 };
    static int severities[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 12, measurementSensorSupply[channel], 0);
}


// Write the sensor supply options for IEPE measurements
void ELM3704::writeIEPESensorySupplyOption(unsigned int channel)
{
    // IEPE measurement type just fixes sensory supply to local control
    static const char *strings[1] = {
        "Local control",
    };
    // Map values based to the corresponding 0x80n01:02 sensor supply value
    static int values[1] = { 65534 };
    static int severities[1] = { 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 1, measurementSensorSupply[channel], 0);
}


// Empty the RTD page options list
void ELM3704::writeNARTDElementPageOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementRTDElementPage[channel]);
    // Update parameter
    setIntegerParam(measurementRTDElementPage[channel], 0);
}


// Write the RTD page options
void ELM3704::writeRTDElementPageOptions(unsigned int channel)
{
    static const char *strings[3] = {
        "1",
        "2",
        "3",
    };
    // Map values based to the corresponding 0x80n01:02 sensor supply value
    static int values[3] = { 1, 2, 3 };
    static int severities[3] = { 0, 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 3, measurementRTDElementPage[channel], 0);
}


// Empty the RTD element options list
void ELM3704::writeNARTDElementOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementRTDElement[channel]);
}


// Write the RTD element options list
void ELM3704::writeRTDElementOptions(unsigned int channel, unsigned int page)
{
    static const char *firstPageStrings[16] = {
        "None",
        "PT100 (-200..850C)",
        "NI100 (-60..250C)",
        "PT1000 (-200..850C)",
        "PT500 (-200..850C)",
        "PT200 (-200..850C)",
        "NI1000 (-60..250C)",
        "NI1000 TK5000 1.5kOhm",
        "NI120 (-60..320C)",
        "KT100/110/130/210/230",
        "KT10/11/13/16/19", // Split same option across two for readable strings
        "KTY81/82-110,120,150",
        "KTY81-121 (-50..150C)",
        "KTY81-122 (-50..150C)",
        "KTY81-151 (-50..150C)",
        "KTY81-152 (-50..150C)",
    };
    static const char *secondPageStrings[16] = {
        "KTY81/82-210,220,250",
        "KTY81-221 (-50..150C)",
        "KTY81-222 (-50..150C)",
        "KTY81-251 (-50..150C)",
        "KTY81-252 (-50..150C)",
        "KTY83-110,120,150",
        "KTY83-121 (-50..175C)",
        "KTY83-122 (-50..175C)",
        "KTY83-151 (-50..175C)",
        "KTY83-152 (-50..175C)",
        "KTY84-130,150 (-40..300C)",
        "KTY84-151 (-40..300C)",
        "KTY21/23-3 (-50..150C)",
        "KTY1x-5 (-50..150C)",
        "KTY1x-7 (-50..150C)",
        "KTY21/23-5 (-50..150C)",
    };
    static const char *thirdPageStrings[4] = {
        "KTY21/23-7 (-50..150C)",
        "B-Parameter equation",
        "DIN IEC 60751 equation",
        "Steinhart Hart equation",
    };
    // Map values based to the corresponding 0x80n01:14 scaler value
    static int firstPageValues[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 14 };
    static int secondPageValues[16] = { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
    static int thirdPageValues[4] = { 31, 64, 65, 66 };
    static int severities[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Update strings and values
    epicsInt32 value = 0;
    std::string elementString = std::string("RTD element type changed to: ");
    switch (page)
    {
        case 1:
            printf("Writing 1st page RTD element options for channel %d\n", channel);
            value = firstPageValues[0];
            elementString += firstPageStrings[0];
            doCallbacksEnum((char **)firstPageStrings, firstPageValues, severities, 16, measurementRTDElement[channel], 0);
            break;
        case 2:
            printf("Writing 2nd page RTD element options for channel %d\n", channel);
            value = secondPageValues[0];
            elementString += secondPageStrings[0];
            doCallbacksEnum((char **)secondPageStrings, secondPageValues, severities, 16, measurementRTDElement[channel], 0);
            break;
        case 3:
            printf("Writing 3rd page RTD element options for channel %d\n", channel);
            value = thirdPageValues[0];
            elementString += thirdPageStrings[0];
            doCallbacksEnum((char **)thirdPageStrings, thirdPageValues, severities, 4, measurementRTDElement[channel], 0);
            break;
        default:
            printf("ERROR: invalid RTD element page %d for channel %d\n", page, channel);
            break;
    }
    // Update the asyn parameter to the first value on the page
    setIntegerParam(measurementRTDElement[channel], value);
    // Write the interface value to the SDO port
    setChannelRTDElement(channel, value);
    // Update the channel status string
    updateChannelStatusString(channel, elementString , epicsSevNone);
}


// Empty the TC page options list
void ELM3704::writeNATCElementPageOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementTCElementPage[channel]);
    // Update parameter
    setIntegerParam(measurementTCElementPage[channel], 0);
}


// Write the TC page options
void ELM3704::writeTCElementPageOptions(unsigned int channel)
{
    static const char *strings[2] = {
        "1",
        "2",
    };
    // Map values based to the corresponding 0x80n01:02 sensor supply value
    static int values[2] = { 1, 2 };
    static int severities[2] = { 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 2, measurementTCElementPage[channel], 0);
}


// Empty the TC element options list
void ELM3704::writeNATCElementOption(unsigned int channel)
{
    // Set options
    writeNAOption(measurementTCElement[channel]);
}

// Write the TC element options list
void ELM3704::writeTCElementOptions(unsigned int channel, unsigned int page)
{
    static const char *firstPageStrings[16] = {
        "K (-270..1372C)",
        "J (-210..1200C)",
        "L (-50..900C)",
        "E (-270..1000C)",
        "T (-270..400C)",
        "N (-270..1300C)",
        "U (-50..600C)",
        "B (200..1820C)",
        "R (-50..1768C)",
        "S (-50..1768C)",
        "C (0..2320C)",
        "D (0..2490C)",
        "G (1000..2300C)",
        "P (PLII 0..1395C)",
        "Au/Pt (0..1000C)",
        "Pt/Pd (0..1000C)",
    };
    // Note: the following settings only available from revision 0017 onwards
    static const char *secondPageStrings[3] = {
        "A-1 (0..2500C)",
        "A-2 (0..1800C)",
        "A-3 (0..1800C)",
    };

    // Map values based to the corresponding 0x80n01:14 scaler value
    static int firstPageValues[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17 };
    static int secondPageValues[3] = { 18, 19, 20 };
    static int severities[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Update strings and values
    epicsInt32 value = 0;
    std::string elementString = std::string("TC element type changed to: ");
    switch (page)
    {
        case 1:
            printf("Writing 1st page TC element options for channel %d\n", channel);
            value = firstPageValues[0];
            elementString += firstPageStrings[0];
            doCallbacksEnum((char **)firstPageStrings, firstPageValues, severities, 16, measurementTCElement[channel], 0);
            break;
        case 2:
            printf("Writing 2nd page TC element options for channel %d\n", channel);
            value = secondPageValues[0];
            elementString += secondPageStrings[0];
            doCallbacksEnum((char **)secondPageStrings, secondPageValues, severities, 3, measurementTCElement[channel], 0);
            break;
        default:
            printf("ERROR: invalid TC element page %d for channel %d\n", page, channel);
            break;
    }
    // Update the asyn parameter to the first value on the page
    setIntegerParam(measurementTCElement[channel], value);
    // We do not need to set the value here as the module will automatically choose a default
    // appropriate type.
    // Update the channel status string
    updateChannelStatusString(channel, elementString , epicsSevNone);
}

// Write default scaler options
void ELM3704::writeDefaultScalerOptions(unsigned int channel)
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
void ELM3704::writeThermocoupleScalerOptions(unsigned int channel)
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


// Set the parameter of a channel via the asynPortClient
asynStatus ELM3704::setChannelParameter(unsigned int channel, const std::string &paramName, unsigned int value)
{
    asynStatus status;

    // Add one to channel as the asyn parameter is numbered 1-4
    std::string parameterString = "CH" + std::to_string(channel+1) + ":" + paramName;
    try
    {
        status = sdoPortClient.writeRead(parameterString, (epicsInt32) value);
    } catch (const std::runtime_error &e)
    {
        // Update to bad channel status message and rethrow exception
        updateChannelStatusString(channel, "Failed to set parameter: " + paramName, epicsSevMajor);
        throw e;
    }
    // Set channel status message
    updateChannelStatusString(channel, "Parameter updated:" + paramName, epicsSevNone);

    return status;
}


// Change the Interface value of a channel via the asynPortClient
asynStatus ELM3704::setChannelInterface(unsigned int channel, unsigned int value)
{
    // Set the parameter
    asynStatus status = setChannelParameter(channel, "Interface", value);

    // When we change interface we also need to check if sub-settings change based on
    // allowed values
    readCurrentChannelSubSettings(channel);

    return status;
}


// Change the sensor supply value of a channel via the asynPortClient
asynStatus ELM3704::setChannelSensorSupply(unsigned int channel, unsigned int value)
{
    // Set parameter
    return setChannelParameter(channel, "SensorSupply", value);
}


// Change the RTD element value of a channel via the asynPortClient
asynStatus ELM3704::setChannelRTDElement(unsigned int channel, unsigned int value)
{
    // Set parameter
    return setChannelParameter(channel, "RTDElement", value);
}


// Change the RTD element value of a channel via the asynPortClient
asynStatus ELM3704::setChannelTCElement(unsigned int channel, unsigned int value)
{
    // Set parameter
    return setChannelParameter(channel, "TCElement", value);
}


// Change the Scaler value of a channel via the asynPortClient
asynStatus ELM3704::setChannelScaler(unsigned int channel, unsigned int value)
{
    // Set parameter
    return setChannelParameter(channel, "Scaler", value);
}


// Method for reading a parameter using the SDO port client
asynStatus ELM3704::readChannelSubSetting(unsigned int channel, const std::string &paramName, epicsInt32 &paramValue)
{
    std::string paramString = "CH" + std::to_string(channel+1) + ":" + paramName;
    return sdoPortClient.read(paramString, paramValue);
}


// Method for reading sub-settings of a module (e.g. after interface change)
asynStatus ELM3704::readCurrentChannelSubSettings(unsigned int channel)
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

    // RTD element
    if (readChannelSubSetting(channel, "RTDElement", parameterValue) == asynSuccess)
    {
        setIntegerParam(measurementRTDElement[channel], parameterValue);
    }
    else status = asynError;

    // TC element
    if (readChannelSubSetting(channel, "TCElement", parameterValue) == asynSuccess)
    {
        setIntegerParam(measurementTCElement[channel], parameterValue);
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
bool ELM3704::checkIfMeasurementTypeChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementType[channel])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[channel], 1);
            // Force the load
            callParamCallbacks();

            try
            {
                /* Update other options based on the current selected type
                 *   - Measurement subtype
                 *   - Sensor supply options
                 *   - RTD element options
                 *   - TC element
                 *   - Scaler options
                */
                switch (value)
                {
                    case Type::None:
                        writeNASubTypeOption(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::Voltage:
                        writeVoltageSubTypeOptions(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::Current:
                        writeCurrentSubTypeOptions(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::Potentiometer:
                        writePotentiometerSubTypeOptions(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::Thermocouple:
                        writeThermocoupleSubTypeOptions(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        // The initial 80mV subtype doesn't use elements
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeThermocoupleScalerOptions(channel);
                        break;

                    case Type::IEPiezoElectric:
                        writeIEPESubTypeOptions(channel);
                        writeIEPESensorySupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::StrainGaugeFullBridge:
                        writeStrainGaugeFBSubTypeOptions(channel);
                        writeStrainGaugeSensorSupplyOptions(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::StrainGaugeHalfBridge:
                        writeStrainGaugeHBSubTypeOptions(channel);
                        writeStrainGaugeSensorSupplyOptions(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::StrainGaugeQuarterBridge2Wire:
                        writeStrainGaugeQB2WireSubTypeOptions(channel);
                        writeStrainGaugeSensorSupplyOptions(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::StrainGaugeQuarterBridge3Wire:
                        writeStrainGaugeQB3WireSubTypeOptions(channel);
                        writeStrainGaugeSensorSupplyOptions(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    case Type::RTD:
                        writeRTDSubTypeOptions(channel);
                        writeNASensorSupplyOption(channel);
                        writeRTDElementPageOptions(channel);
                        writeRTDElementOptions(channel);
                        writeNATCElementPageOption(channel);
                        writeNATCElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;

                    default:
                        writeNASubTypeOption(channel);
                        writeNASensorSupplyOption(channel);
                        writeNARTDElementPageOption(channel);
                        writeNARTDElementOption(channel);
                        writeDefaultScalerOptions(channel);
                        break;
                }
            } catch (const std::runtime_error &e)
            {
                // Force reload anyway and rethrow
                setIntegerParam(measurementTypeLoaded[channel], 0);
                throw e;
            }

            // Set the loaded parameter to done
            setIntegerParam(measurementTypeLoaded[channel], 0);

            // Signal that it was the type that changed
            return true;

            // Break out of the channel loop
            break;
        }
    }
    return false;
}


// Check and handle changes to the measurement subtype
bool ELM3704::checkIfMeasurementSubTypeChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementSubType[channel])
        {
            // Check if we are in TC mode and moving from 80mV <-> CJC, CJC RTD
            checkIfSubTypeOptionChangingBetweenTCTypes(channel, value);

            // Now set interface
            printf("Channel %d measurement subtype changed to %d\n", channel, value);
            setChannelInterface(channel, value);
            return true;
        }
    }
    return false;
}



// Check and handle changes to the sensor supply option
bool ELM3704::checkIfSensorSupplyOptionChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementSensorSupply[channel])
        {
            printf("Channel %d sensor supply option changed to %d\n", channel, value);
            setChannelSensorSupply(channel, value);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the RTD page
bool ELM3704::checkIfRTDPageChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementRTDElementPage[channel])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[channel], 1);
            callParamCallbacks();
            printf("Channel %d RTD element page changed to %d\n", channel, value);
            writeRTDElementOptions(channel, value);
            setIntegerParam(measurementTypeLoaded[channel], 0);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the RTD option
bool ELM3704::checkIfRTDOptionChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementRTDElement[channel])
        {
            printf("Channel %d RTD element option changed to %d\n", channel, value);
            setChannelRTDElement(channel, value);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the TC page
bool ELM3704::checkIfTCPageChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementTCElementPage[channel])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[channel], 1);
            callParamCallbacks();
            printf("Channel %d TC element page changed to %d\n", channel, value);
            writeTCElementOptions(channel, value);
            setIntegerParam(measurementTypeLoaded[channel], 0);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the TC option
bool ELM3704::checkIfTCOptionChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementTCElement[channel])
        {
            printf("Channel %d TC element option changed to %d\n", channel, value);
            setChannelTCElement(channel, value);
            return true;
        }
    }
    return false;
}

// Check and handle changes to the scaler option
bool ELM3704::checkIfScalerOptionChanged(int param, const epicsInt32 &value)
{
    for (unsigned int channel=0; channel<4; channel++)
    {
        if (param == measurementScaler[channel])
        {
            printf("Channel %d scaler option changed to %d\n", channel, value);
            setChannelScaler(channel, value);
            return true;
        }
    }
    return false;
}


// Check if we are going between TC subtype options
void ELM3704::checkIfSubTypeOptionChangingBetweenTCTypes(unsigned int channel, const epicsInt32 &value)
{
    /* There are 3 TC measurement types:
       * 81 - TC 80mV
       * 86 - TC CJC
       * 87 - TC CJC RTD (Note: only available from revision 0017 onwards)

       CJC and CJC RTD modes expect the TC element to be set to the appropriate type. 80mV
       just measures the voltage output. So we need to check if the measurement sub-type is
       swapping between these two modes to display correct TC element options.
    */
    enum TCType { voltage=81, cjc=86, cjcrtd=87 };

    // Check if we are going to 80mV
    if (value == TCType::voltage)
    {
        // Check if we are moving from CJC or CJC RTD
        int currentSubType;
        getIntegerParam(measurementSubType[channel], &currentSubType);
        if (currentSubType == TCType::cjc || currentSubType == TCType::cjcrtd)
        {
            // Hide TC element options
            writeNATCElementPageOption(channel);
            writeNATCElementOption(channel);
        }
    }
    // Check if we are going to CJC or CJC RTD
    else if (value == TCType::cjc || value == TCType::cjcrtd)
    {
        // Check if we are moving from 80mV
        int currentSubType;
        getIntegerParam(measurementSubType[channel], &currentSubType);
        if (currentSubType == TCType::voltage)
        {
            // Show TC element options
            writeTCElementPageOptions(channel);
            writeTCElementOptions(channel);
        }
    }

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
    try
    {
        if (checkIfMeasurementTypeChanged(param, value)) {}
        else if (checkIfSensorSupplyOptionChanged(param, value)) {}
        else if (checkIfMeasurementSubTypeChanged(param, value)) {}
        else if (checkIfRTDPageChanged(param, value)) {}
        else if (checkIfRTDOptionChanged(param, value)) {}
        else if (checkIfTCPageChanged(param, value)) {}
        else if (checkIfTCOptionChanged(param, value)) {}
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
void ELM3704::updateChannelStatusString(unsigned int channel, const std::string &string, const epicsAlarmSeverity &severity)
{
    setStringParam(channelStatusMessage[channel], string);
    setParamAlarmSeverity(channelStatusMessage[channel], severity);
}


/* EPICS IOCSH STUFF */

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
