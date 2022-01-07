#include "ELM3704.h"

#include <iocsh.h>
#include <epicsExport.h>


// For logging
static const char *driverName = "ELM3704";


// Constructor
ELM3704::ELM3704(const char* portName) : asynPortDriver(
    portName,  /* asyn port name */
    1, /* maxAddr */
    asynInt32Mask | asynDrvUserMask, /* Interface mask */
    asynInt32Mask,  /* Interrupt mask */
    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
    1, /* Autoconnect */
    0, /* Default priority */
    0) /* Default stack size*/
{

    /* Parameter creation */

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

    printf("writeInt32 called for parameter %d and value %d\n", param, value);

    // Check if the parameter is handled directly in this subclass
    bool handled = false;
    for (int i=0; i<4; i++)
    {
        if (param == measurementType[i])
        {
            printf("Channel %d measurement type changed to %d\n", i, value);
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
