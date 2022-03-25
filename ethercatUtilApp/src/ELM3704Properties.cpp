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

// Values
int ELM3704Properties::voltageValues[13] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15 };
int ELM3704Properties::currentValues[4] = { 17, 18, 19, 20 };
int ELM3704Properties::potValues[2] = { 65, 66 };
int ELM3704Properties::TCValues[3] = { 81, 86, 87 };
int ELM3704Properties::IEPEValues[5] = { 97, 98, 99, 107, 108 };
int ELM3704Properties::StrainGaugeFBValues[6] = { 259, 261, 268, 291, 293, 300 };
int ELM3704Properties::StrainGaugeHBValues[4] = { 323, 329, 355, 361 };
int ELM3704Properties::StrainGaugeQB2WValues[8] = { 388, 390, 391, 396, 452, 454, 455, 460 };
int ELM3704Properties::StrainGaugeQB3WValues[8] = { 420, 422, 423, 428, 484, 486, 487, 492 };
int ELM3704Properties::RTDValues[15] = { 785, 786, 787, 800, 801, 802, 821, 822, 823, 830, 831, 832, 848, 849, 850 };

// Strings
const char *ELM3704Properties::voltageStrings[13] = {
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
const char *ELM3704Properties::currentStrings[4] = {
    "+/- 20mA",
    "0-20mA",
    "4-20mA",
    "4-20mA NAMUR",
};
const char *ELM3704Properties::potStrings[2] = {
    "3 wire",
    "5 wire",
};
const char *ELM3704Properties::TCStrings[3] = {
    "80mV",
    "CJC",
    "CJC RTD",
};
const char *ELM3704Properties::IEPEStrings[5] = {
    "+/- 10V",
    "+/- 5V",
    "+/- 2.5V",
    "0-20V",
    "0-10V",
};
const char *ELM3704Properties::StrainGaugeFBStrings[6] = {
    "4 wire 2mV/V",
    "4 wire 4mV/V",
    "4 wire 32mV/V",
    "6 wire 2mV/V",
    "6 wire 4mV/V",
    "6 wire 32mV/V",
};
const char *ELM3704Properties::StrainGaugeHBStrings[4] = {
    "3 wire 2mV/V",
    "3 wire 16mV/V",
    "5 wire 2mV/V",
    "5 wire 16mV/V",
};
const char *ELM3704Properties::StrainGaugeQB2WStrings[8] = {
    "120R 2mV/V comp",
    "120R 4mV/V comp",
    "120R 8mV/V",
    "120R 32mV/V",
    "350R 2mV/V comp",
    "350R 4mV/V comp",
    "350R 8mV/V",
    "350R 32mV/V",
};
const char *ELM3704Properties::StrainGaugeQB3WStrings[8] = {
    "120R 2mV/V comp",
    "120R 4mV/V comp",
    "120R 8mV/V",
    "120R 32mV/V",
    "350R 2mV/V comp",
    "350R 4mV/V comp",
    "350R 8mV/V",
    "350R 32mV/V",
};
const char *ELM3704Properties::RTDStrings[15] = {
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

// Severities can be done with a single array of all values
int ELM3704Properties::severities[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
