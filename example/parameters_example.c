/*
 * @Description    :
 * @Author         : Aiyangsky
 * @Date           : 2022-08-11 10:52:56
 * @LastEditors    : Aiyangsky
 * @LastEditTime   : 2022-08-11 12:04:09
 * @FilePath       : \Parameters\example\parameters_example.c
 */

#include <stdio.h>
#include <string.h>

#include "Parameters.h"

PARAMETERS_CB_T parameters_test;
unsigned char simulation_ROM[128];
unsigned char simulation_RAM[128];

unsigned short checksum(unsigned char *data, unsigned int size)
{
    unsigned short Sum;
    while (size)
    {
        Sum += *data;
        data++;
        size--;
    }
    return Sum;
}

bool Read_From_ROM(unsigned char *dst, unsigned int offset, unsigned int size)
{
    memcpy(dst, simulation_ROM + offset, size);

    return true;
}

bool Write_2_ROM(unsigned char *src, unsigned int offset, unsigned int size)
{
    memcpy(simulation_ROM + offset, src, size);

    return true;
}

int main(void)
{
    memset(simulation_ROM, EMPTY_BYTE, sizeof(simulation_ROM));
    memset(simulation_RAM, EMPTY_BYTE, sizeof(simulation_RAM));

    Parameters_Init(&parameters_test, "TEST\0", simulation_RAM, 0, 1024,
                    Read_From_ROM, Write_2_ROM, checksum);

    double value_1 = 1024.0f;
    Parameters_Creat(&parameters_test, "ABCDEF\0", PARAMETERS_TYPE_F64, &value_1);

    printf("Parameters_Creat out %f\n",
           *((double *)(Parameters_Creat(&parameters_test, "ABCDEF\0", PARAMETERS_TYPE_F64, &value_1))));

    double value_2 = 102.0f;
    Parameters_Creat(&parameters_test, "ABCDEFh\0", PARAMETERS_TYPE_F64, &value_2);

    printf("Parameters_Creat out %f\n",
           *((double *)(Parameters_Creat(&parameters_test, "ABCDEFh\0", PARAMETERS_TYPE_F64, &value_1))));

    double value_3 = 104.0f;
    Parameters_Creat(&parameters_test, "ABCDEFj\0", PARAMETERS_TYPE_F64, &value_3);

    printf("Parameters_Creat out %f\n",
           *((double *)(Parameters_Creat(&parameters_test, "ABCDEFj\0", PARAMETERS_TYPE_F64, &value_1))));

    unsigned int count;
    printf("simulation_ROM :---------------------\n");
    for (count = 0; count < sizeof(simulation_ROM); count += 16)
    {
        printf("%x %x %x %x ", simulation_ROM[count], simulation_ROM[count + 1], simulation_ROM[count + 2], simulation_ROM[count + 3]);
        printf("%x %x %x %x ", simulation_ROM[count + 4], simulation_ROM[count + 5], simulation_ROM[count + 6], simulation_ROM[count + 7]);
        printf("%x %x %x %x ", simulation_ROM[count + 8], simulation_ROM[count + 9], simulation_ROM[count + 10], simulation_ROM[count + 11]);
        printf("%x %x %x %x\n", simulation_ROM[count + 12], simulation_ROM[count + 13], simulation_ROM[count + 14], simulation_ROM[count + 15]);
    }

    printf("\nsimulation_RAM :---------------------\n");
    for (count = 0; count < sizeof(simulation_RAM); count += 16)
    {
        printf("%x %x %x %x ", simulation_RAM[count], simulation_RAM[count + 1], simulation_RAM[count + 2], simulation_RAM[count + 3]);
        printf("%x %x %x %x ", simulation_RAM[count + 4], simulation_RAM[count + 5], simulation_RAM[count + 6], simulation_RAM[count + 7]);
        printf("%x %x %x %x ", simulation_RAM[count + 8], simulation_RAM[count + 9], simulation_RAM[count + 10], simulation_RAM[count + 11]);
        printf("%x %x %x %x\n", simulation_RAM[count + 12], simulation_RAM[count + 13], simulation_RAM[count + 14], simulation_RAM[count + 15]);
    }

    Parameters_Del(&parameters_test, "ABCDEFh\0");

    printf("simulation_ROM :---------------------\n");
    for (count = 0; count < sizeof(simulation_ROM); count += 16)
    {
        printf("%x %x %x %x ", simulation_ROM[count], simulation_ROM[count + 1], simulation_ROM[count + 2], simulation_ROM[count + 3]);
        printf("%x %x %x %x ", simulation_ROM[count + 4], simulation_ROM[count + 5], simulation_ROM[count + 6], simulation_ROM[count + 7]);
        printf("%x %x %x %x ", simulation_ROM[count + 8], simulation_ROM[count + 9], simulation_ROM[count + 10], simulation_ROM[count + 11]);
        printf("%x %x %x %x\n", simulation_ROM[count + 12], simulation_ROM[count + 13], simulation_ROM[count + 14], simulation_ROM[count + 15]);
    }

    printf("\nsimulation_RAM :---------------------\n");
    for (count = 0; count < sizeof(simulation_RAM); count += 16)
    {
        printf("%x %x %x %x ", simulation_RAM[count], simulation_RAM[count + 1], simulation_RAM[count + 2], simulation_RAM[count + 3]);
        printf("%x %x %x %x ", simulation_RAM[count + 4], simulation_RAM[count + 5], simulation_RAM[count + 6], simulation_RAM[count + 7]);
        printf("%x %x %x %x ", simulation_RAM[count + 8], simulation_RAM[count + 9], simulation_RAM[count + 10], simulation_RAM[count + 11]);
        printf("%x %x %x %x\n", simulation_RAM[count + 12], simulation_RAM[count + 13], simulation_RAM[count + 14], simulation_RAM[count + 15]);
    }
}