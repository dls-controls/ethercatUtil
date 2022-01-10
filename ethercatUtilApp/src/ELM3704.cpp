#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>


// For logging
static const char *driverName = "ELM3704";


// Constructor
ELM3704::ELM3704(const char* portName) : asynPortDriver(
    portName,  /* asyn port name */
    1, /* maxAddr */
    asynInt32Mask | asynEnumMask  | asynDrvUserMask, /* Interface mask */
    asynInt32Mask | asynEnumMask,  /* Interrupt mask */
    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
    1, /* Autoconnect */
    0, /* Default priority */
    0) /* Default stack size*/
{

    /* Asyn parameter creation */

    // For each channel
    static const int NBUFF = 255;
    char str[NBUFF];
    for (unsigned int ch=0; ch<4; ch++) {
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
    setIntegerParam(measurementSubType[channel], 1);
    // Update strings and values
    doCallbacksEnum((char **)voltageRangeStrings, voltageRangeValues, voltageRangeSeverities, 12, measurementSubType[channel], 0);
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
    // Update strings and values
    doCallbacksEnum((char **)noneRangeStrings, noneRangeValues, noneRangeSeverities, 1, measurementSubType[channel], 0);
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
            if (value == 0)
            {
                printf("Channel %d measurement type changed to None\n");
                writeNoneStrings(i);
            }
            if (value == 1)
            {
                printf("Channel %d measurement type changed to Voltage\n");
                writeVoltageStrings(i);
            }

            // Set the loaded parameter to done
            setIntegerParam(measurementTypeLoaded[i], 0);

            // Finish updating parameter
            handled = true;
            status = asynPortDriver::writeInt32(pasynUser, value);
            break;
        }
        else if (param == measurementSubType[i])
        {
            printf("Channel %d measurement sub-type changed to %d\n", i, value);
            handled = true;
            status = asynPortDriver::writeInt32(pasynUser, value);
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
      * \param[in] portName The name of the asyn port driver to be created.
      */
    int ELM3704DriverConfigure(const char *portName)
    {
        new ELM3704(portName);
        return(asynSuccess);
    }


    /* EPICS iocsh shell commands */

    static const iocshArg initArg0 = { "portName", iocshArgString };
    static const iocshArg * const initArgs[] = { &initArg0 };
    static const iocshFuncDef initFuncDef = { "ELM3704DriverConfigure", 1, initArgs };

    static void initCallFunc(const iocshArgBuf *args)
    {
        ELM3704DriverConfigure(args[0].sval);
    }

    void ELM3704DriverRegister(void)
    {
        iocshRegister(&initFuncDef, initCallFunc);
    }

    epicsExportRegistrar(ELM3704DriverRegister);

}
