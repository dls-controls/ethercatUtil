/*
 * SdoPortClient.h
 *
 * Benjamin Bradnick
 *
 * Class which uses an asynPortClient to connect to an EtherCAT SDO port.
 *
 */

#ifndef SDOPORTCLIENT_H
#define SDOPORTCLIENT_H


#include <asynPortClient.h>


class SdoPortClient
{

public:
    // Constructor
    SdoPortClient(const char* sdoPortName);

    // Methods for writing and reading parameter values
    asynStatus writeRead(const std::string &paramName, const epicsInt32 &value, const double &timeout=1.0);
    asynStatus read(const std::string &paramName, epicsInt32 &value);

private:
    // Attributes
    std::string portName;
    asynPortClient portClient;

    // Methods
    void report();

};

#endif /* SDOPORTCLIENT_H */
