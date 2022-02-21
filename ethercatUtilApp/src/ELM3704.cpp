#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>

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
    sdoPortClient(sdoPortName) /* Create SdoPortClient instance */
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

        // RTD element
        epicsSnprintf(str, NBUFF, "CH%d:RTD_ELEMENT_PAGE", ch+1);
        createParam(str, asynParamInt32, &measurementRTDElementPage[ch]);
        epicsSnprintf(str, NBUFF, "CH%d:RTD_ELEMENT", ch+1);
        createParam(str, asynParamInt32, &measurementRTDElement[ch]);

        // TC element
        epicsSnprintf(str, NBUFF, "CH%d:TC_ELEMENT_PAGE", ch+1);
        createParam(str, asynParamInt32, &measurementTCElementPage[ch]);
        epicsSnprintf(str, NBUFF, "CH%d:TC_ELEMENT", ch+1);
        createParam(str, asynParamInt32, &measurementTCElement[ch]);

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
    writeNAOption(measurementSubType[channel]);
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 13, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Voltage set. Range: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 4, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Current set. Range: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 2, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Potentiometer set. Range: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 3, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "Thermocouple set: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 5, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "IEPE set. Range: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 6, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "FB strain gauge set: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 4, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "HB strain gauge set: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 8, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "QB 2wire strain gauge set: " + std::string(strings[0])); 
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 8, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "QB 3wire strain gauge set: " + std::string(strings[0]));
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
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 15, measurementSubType[channel], 0);
    // Set interface to first valid subtype
    setFirstSubTypeAfterTypeChanged(channel, values[0], "RTD set: " + std::string(strings[0]));
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
    // Map values based to the corresponding 0x80n01:02 sensor supply value
    static int values[12] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 65534, 65535 };
    static int severities[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 12, measurementSensorSupply[channel], 0);
}


// Empty the RTD page options list
void ELM3704::writeNARTDElementPageOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementRTDElementPage[channel]);
    // Update parameter
    setIntegerParam(measurementRTDElementPage[channel], 0);
}


// Write the RTD page options
void ELM3704::writeRTDElementPageOptions(const unsigned int &channel)
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
void ELM3704::writeNARTDElementOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementRTDElement[channel]);
}


// Write the RTD element options list
void ELM3704::writeRTDElementOptions(const unsigned int &channel, const unsigned int &page)
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
void ELM3704::writeNATCElementPageOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementTCElementPage[channel]);
    // Update parameter
    setIntegerParam(measurementTCElementPage[channel], 0);
}


// Write the TC page options
void ELM3704::writeTCElementPageOptions(const unsigned int &channel)
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
void ELM3704::writeNATCElementOption(const unsigned int &channel)
{
    // Set options
    writeNAOption(measurementTCElement[channel]);
}

// Write the TC element options list
void ELM3704::writeTCElementOptions(const unsigned int &channel, const unsigned int &page)
{
    static const char *firstPageStrings[16] = {
        "None",
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
    };
    static const char *secondPageStrings[4] = {
        "Pt/Pd (0..1000C)",
        "A-1 (0..2500C)",
        "A-2 (0..1800C)",
        "A-3 (0..1800C)",
    };

    // Map values based to the corresponding 0x80n01:14 scaler value
    static int firstPageValues[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16 };
    static int secondPageValues[4] = { 17, 18, 19, 20 };
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
            doCallbacksEnum((char **)secondPageStrings, secondPageValues, severities, 4, measurementTCElement[channel], 0);
            break;
        default:
            printf("ERROR: invalid TC element page %d for channel %d\n", page, channel);
            break;
    }
    // Update the asyn parameter to the first value on the page
    setIntegerParam(measurementTCElement[channel], value);
    // Write the interface value to the SDO port
    setChannelRTDElement(channel, value);
    // Update the channel status string
    updateChannelStatusString(channel, elementString , epicsSevNone);
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


// Set the parameter of a channel via the asynPortClient
asynStatus ELM3704::setChannelParameter(const unsigned int &channel, const std::string &paramName, const unsigned int &value)
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
asynStatus ELM3704::setChannelInterface(const unsigned int &channel, const unsigned int &value)
{
    // Set the parameter
    asynStatus status = setChannelParameter(channel, "Interface", value);

    // When we change interface we also need to check if sub-settings change based on
    // allowed values
    readCurrentChannelSubSettings(channel);

    return status;
}


// Change the sensor supply value of a channel via the asynPortClient
asynStatus ELM3704::setChannelSensorSupply(const unsigned int &channel, const unsigned int &value)
{
    // Set parameter
    return setChannelParameter(channel, "SensorSupply", value);
}


// Change the RTD element value of a channel via the asynPortClient
asynStatus ELM3704::setChannelRTDElement(const unsigned int &channel, const unsigned int &value)
{
    // Set parameter
    return setChannelParameter(channel, "RTDElement", value);
}


// Change the RTD element value of a channel via the asynPortClient
asynStatus ELM3704::setChannelTCElement(const unsigned int &channel, const unsigned int &value)
{
    // Set parameter
    return setChannelParameter(channel, "TCElement", value);
}


// Change the Scaler value of a channel via the asynPortClient
asynStatus ELM3704::setChannelScaler(const unsigned int &channel, const unsigned int &value)
{
    // Set parameter
    return setChannelParameter(channel, "Scaler", value);
}


// Method for reading a parameter using the SDO port client
asynStatus ELM3704::readChannelSubSetting(const unsigned int &channel, const std::string &paramName, epicsInt32 &paramValue)
{
    std::string paramString = "CH" + std::to_string(channel+1) + ":" + paramName;
    return sdoPortClient.read(paramString, paramValue);
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
bool ELM3704::checkIfMeasurementTypeChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementType[ch])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[ch], 1);
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
                        writeNASubTypeOption(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::Voltage:
                        writeVoltageSubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::Current:
                        writeCurrentSubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::Potentiometer:
                        writePotentiometerSubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::Thermocouple:
                        writeThermocoupleSubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeTCElementPageOptions(ch);
                        writeTCElementOptions(ch);
                        writeThermocoupleScalerOptions(ch);
                        break;

                    case Type::IEPiezoElectric:
                        writeIEPESubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::StrainGaugeFullBridge:
                        writeStrainGaugeFBSubTypeOptions(ch);
                        writeStrainGaugeSensorSupplyOptions(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::StrainGaugeHalfBridge:
                        writeStrainGaugeHBSubTypeOptions(ch);
                        writeStrainGaugeSensorSupplyOptions(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::StrainGaugeQuarterBridge2Wire:
                        writeStrainGaugeQB2WireSubTypeOptions(ch);
                        writeStrainGaugeSensorSupplyOptions(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::StrainGaugeQuarterBridge3Wire:
                        writeStrainGaugeQB3WireSubTypeOptions(ch);
                        writeStrainGaugeSensorSupplyOptions(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    case Type::RTD:
                        writeRTDSubTypeOptions(ch);
                        writeNASensorSupplyOption(ch);
                        writeRTDElementPageOptions(ch);
                        writeRTDElementOptions(ch);
                        writeNATCElementPageOption(ch);
                        writeNATCElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;

                    default:
                        writeNASubTypeOption(ch);
                        writeNASensorSupplyOption(ch);
                        writeNARTDElementPageOption(ch);
                        writeNARTDElementOption(ch);
                        writeDefaultScalerOptions(ch);
                        break;
                }
            } catch (const std::runtime_error &e)
            {
                // Force reload anyway and rethrow
                setIntegerParam(measurementTypeLoaded[ch], 0);
                throw e;
            }

            // Set the loaded parameter to done
            setIntegerParam(measurementTypeLoaded[ch], 0);

            // Signal that it was the type that changed
            return true;

            // Break out of the channel loop
            break;
        }
    }
    return false;
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



// Check and handle changes to the sensor supply option
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


// Check and handle changes to the RTD page
bool ELM3704::checkIfRTDPageChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementRTDElementPage[ch])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[ch], 1);
            callParamCallbacks();
            printf("Channel %d RTD element page changed to %d\n", ch, value);
            writeRTDElementOptions(ch, value);
            setIntegerParam(measurementTypeLoaded[ch], 0);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the RTD option
bool ELM3704::checkIfRTDOptionChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementRTDElement[ch])
        {
            printf("Channel %d RTD element option changed to %d\n", ch, value);
            setChannelRTDElement(ch, value);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the TC page
bool ELM3704::checkIfTCPageChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementTCElementPage[ch])
        {
            // Set the loaded parameter to loading for GUI update
            setIntegerParam(measurementTypeLoaded[ch], 1);
            callParamCallbacks();
            printf("Channel %d TC element page changed to %d\n", ch, value);
            writeTCElementOptions(ch, value);
            setIntegerParam(measurementTypeLoaded[ch], 0);
            return true;
        }
    }
    return false;
}


// Check and handle changes to the RTD option
bool ELM3704::checkIfTCOptionChanged(const int &param, const epicsInt32 &value)
{
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementTCElement[ch])
        {
            printf("Channel %d TC element option changed to %d\n", ch, value);
            setChannelTCElement(ch, value);
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
void ELM3704::updateChannelStatusString(const unsigned int &channel, const std::string &string, const epicsAlarmSeverity &severity)
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
