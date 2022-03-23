#include "SdoPortClient.h"


// Constructor
SdoPortClient::SdoPortClient(const char* sdoPortName): 
    portName(sdoPortName),
    portClient(sdoPortName)
{
    report();
}


// Write to the port and wait until the readback matches (or time out)
asynStatus SdoPortClient::writeRead(const std::string &paramName, const epicsInt32 &value, double timeout)
{
    // Write to the value parameter
    asynStatus status = portClient.write(paramName, value);
    if (status)
    {
        printf(
            "%s: failed to set parameter %s to value %d (status %d)\n",
            portName.c_str(),
            paramName.c_str(),
            value,
            status
        );
    }
    else
    {
        // Poll the readback until it matches or we time out
        epicsInt32 readbackValue;
        double parameterSetTime = 0.0;
        static const double parameterPollInterval = 0.1;
        asynStatus readStatus = read(paramName, readbackValue);
        if (readStatus == asynSuccess)
        {
            // Readback value doesn't match, wait and see if it updates
            while (readbackValue != value)
            {
                epicsThreadSleep(parameterPollInterval);
                readStatus = read(paramName, readbackValue);
                parameterSetTime += parameterPollInterval;

                // Check if we exceed a timeout limit
                if (parameterSetTime > timeout)
                {
                    // Throw an exception which should be caught by writeInt32
                    throw std::runtime_error(
                        "ERROR: timeout setting " +
                        paramName +
                        " from " +
                        std::to_string(readbackValue) +
                        " to " +
                        std::to_string(value)
                    );
                }
                else if (readStatus)
                {
                    // TODO: check if we should throw a custom error here and set status message
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


// Wrapper function to read from asynPortClient
asynStatus SdoPortClient::read(const std::string &paramName, epicsInt32 &value)
{
    asynStatus status = portClient.read(paramName, &value);
    if (status)
    {
        printf(
            "%s: could not read asynPortClient parameter %s (status %d)\n",
            portName.c_str(),
            paramName.c_str(),
            status
        );  
    }
    return status;
}


// Report list of parameters
void SdoPortClient::report()
{
    FILE *pFile;
    char fileName[50];
    sprintf(fileName, "/tmp/%s-report.txt", portName.c_str());
    pFile = fopen(fileName, "w");
    if (pFile == NULL)
    {
        printf("%s: error opening report file\n", portName.c_str());
    }
    else
    {
        portClient.report(pFile, 1);
        fclose(pFile);
        printf("%s: report file written to %s\n", portName.c_str(), fileName);
    }
}
