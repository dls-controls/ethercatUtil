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
    static const char *noneRangeStrings[1] = { "None" };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int noneRangeValues[1] = { 0 };
    static int noneRangeSeverities[1] = { 0 };
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], 0);
    // Write zero to interface parameter of client SDO port
    setChannelInterface(channel, 0);
    // Update strings and values
    doCallbacksEnum(
        (char **)noneRangeStrings,
        noneRangeValues,
        noneRangeSeverities,
        1,
        measurementSubType[channel],
        0
    );
}


// Write the voltage subtype options
void ELM3704::writeVoltageSubTypeOptions(const unsigned int &channel)
{
    static const char *voltageRangeStrings[13] = {
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
    static int voltageRangeValues[13] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15 };
    static int voltageRangeSeverities[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], voltageRangeValues[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, voltageRangeValues[0]);
    // Update strings and values
    doCallbacksEnum(
        (char **)voltageRangeStrings,
        voltageRangeValues,
        voltageRangeSeverities,
        13,
        measurementSubType[channel],
        0
    );
}


// Write current subtype options
void ELM3704::writeCurrentSubTypeOptions(const unsigned int &channel)
{
    static const char *currentRangeStrings[4] = {
        "+/- 20mA",
        "0-20mA",
        "4-20mA",
        "4-20mA NAMUR",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int currentRangeValues[4] = { 17, 18, 19, 20 };
    static int currentRangeSeverities[4] = { 0, 0, 0, 0 };
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], currentRangeValues[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, currentRangeValues[0]);
    // Update strings and values
    doCallbacksEnum(
        (char **)currentRangeStrings,
        currentRangeValues,
        currentRangeSeverities,
        4,
        measurementSubType[channel],
        0
    );
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
    bool measurementSubTypeChanged = false;
    for (unsigned int ch=0; ch<4; ch++)
    {
        if (param == measurementSubType[ch])
        {
            // Get current measurement type to determine actions to take
            epicsInt32 type;
            getIntegerParam(measurementType[ch], &type);
            switch(type)
            {
                case Type::Voltage:
                    printf("Channel %d measurement voltage subtype changed to %d\n", ch, value);
                    setChannelInterface(ch, value);
                    break;

                case Type::Current:
                    printf("Channel %d measurement current subtype changed to %d\n", ch, value);
                    setChannelInterface(ch, value);
                    break;         

                default:
                    printf("Channel %d measurement type not implemented %d\n", ch, value);
                    break;

            }

            // Signal that it was the type that changed
            measurementSubTypeChanged = true;

            // Break out of the channel loop
            break;

        }
    }
    return measurementSubTypeChanged;
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

    printf("writeInt32 called - parameter: %d, value: %d\n", param, value);

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
