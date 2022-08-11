/*
 * @Description    : Parameters
 * @Author         : Aiyangsky
 * @Date           : 2022-08-08 12:03:31
 * @LastEditors    : Aiyangsky
 * @LastEditTime   : 2022-08-11 13:24:51
 * @FilePath       : \SparrowSkyFlightControl\SRC\moduldes\Parameters\src\Parameters.h
 */

#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define EMPTY_BYTE 0X00

    typedef enum
    {
        PARAMETERS_TYPE_UINT8 = 1,
        PARAMETERS_TYPE_INT8,
        PARAMETERS_TYPE_UINT16,
        PARAMETERS_TYPE_INT16,
        PARAMETERS_TYPE_UINT32,
        PARAMETERS_TYPE_INT32,
        PARAMETERS_TYPE_UINT64,
        PARAMETERS_TYPE_INT64,
        PARAMETERS_TYPE_F32,
        PARAMETERS_TYPE_F64,
    } PARAMETERS_TYPE_T;

#pragma pack(1)
    typedef struct
    {
        char table_tag[16];
        unsigned short used_index;
        unsigned short check_value;
    } PARAMETERS_TABLE_INFO_T;
#pragma pack()

    typedef struct
    {
        unsigned char *block_start;
        unsigned int ROM_start_offset;
        unsigned int block_size;

        PARAMETERS_TABLE_INFO_T table_info;

        // callback list
        /**
         * @description:                    Read data from ROM
         * @param       {unsigned char} *   The start address of read data
         * @param       {unsigned} int      The offset in ROM of data to be read
         * @param       {unsigned} int      The size to be read
         * @return      {*}                 Read success or fail
         * @note       :
         */
        bool (*Read_From_ROM)(unsigned char *, unsigned int, unsigned int);

        /**
         * @description:                    Write data to ROM
         * @param       {unsigned char} *   The data start address of write
         * @param       {unsigned} int      The offset in ROM of data should be write
         * @param       {unsigned} int      The size shoude be write
         * @return      {*}                 Write success or fail
         * @note       :
         */
        bool (*Write_2_ROM)(unsigned char *, unsigned int, unsigned int);

        /**
         * @description:                    data checkout
         * @param       {unsigned char} *   check start address
         * @param       {unsigned} int      check size
         * @return      {*}                 value of check
         * @note       :
         */
        unsigned short (*checkout)(unsigned char *, unsigned int);

    } PARAMETERS_CB_T;

    bool Parameters_Init(PARAMETERS_CB_T *moudule, char *table_tag,
                         unsigned char *RAM_block, unsigned int ROM_block, unsigned int size,
                         bool (*Read_From_ROM)(unsigned char *, unsigned int, unsigned int),
                         bool (*Write_2_ROM)(unsigned char *, unsigned int, unsigned int),
                         unsigned short (*checkout)(unsigned char *, unsigned int));

    void *Parameters_Creat(PARAMETERS_CB_T *moudule, char *name, PARAMETERS_TYPE_T type, void *default_value);
    void *Parameters_Chanege(PARAMETERS_CB_T *moudule, char *name, void *value);
    bool Parameters_Del(PARAMETERS_CB_T *moudule, char *name);
    void Parameters_Get_name(PARAMETERS_CB_T *moudule, unsigned short index, char *name);
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_PARAMETERS_H
