#ifndef _NUMCAL_H
#define _NUMCAL_H

#include "math.h"

float get_LUX(unsigned short light_sensor_ad);
int conv_res_to_temp(unsigned short ADC_data);

#endif
