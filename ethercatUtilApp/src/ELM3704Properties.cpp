#include "ELM3704Properties.h"


// Constructor
ELM3704Properties::ELM3704Properties()
{

}

/* Static member definitions
 *
 * Due to using C++11, we cannot use `const static inline` from C++17). We define them
 * in the source .cpp so they are only defined once.
*/


// Interface values
int ELM3704Properties::voltageValues[numVoltageOptions] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15 };
int ELM3704Properties::currentValues[numCurrentOptions] = { 17, 18, 19, 20 };
int ELM3704Properties::potValues[numPotOptions] = { 65, 66 };
int ELM3704Properties::TCValues[numTCOptions] = { 81, 86, 87 };
int ELM3704Properties::IEPEValues[numIEPEOptions] = { 97, 98, 99, 107, 108 };
int ELM3704Properties::StrainGaugeFBValues[numStrainGaugeFBOptions] = { 259, 261, 268, 291, 293, 300 };
int ELM3704Properties::StrainGaugeHBValues[numStrainGaugeHBOptions] = { 323, 329, 355, 361 };
int ELM3704Properties::StrainGaugeQB2WValues[numStrainGaugeQB2WOptions] = { 388, 390, 391, 396, 452, 454, 455, 460 };
int ELM3704Properties::StrainGaugeQB3WValues[numStrainGaugeQB3WOptions] = { 420, 422, 423, 428, 484, 486, 487, 492 };
int ELM3704Properties::RTDValues[numRTDOptions] = { 785, 786, 787, 800, 801, 802, 821, 822, 823, 830, 831, 832, 848, 849, 850 };


// Interface strings
const char *ELM3704Properties::voltageStrings[numVoltageOptions] = {
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
const char *ELM3704Properties::currentStrings[numCurrentOptions] = {
    "+/- 20mA",
    "0-20mA",
    "4-20mA",
    "4-20mA NAMUR",
};
const char *ELM3704Properties::potStrings[numPotOptions] = {
    "3 wire",
    "5 wire",
};
const char *ELM3704Properties::TCStrings[numTCOptions] = {
    "80mV",
    "CJC",
    "CJC RTD",
};
const char *ELM3704Properties::IEPEStrings[numIEPEOptions] = {
    "+/- 10V",
    "+/- 5V",
    "+/- 2.5V",
    "0-20V",
    "0-10V",
};
const char *ELM3704Properties::StrainGaugeFBStrings[numStrainGaugeFBOptions] = {
    "4 wire 2mV/V",
    "4 wire 4mV/V",
    "4 wire 32mV/V",
    "6 wire 2mV/V",
    "6 wire 4mV/V",
    "6 wire 32mV/V",
};
const char *ELM3704Properties::StrainGaugeHBStrings[numStrainGaugeHBOptions] = {
    "3 wire 2mV/V",
    "3 wire 16mV/V",
    "5 wire 2mV/V",
    "5 wire 16mV/V",
};
const char *ELM3704Properties::StrainGaugeQB2WStrings[numStrainGaugeQB2WOptions] = {
    "120R 2mV/V comp",
    "120R 4mV/V comp",
    "120R 8mV/V",
    "120R 32mV/V",
    "350R 2mV/V comp",
    "350R 4mV/V comp",
    "350R 8mV/V",
    "350R 32mV/V",
};
const char *ELM3704Properties::StrainGaugeQB3WStrings[numStrainGaugeQB3WOptions] = {
    "120R 2mV/V comp",
    "120R 4mV/V comp",
    "120R 8mV/V",
    "120R 32mV/V",
    "350R 2mV/V comp",
    "350R 4mV/V comp",
    "350R 8mV/V",
    "350R 32mV/V",
};
const char *ELM3704Properties::RTDStrings[numRTDOptions] = {
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


// Sensor supply values and strings
int ELM3704Properties::SGSensorSupplyValues[numSGSensorSupplyOptions] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 65534, 65535 };
int ELM3704Properties::IEPESensorSupplyValues[numIEPESensorSupplyOptions] = { 65534 };

const char *ELM3704Properties::SGSensorSupplyStrings[numSGSensorSupplyOptions] = {
    "0.0V",
    "1.0V",
    "1.5V",
    "2.0V",
    "2.5V",
    "3.0V",
    "3.5V",
    "4.0V",
    "4.5V",
    "5.0V",
    "Local control",
    "External supply",
};
const char *ELM3704Properties::IEPESensorSupplyStrings[numIEPESensorSupplyOptions] = {
    "Local control",
};


// RTD values and strings
int ELM3704Properties::RTDElementPageValues[numRTDElementPageOptions] = { 1, 2, 3};
int ELM3704Properties::RTDElementFirstPageValues[numRTDElementFirstPageOptions] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 14 };
int ELM3704Properties::RTDElementSecondPageValues[numRTDElementSecondPageOptions] = { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
int ELM3704Properties::RTDElementThirdPageValues[numRTDElementThirdPageOptions] = { 31, 64, 65, 66 };

const char *ELM3704Properties::RTDElementPageStrings[numRTDElementPageOptions] = {
    "1",
    "2",
    "3",
};
const char *ELM3704Properties::RTDElementFirstPageStrings[numRTDElementFirstPageOptions] = {
    "None",
    "PT100 (-200..850C)",
    "NI100 (-60..250C)",
    "PT1000 (-200..850C)",
    "PT500 (-200..850C)",
    "PT200 (-200..850C)",
    "NI1000 (-60..250C)",
    "NI1000 TK5000 1.5kOhm",
    "NI120 (-60..320C)",
    "KT100/110/130/210/230",
    "KT10/11/13/16/19", // Split same option across two for readable strings
    "KTY81/82-110,120,150",
    "KTY81-121 (-50..150C)",
    "KTY81-122 (-50..150C)",
    "KTY81-151 (-50..150C)",
    "KTY81-152 (-50..150C)",
};
const char *ELM3704Properties::RTDElementSecondPageStrings[numRTDElementSecondPageOptions] = {
    "KTY81/82-210,220,250",
    "KTY81-221 (-50..150C)",
    "KTY81-222 (-50..150C)",
    "KTY81-251 (-50..150C)",
    "KTY81-252 (-50..150C)",
    "KTY83-110,120,150",
    "KTY83-121 (-50..175C)",
    "KTY83-122 (-50..175C)",
    "KTY83-151 (-50..175C)",
    "KTY83-152 (-50..175C)",
    "KTY84-130,150 (-40..300C)",
    "KTY84-151 (-40..300C)",
    "KTY21/23-3 (-50..150C)",
    "KTY1x-5 (-50..150C)",
    "KTY1x-7 (-50..150C)",
    "KTY21/23-5 (-50..150C)",
};
const char *ELM3704Properties::RTDElementThirdPageStrings[numRTDElementThirdPageOptions] = {
    "KTY21/23-7 (-50..150C)",
    "B-Parameter equation",
    "DIN IEC 60751 equation",
    "Steinhart Hart equation",
};


// TC element values and strings
int ELM3704Properties::TCElementPageValues[numTCElementPageOptions] = { 1, 2 };
int ELM3704Properties::TCElementFirstPageValues[numTCElementFirstPageOptions] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17 };
int ELM3704Properties::TCElementSecondPageValues[numTCElementSecondPageOptions] = { 18, 19, 20 };

const char *ELM3704Properties::TCElementPageStrings[numTCElementPageOptions] = {
    "1",
    "2",
};
const char *ELM3704Properties::TCElementFirstPageStrings[numTCElementFirstPageOptions] = {
    "K (-270..1372C)",
    "J (-210..1200C)",
    "L (-50..900C)",
    "E (-270..1000C)",
    "T (-270..400C)",
    "N (-270..1300C)",
    "U (-50..600C)",
    "B (200..1820C)",
    "R (-50..1768C)",
    "S (-50..1768C)",
    "C (0..2320C)",
    "D (0..2490C)",
    "G (1000..2300C)",
    "P (PLII 0..1395C)",
    "Au/Pt (0..1000C)",
    "Pt/Pd (0..1000C)",
};
// Note: the following settings only available from revision 0017 onwards
const char *ELM3704Properties::TCElementSecondPageStrings[numTCElementSecondPageOptions] = {
    "A-1 (0..2500C)",
    "A-2 (0..1800C)",
    "A-3 (0..1800C)",
};


// Scaler values and strings
int ELM3704Properties::DefaultScalerValues[numDefaultScalerOptions] = { 0, 3 };
int ELM3704Properties::TCScalerValues[numTCScalerOptions] = { 0, 3, 6, 7, 8 };

const char *ELM3704Properties::DefaultScalerStrings[numDefaultScalerOptions] = {
    "Extended range",
    "Legacy range",
};
const char *ELM3704Properties::TCScalerStrings[numTCScalerOptions] = {
    "Extended range",
    "Legacy range",
    "Celsius",
    "Kelvin",
    "Fahrenheit",
};

// Severities can be done with a single array of all values
int ELM3704Properties::severities[numSeveritiesOptions] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
