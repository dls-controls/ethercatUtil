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
    // Interface values
    static int voltageValues[13];
    static int currentValues[4];
    static int potValues[2];
    static int TCValues[3];
    static int IEPEValues[5];
    static int StrainGaugeFBValues[6];
    static int StrainGaugeHBValues[4];
    static int StrainGaugeQB2WValues[8];
    static int StrainGaugeQB3WValues[8];
    static int RTDValues[15];

    // Interface strings
    static const char *voltageStrings[13];
    static const char *currentStrings[4];
    static const char *potStrings[2];
    static const char *TCStrings[3];
    static const char *IEPEStrings[5];
    static const char *StrainGaugeFBStrings[6];
    static const char *StrainGaugeHBStrings[4];
    static const char *StrainGaugeQB2WStrings[8];
    static const char *StrainGaugeQB3WStrings[8];
    static const char *RTDStrings[15];

    // Severities
    static int severities[16];

private:
    // Constructor is private as we just have static members
    ELM3704Properties();

};

#endif /* ELM3704PROPERTIES_H */
