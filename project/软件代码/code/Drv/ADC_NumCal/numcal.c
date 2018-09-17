#include "numcal.h"
//from -20℃ to 120℃
double resistor_to_temperature_tbl[] = {
	98.26 , 92.74 , 87.55 , 82.69 , 78.12 , 73.83 , 69.8 , 66.01 , 62.45 ,
	59.1  , 55.95 , 52.98 , 50.19 , 47.57 , 45.09 , 42.76 , 40.56 , 38.49 ,
	36.53 , 34.68 , 32.94 , 31.3  , 29.74 , 28.28 , 26.89 , 25.58 , 24.34 ,
	23.17 , 22.06 , 21.01 , 20.01 , 19.07 , 18.18 , 17.33 , 16.53 , 15.77 ,
	15.05 , 14.37 , 13.72 , 13.1  , 12.52 , 11.96 , 11.43 , 10.93 , 10.45 ,
	10	  , 9.569 , 9.158 ,	8.768 , 8.396 , 8.042 ,	7.705 , 7.384 , 7.078 ,
	6.786 , 6.508 , 6.243 ,	5.99  , 5.749 , 5.519 ,	5.299 ,	5.089 ,	4.889 ,
	4.697 ,	4.514 ,	4.339 ,	4.172 ,	4.012 ,	3.86  ,	3.713 ,	3.574 ,	3.44  ,
	3.312 ,	3.189 ,	3.071 ,	2.959 ,	2.851 ,	2.748 ,	2.649 ,	2.554 ,	2.463 ,
	2.375 ,	2.292 ,	2.211 ,	2.134 ,	2.06  ,	1.989 ,	1.921 ,	1.855 ,	1.792 ,
	1.731 ,	1.673 ,	1.617 ,	1.563 ,	1.512 ,	1.462 ,	1.414 ,	1.368 ,	1.324 ,
	1.281 ,	1.24  ,	1.201 ,	1.163 ,	1.126 ,	1.091 ,	1.056 ,	1.024 ,	0.992 ,
	0.9615,	0.9321,	0.9038,	0.8764,	0.851,	0.8245,	0.8   ,	0.7762,	0.7533,
	0.7312,	0.7099,	0.6892,	0.6693,	0.65  ,	0.6314,	0.6135,	0.5961,	0.5793,
	0.563 ,	0.5473,	0.5321,	0.5174,	0.5031,	0.4894,	0.476 ,	0.4631,	0.4506,
	0.4386,	0.4268,	0.4155,	0.4045,	0.3939,	0.3835,
};
/**
 *描述：输入光照模块AD值，输出光强度值（lux）
 *参数：light_sensor_ad 光照模块AD值
 *返回值：光强度值（lux）
 *例：AD:973 light_sensor_voltege:784 resistance:3.116057 light_sensor_lux:137.238647
 **/
float get_LUX(unsigned short light_sensor_ad)				 
{
	unsigned short light_sensor_voltege;
	float light_sensor_resistance;
	float light_sensor_lux;

	light_sensor_voltege = ((float)light_sensor_ad*3300.0)/4096.0;  //单位：mV
	light_sensor_resistance = (10.0 * light_sensor_voltege) / (3300.0 - light_sensor_voltege); //单位：kΩ
	light_sensor_lux = pow(10, ((log10(15) - log10(light_sensor_resistance) + 0.6) / 0.6));
	return light_sensor_lux;
}


/**
 *描述：输入温度模块AD值，计算温度
 *参数：ADC_data 温度模块AD值
 *返回值：温度值（摄氏度）
 *例：AD:973 light_sensor_voltege:784 resistance:3.116057 light_sensor_lux:137.238647
 **/
int conv_res_to_temp(unsigned short ADC_data)
{
	double voltage_val_mV = ((double)ADC_data*3300.0)/4095.0;
	double resistor_val = (10000.0 * voltage_val_mV) / (3300.0 - voltage_val_mV);	// R7/(R1+R7)*3.3 = V	
	double resistor_val_max;
	double resistor_val_min;
	int i;
	int tbl_len = sizeof(resistor_to_temperature_tbl)/sizeof(double);
	
	resistor_val_min = resistor_to_temperature_tbl[tbl_len - 1] * 1000.0;
	resistor_val_max = resistor_to_temperature_tbl[0] * 1000.0;

	if(resistor_val < 0 || resistor_val > resistor_val_max ||  resistor_val < resistor_val_min)
		return -1;

	for(i = 0; i < tbl_len; i++){
		//printf("resistor_val:%f, tbl:%f\n", resistor_val, resistor_to_temperature_tbl[i]* 10000.0);
		if(resistor_val >= resistor_to_temperature_tbl[i] * 1000.0)
			break;
	}
	//printf("index:%d\n", i);

	return i-20;
}
