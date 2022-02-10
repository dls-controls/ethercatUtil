/*
 * simELM3704SdoPortDriver.h
 *
 * Benjamin Bradnick
 *
 * Test class for simulating the ELM3704 SDO asynPortDriver
 *
*/

#include <asynPortDriver.h>

#include <iocsh.h>
#include <epicsExport.h>
#include <epicsThread.h>


class SimELM3704SdoPortDriver : public asynPortDriver
{

public:
    // Constructor
    SimELM3704SdoPortDriver(const char *portName) : asynPortDriver(
        portName,
        1,
        asynInt32Mask | asynDrvUserMask,
        asynInt32Mask,
        0,
        1,
        0,
        0),
        portName(portName)
    {
        // Create test parameters for each channel
        static const int NBUFF = 255;
        char str[NBUFF];
        for (unsigned int ch=0; ch<4; ch++)
        {
            // Interface
            epicsSnprintf(str, NBUFF, "CH%d:Interface", ch+1);
            createParam(str, asynParamInt32, &interface[ch]);

            // Sensor supply
            epicsSnprintf(str, NBUFF, "CH%d:SensorSupply", ch+1);
            createParam(str, asynParamInt32, &sensorSupply[ch]);

            // RTD element
            epicsSnprintf(str, NBUFF, "CH%d:RTDElement", ch+1);
            createParam(str, asynParamInt32, &RTDElement[ch]);

            // TC element
            epicsSnprintf(str, NBUFF, "CH%d:TCElement", ch+1);
            createParam(str, asynParamInt32, &TCElement[ch]);

            // Scaler
            epicsSnprintf(str, NBUFF, "CH%d:Scaler", ch+1);
            createParam(str, asynParamInt32, &scaler[ch]);

        }

        // Initialise values
        for (unsigned int ch=0; ch<4; ch++)
        {
            setIntegerParam(interface[ch], 0);
            setIntegerParam(sensorSupply[ch], 0);
            setIntegerParam(RTDElement[ch], 0);
            setIntegerParam(TCElement[ch], 0);
            setIntegerParam(scaler[ch], 0);
        }
    }

    // Overide writeInt32 for basic logging
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value)
    {
        printf("%s: writing value %d to param %d\n", portName.c_str(), pasynUser->reason, value);
        // Pretend to take some time
        epicsThreadSleep(1.0);
        return asynPortDriver::writeInt32(pasynUser, value);
    }

private:
    // Attributes
    std::string portName;

    // Simulated asynParameter indices for each channel
    int interface[4];
    int sensorSupply[4];
    int RTDElement[4];
    int TCElement[4];
    int scaler[4];

};


/* EPICS IOCSH STUFF */

extern "C"
{

    /** EPICS iocsh callable function to call constructor for the TestSdoPortDriver class.
      * \param[in] portName The name of the asyn port created in this driver.
      */
    int SimELM3704SdoPortDriverConfigure(const char *portName)
    {
        new SimELM3704SdoPortDriver(portName);
        return(asynSuccess);
    }


    /* EPICS iocsh shell commands */

    static const iocshArg initArg0 = { "portName", iocshArgString };
    static const iocshArg * const initArgs[] = { &initArg0 };
    static const iocshFuncDef initFuncDef = { "SimELM3704SdoPortDriverConfigure", 1, initArgs };

    static void initCallFunc(const iocshArgBuf *args)
    {
        SimELM3704SdoPortDriverConfigure(args[0].sval);
    }

    void SimELM3704SdoPortDriverRegister(void)
    {
        iocshRegister(&initFuncDef, initCallFunc);
    }

    epicsExportRegistrar(SimELM3704SdoPortDriverRegister);

}