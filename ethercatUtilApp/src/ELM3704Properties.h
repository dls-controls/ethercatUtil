/*
 * ELM3704Properties.h
 *
 * Benjamin Bradnick
 *
 * Class for storing property values and strings for channel settings of the
 * ELM3704 module.
 *
*/

#ifndef ELM3704PROPERTIES_H
#define ELM3704PROPERTIES_H


class ELM3704Properties
{
public:
    // Interface number of options
    static const int numVoltageOptions = 13;
    static const int numCurrentOptions = 4;
    static const int numPotOptions = 2;
    static const int numTCOptions = 3;
    static const int numIEPEOptions = 5;
    static const int numStrainGaugeFBOptions = 6;
    static const int numStrainGaugeHBOptions = 4;
    static const int numStrainGaugeQB2WOptions = 8;
    static const int numStrainGaugeQB3WOptions = 8;
    static const int numRTDOptions = 15;


    // Interface values
    static int voltageValues[numVoltageOptions];
    static int currentValues[numCurrentOptions];
    static int potValues[numPotOptions];
    static int TCValues[numTCOptions];
    static int IEPEValues[numIEPEOptions];
    static int StrainGaugeFBValues[numStrainGaugeFBOptions];
    static int StrainGaugeHBValues[numStrainGaugeHBOptions];
    static int StrainGaugeQB2WValues[numStrainGaugeQB2WOptions];
    static int StrainGaugeQB3WValues[numStrainGaugeQB3WOptions];
    static int RTDValues[numRTDOptions];

    // Interface strings
    static const char *voltageStrings[numVoltageOptions];
    static const char *currentStrings[numCurrentOptions];
    static const char *potStrings[numPotOptions];
    static const char *TCStrings[numTCOptions];
    static const char *IEPEStrings[numIEPEOptions];
    static const char *StrainGaugeFBStrings[numStrainGaugeFBOptions];
    static const char *StrainGaugeHBStrings[numStrainGaugeHBOptions];
    static const char *StrainGaugeQB2WStrings[numStrainGaugeQB2WOptions];
    static const char *StrainGaugeQB3WStrings[numStrainGaugeQB3WOptions];
    static const char *RTDStrings[numRTDOptions];

    // Sensor supply values and strings
    static const int numSGSensorSupplyOptions = 12;
    static const int numIEPESensorSupplyOptions = 1;
    static int SGSensorSupplyValues[numSGSensorSupplyOptions];
    static int IEPESensorSupplyValues[numIEPESensorSupplyOptions];
    static const char *SGSensorSupplyStrings[numSGSensorSupplyOptions];
    static const char *IEPESensorSupplyStrings[numIEPESensorSupplyOptions];

    // RTD values and strings
    static const int numRTDElementPageOptions = 3;
    static const int numRTDElementFirstPageOptions = 16;
    static const int numRTDElementSecondPageOptions = 16;
    static const int numRTDElementThirdPageOptions = 4;
    static int RTDElementPageValues[numRTDElementPageOptions];
    static int RTDElementFirstPageValues[numRTDElementFirstPageOptions];
    static int RTDElementSecondPageValues[numRTDElementSecondPageOptions];
    static int RTDElementThirdPageValues[numRTDElementThirdPageOptions];
    static const char *RTDElementPageStrings[numRTDElementPageOptions];
    static const char *RTDElementFirstPageStrings[numRTDElementFirstPageOptions];
    static const char *RTDElementSecondPageStrings[numRTDElementSecondPageOptions];
    static const char *RTDElementThirdPageStrings[numRTDElementThirdPageOptions];

    // TC element values and strings
    static const int numTCElementPageOptions = 2;
    static const int numTCElementFirstPageOptions = 16;
    static const int numTCElementSecondPageOptions = 3;
    static int TCElementPageValues[numTCElementPageOptions];
    static int TCElementFirstPageValues[numTCElementFirstPageOptions];
    static int TCElementSecondPageValues[numTCElementSecondPageOptions];
    static const char *TCElementPageStrings[numTCElementPageOptions];
    static const char *TCElementFirstPageStrings[numTCElementFirstPageOptions];
    static const char *TCElementSecondPageStrings[numTCElementSecondPageOptions];

    // Scaler values and strings
    static const int numDefaultScalerOptions = 2;
    static const int numTCScalerOptions = 5;
    static int DefaultScalerValues[numDefaultScalerOptions];
    static int TCScalerValues[numTCScalerOptions];
    static const char *DefaultScalerStrings[numDefaultScalerOptions];
    static const char *TCScalerStrings[numTCScalerOptions];

    // Severities
    static const int numSeveritiesOptions = 16;
    static int severities[numSeveritiesOptions];

private:
    // Constructor is private as we just have static members
    ELM3704Properties();

};

#endif /* ELM3704PROPERTIES_H */
