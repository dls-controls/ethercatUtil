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


// Write voltage strings and values of measurement subtype MBBI/MBBO records
void ELM3704::writeVoltageStrings(const unsigned int &channel)
{
    static const char *voltageRangeStrings[12] = {
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
        "+/- 0..10V",
    };
    // Map values based to the corresponding 0x80n01:01 interface value
    static int voltageRangeValues[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14 };
    static int voltageRangeSeverities[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Set subtype parameter to first possible value
    setIntegerParam(measurementSubType[channel], voltageRangeValues[0]);
    // Write first value to interface parameter of client SDO port
    setChannelInterface(channel, voltageRangeValues[0]);
    // Update strings and values
    doCallbacksEnum(
        (char **)voltageRangeStrings,
        voltageRangeValues,
        voltageRangeSeverities,
        12,
        measurementSubType[channel],
        0
    );
}


// Empty out strings and values of measurement subtype MBBI/MBBO records
void ELM3704::writeNoneStrings(const unsigned int &channel)
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


// Change the Interface value of a channel via the asynPortClient
asynStatus ELM3704::setChannelInterface(const unsigned int &channel, const unsigned int &value)
{
    // Add one to channel as the asyn parameter is numbered 1-4
    std::string interface_string = "CH" + std::to_string(channel+1) + ":Interface";
    printf("Setting channel %d interface via param %s to %d\n", channel+1, interface_string.c_str(), value);
    return sdoPortClient.write(interface_string, (epicsInt32) value);
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

    // Check if the parameter is handled directly in this subclass
    bool handled = false;
    for (unsigned int i=0; i<4; i++)
    {
        if (param == measurementType[i])
        {
            // Set the loaded parameter to loading
            setIntegerParam(measurementTypeLoaded[i], 1);
            callParamCallbacks();

            // Update the mbbi/mbbo strings
            if (value == Type::None)
            {
                printf("Channel %d measurement type changed to None\n", i);
                writeNoneStrings(i);
            }
            if (value == Type::Voltage)
            {
                printf("Channel %d measurement type changed to Voltage\n", i);
                writeVoltageStrings(i);
            }

            // Set the loaded parameter to done
            setIntegerParam(measurementTypeLoaded[i], 0);

            // Let base class handle parameter update
            handled = true;
            status = asynPortDriver::writeInt32(pasynUser, value);
            break;
        }
        else if (param == measurementSubType[i])
        {
            // Get current measurement type
            epicsInt32 type;
            getIntegerParam(measurementType[i], &type);

            // Check if voltage
            if (type == 1)
            {
                // Then set the interface parameter to the correct voltage setting
                setChannelInterface(i, value);
            }

            // Check if write was successful
            if (status)
            {
                printf("Changing %d measurement sub-type to %d failed\n", i, value);
            }
            else
            {
                printf("Channel %d measurement sub-type changed to %d\n", i, value);
                status = asynPortDriver::writeInt32(pasynUser, value);
            }
            handled = true;
            break;
        }
    }

    // Otherwise call the parent class for standard handling
    if (handled != true)
    {
        status = asynPortDriver::writeInt32(pasynUser, value);
    }

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
