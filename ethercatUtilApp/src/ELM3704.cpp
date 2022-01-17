#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>

#include <string>

// For logging
static const char *driverName = "ELM3704";


// Constructor
ELM3704::ELM3704(const char* portName, const char* sdoPortName) : asynPortDriver(
    portName,  /* asyn port name for this driver*/
    1, /* maxAddr */
    asynInt32Mask | asynEnumMask  | asynDrvUserMask, /* Interface mask */
    asynInt32Mask | asynEnumMask,  /* Interrupt mask */
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
    }
}


// Empty the subtype options list
void ELM3704::writeNoneSubTypeOptions(const unsigned int &channel)
{
    static const char *strings[1] = { "None" };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int values[1] = { 0 };
    static int severities[1] = { 0 };
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], 0);
    // Write zero to interface parameter of client SDO port
    setChannelInterface(channel, 0);
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 1, measurementSubType[channel], 0);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
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
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], values[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, values[0]);
    // Update strings and values
    doCallbacksEnum((char **)strings, values, severities, 15, measurementSubType[channel], 0);
}


// Change the Interface value of a channel via the asynPortClient
asynStatus ELM3704::setChannelInterface(const unsigned int &channel, const unsigned int &value)
{
    // Add one to channel as the asyn parameter is numbered 1-4
    std::string interface_string = "CH" + std::to_string(channel+1) + ":Interface";
    printf("Setting channel %d interface via param %s to %d\n", channel+1, interface_string.c_str(), value);
    return sdoPortClient.write(interface_string, (epicsInt32) value);
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

            // Update measurement subtype strings and values based on input value
            switch (value)
            {
                case Type::None:
                    printf("Channel %d measurement type changed to None\n", ch);
                    writeNoneSubTypeOptions(ch);
                    break;

                case Type::Voltage:
                    printf("Channel %d measurement type changed to Voltage\n", ch);
                    writeVoltageSubTypeOptions(ch);
                    break;

                case Type::Current:
                    printf("Channel %d measurement type changed to Current\n", ch);
                    writeCurrentSubTypeOptions(ch);
                    break;

                case Type::Potentiometer:
                    printf("Channel %d measurement type changed to Potentiometer\n", ch);
                    writePotentiometerSubTypeOptions(ch);
                    break;

                case Type::Thermocouple:
                    printf("Channel %d measurement type changed to Thermocouple\n", ch);
                    writeThermocoupleSubTypeOptions(ch);
                    break;

                case Type::IEPiezoElectric:
                    printf("Channel %d measurement type changed to IEPE\n", ch);
                    writePotentiometerSubTypeOptions(ch);
                    break;

                case Type::StrainGaugeFullBridge:
                    printf("Channel %d measurement type changed to Strain gauge FB\n", ch);
                    writeStrainGaugeFBSubTypeOptions(ch);
                    break;

                case Type::StrainGaugeHalfBridge:
                    printf("Channel %d measurement type changed to Strain gauge HB\n", ch);
                    writeStrainGaugeHBSubTypeOptions(ch);
                    break;

                case Type::StrainGaugeQuarterBridge2Wire:
                    printf("Channel %d measurement type changed to Strain gauge QB 2 wire\n", ch);
                    writeStrainGaugeQB2WireSubTypeOptions(ch);
                    break;

                case Type::StrainGaugeQuarterBridge3Wire:
                    printf("Channel %d measurement type changed to Strain gauge QB 3 wire\n", ch);
                    writeStrainGaugeQB3WireSubTypeOptions(ch);
                    break;

                case Type::RTD:
                    printf("Channel %d measurement type changed to RTD\n", ch);
                    writeRTDSubTypeOptions(ch);
                    break;

                default:
                    printf("Channel %d measurement type changed to unimplemented type %d\n", ch, value);
                    writeNoneSubTypeOptions(ch);
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
    if (checkIfMeasurementTypeChanged(param, value)){}
    else if (checkIfMeasurementSubTypeChanged(param, value)){}
    else
    {
        printf("%s: parameter: %d not handled in ELM3704 clss\n", functionName, param);
    }

    // Call the parent class method for handling parameter update and callback
    status = asynPortDriver::writeInt32(pasynUser, value);

    // Check status
    if (status)
    {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,"%s::%s: Error setting values'\n",
                  driverName, functionName);
    }
    return status;   
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
