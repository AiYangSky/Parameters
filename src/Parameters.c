/*
 * @Description    : Parameters.c
 * @Author         : Aiyangsky
 * @Date           : 2022-08-08 12:10:45
 * @LastEditors    : Aiyangsky
 * @LastEditTime   : 2022-08-11 12:03:42
 * @FilePath       : \Parameters\Parameters.c
 */

#include "Parameters.h"

#include <string.h>
#include <stdio.h>

#pragma pack(1)
typedef struct
{
    char name[16];
    unsigned char type;
    unsigned char data[8];
} PARAMETERS_CELL_T;
#pragma pack()

/**
 * @description:                                Synchronizing each other's cell data into RAM or ROM
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management module
 * @param       {unsigned short} index          index of cell
 * @param       {unsigned char} operate         type of operate <1> RAM to ROM   <2> ROM to RAM
 * @param       {unsigned char} max_retry       max retry count
 * @return      {*}                             success or fail
 * @note       :
 */
static bool Parameters_Cell_SYNC(PARAMETERS_CB_T *moudule, unsigned short index, unsigned char operate, unsigned char max_retry)
{
    bool status = true;
    bool (*operate_fun)(unsigned char *, unsigned int, unsigned int);

    switch (operate)
    {
    case 1:
        operate_fun = moudule->Write_2_ROM;
        break;
    case 2:
        operate_fun = moudule->Read_From_ROM;
        break;

    default:
        break;
    }

    // Address out of bounds checking
    if ((index + 1) * sizeof(PARAMETERS_CELL_T) > moudule->block_size)
    {
        printf("Parameters 0X%d index out!error!\n", moudule);
        status = false;
    }
    else
    {
        while (max_retry)
        {
            if (operate_fun(moudule->block_start + index * sizeof(PARAMETERS_CELL_T),
                            moudule->ROM_start_offset + sizeof(PARAMETERS_TABLE_INFO_T) + index * sizeof(PARAMETERS_CELL_T),
                            sizeof(PARAMETERS_CELL_T)))
            {
                break;
            }
            else
            {
                max_retry--;
            }
        }
        if (max_retry == 0)
        {
            printf("Parameters 0X%d SYNC cell:%d failed! type %d\n", moudule, index, operate);
            status = false;
        }
    }
    return status;
}

/**
 * @description:                                Synchronizing each other's moudule information into RAM or ROM
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {unsigned char} operate         type of operate <1> RAM to ROM   <2> ROM to RAM
 * @param       {unsigned char} max_retry       max retry count
 * @return      {*}                             success or fail
 * @note       :
 */
static bool Parameters_Info_SYNC(PARAMETERS_CB_T *moudule, unsigned char operate, unsigned char max_retry)
{
    bool status = true;
    bool (*operate_fun)(unsigned char *, unsigned int, unsigned int);

    switch (operate)
    {
    case 1:
        operate_fun = moudule->Write_2_ROM;
        break;
    case 2:
        operate_fun = moudule->Read_From_ROM;
        break;

    default:
        break;
    }

    while (max_retry)
    {
        if (operate_fun((unsigned char *)&moudule->table_info,
                        moudule->ROM_start_offset,
                        sizeof(PARAMETERS_TABLE_INFO_T)))
        {
            break;
        }
        else
        {
            max_retry--;
        }
    }
    if (max_retry == 0)
    {
        printf("Parameters 0X%d SYNC info failed! type :%d\n", moudule, operate);
        status = false;
    }
}

/**
 * @description:                                Search for an existing identifier cell or an empty cell
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {char} *name                    String identifier. Must end with '\0' and be no longer than 17
 * @param       {unsigned short} *index         The index of the found cell in the block
 * @return      {*}                             success or fail
 * @note       :
 */
static bool Parameters_Search(PARAMETERS_CB_T *moudule, char *name, unsigned short *index)
{
    unsigned char *search = moudule->block_start;
    bool status = false;
    char temp[17];

    *index = 0;
    temp[16] = '\0';
    while (((*index) + 1) * sizeof(PARAMETERS_CELL_T) < moudule->block_size)
    {
        memcpy(temp, search, 16);

        if (strcmp(temp, name) == 0 || temp[0] == EMPTY_BYTE)
        {
            status = true;
            break;
        }
        search += sizeof(PARAMETERS_CELL_T);
        (*index)++;
    }

    return status;
}

/**
 * @description:                                Assign values to a block of data by type
 * @param       {void} *dst                     Destation
 * @param       {void} *src                     Source
 * @param       {PARAMETERS_TYPE_T} type
 * @return      {*}                             success or fail
 * @note       :
 */
static bool Parameters_Load_value(void *dst, void *src, PARAMETERS_TYPE_T type)
{
    bool status = true;
    switch (type)
    {
    case PARAMETERS_TYPE_UINT8:
    case PARAMETERS_TYPE_INT8:
        memcpy(dst, src, 1);
        break;
    case PARAMETERS_TYPE_UINT16:
    case PARAMETERS_TYPE_INT16:
        memcpy(dst, src, 2);
        break;
    case PARAMETERS_TYPE_UINT32:
    case PARAMETERS_TYPE_INT32:
    case PARAMETERS_TYPE_F32:
        memcpy(dst, src, 4);
        break;
    case PARAMETERS_TYPE_UINT64:
    case PARAMETERS_TYPE_INT64:
    case PARAMETERS_TYPE_F64:
        memcpy(dst, src, 8);
        break;

    default:
        status = false;
        break;
    }

    return status;
}

/**
 * @description:
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {char} *table_tag               String identifier. Must end with '\0' and be no longer than 16
 * @param       {unsigned char} *RAM_block      Address of a storage block allocated in RAM
 * @param       {unsigned int} ROM_block        Offset address of a storage block allocated in ROM
 * @param       {unsigned int} size             Size of the parameter table to manage , in bytes
 * @param       {bool char} Read_From_ROM       Funtion of read data from ROM
 * @param       {bool} Write_2_ROM              Funtion of write data to ROM
 * @param       {unsigned short} checkout       Funtion of data checkout
 * @return      {*}                             success or fail
 * @note       :
 */
bool Parameters_Init(PARAMETERS_CB_T *moudule, char *table_tag, unsigned char *RAM_block, unsigned int ROM_block, unsigned int size,
                     bool (*Read_From_ROM)(unsigned char *, unsigned int, unsigned int),
                     bool (*Write_2_ROM)(unsigned char *, unsigned int, unsigned int),
                     unsigned short (*checkout)(unsigned char *, unsigned int))
{
    bool status = true;
    unsigned short index = 0;

    moudule->block_start = RAM_block;
    moudule->ROM_start_offset = ROM_block;
    moudule->block_size = size;

    memset(moudule->block_start, EMPTY_BYTE, moudule->block_size);

    moudule->Read_From_ROM = Read_From_ROM;
    moudule->Write_2_ROM = Write_2_ROM;
    moudule->checkout = checkout;

    // load table info
    status = Parameters_Info_SYNC(moudule, 2, 4);

    if (status)
    {
        if (!strcmp(moudule->table_info.table_tag, table_tag) == 0)
        {
            // This is a whole new memory area that needs to write new information
            memset(moudule->table_info.table_tag, '\0', 16);
            strcpy(moudule->table_info.table_tag, table_tag);
            moudule->table_info.used_index = 0;
            moudule->table_info.check_value = moudule->checkout(moudule->block_start, 0);

            status = Parameters_Info_SYNC(moudule, 1, 4);
        }
    }

    if (status)
    {
        // load ROM to RAM
        while (index < moudule->table_info.used_index)
        {
            status = Parameters_Cell_SYNC(moudule, index, 2, 4);
            if (status)
            {
                index++;
            }
            else
            {
                break;
            }
        }
    }

    if (status)
    {
        // checkout
        if (!moudule->checkout(moudule->block_start, index * sizeof(PARAMETERS_CELL_T)) ==
            moudule->table_info.check_value)
        {
            printf("Parameters 0X%d checkout error!\n", moudule);
            status = false;
        }
    }

    return status;
}

/**
 * @description:                                Create a storage cell based on the parameter identifier and fill it with the default value,
 *                                              returning the current stored value if the parameter already exists.
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {char} *name                    String identifier. Must end with '\0' and be no longer than 17
 * @param       {PARAMETERS_TYPE_T} type
 * @param       {void} *default_value           Address default value.
 * @return      {*}                             Address of value.
 * @note       :                                If no identifier is found and there is no space left to create a cell, NULL is returned.
 */
void *Parameters_Creat(PARAMETERS_CB_T *moudule, char *name, PARAMETERS_TYPE_T type, void *default_value)
{
    PARAMETERS_CELL_T *cell = NULL;
    void *ret = NULL;
    unsigned short index;
    if (Parameters_Search(moudule, name, &index))
    {
        cell = (PARAMETERS_CELL_T *)(moudule->block_start + index * sizeof(PARAMETERS_CELL_T));
        ret = cell->data;
        if (cell->name[0] == EMPTY_BYTE)
        {
            strcpy(cell->name, name);
            cell->type = type;

            if (!Parameters_Load_value(cell->data, default_value, type))
            {
                printf("Parameters %s type error\n", name);
            }
            else
            {
                moudule->table_info.used_index++;
                moudule->table_info.check_value = moudule->checkout(moudule->block_start, moudule->table_info.used_index * sizeof(PARAMETERS_CELL_T));

                Parameters_Cell_SYNC(moudule, index, 1, 4);
                Parameters_Info_SYNC(moudule, 1, 4);
            }
        }
    }
    else
    {
        printf("Parameters 0X%d without space\n",moudule);
    }

    return ret;
}

/**
 * @description:                                Change the value of the parameter
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {char} *name                    String identifier. Must end with '\0' and be no longer than 17
 * @param       {void} *value                   Address of value.
 * @return      {*}                             Address of value.
 * @note       :                                If the corresponding identifier is not found, NULL is returned.
 */
void *Parameters_Chanege(PARAMETERS_CB_T *moudule, char *name, void *value)
{
    PARAMETERS_CELL_T *cell = NULL;
    void *ret = NULL;
    unsigned short index;

    if (Parameters_Search(moudule, name, &index))
    {
        cell = (PARAMETERS_CELL_T *)(moudule->block_start + index * sizeof(PARAMETERS_CELL_T));
        if (cell->name[0] != EMPTY_BYTE)
        {
            ret = cell->data;
            if (!Parameters_Load_value(cell->data, value, cell->type))
            {
                printf("Parameters %s type error\n", name);
            }
            else
            {
                moudule->table_info.check_value = moudule->checkout(moudule->block_start, moudule->table_info.used_index * sizeof(PARAMETERS_CELL_T));

                Parameters_Cell_SYNC(moudule, index, 1, 4);
                Parameters_Info_SYNC(moudule, 1, 4);
            }
        }
    }

    return ret;
}

/**
 * @description:                                Delete an existing parameter storage cell
 * @param       {PARAMETERS_CB_T} *moudule      Pointer to the parameter management modules
 * @param       {char} *name                    String identifier. Must end with '\0' and be no longer than 17
 * @return      {*}                             If the corresponding identifier is not found, false is returned.
 * @note       :
 */
bool Parameters_Del(PARAMETERS_CB_T *moudule, char *name)
{
    bool status = false;
    PARAMETERS_CELL_T *cell = NULL;
    unsigned short index;
    unsigned short temp_index;

    if (Parameters_Search(moudule, name, &index))
    {
        cell = (PARAMETERS_CELL_T *)(moudule->block_start + index * sizeof(PARAMETERS_CELL_T));
        if (cell->name[0] != EMPTY_BYTE)
        {
            temp_index = index;
            memset(cell, EMPTY_BYTE, sizeof(PARAMETERS_CELL_T));
            while (temp_index < moudule->table_info.used_index)
            {
                memcpy(cell, (void *)(cell + 1), sizeof(PARAMETERS_CELL_T));
                temp_index++;
                cell++;
            }
            memset(cell, EMPTY_BYTE, sizeof(PARAMETERS_CELL_T));

            moudule->table_info.used_index--;
            moudule->table_info.check_value = moudule->checkout(moudule->block_start, moudule->table_info.used_index * sizeof(PARAMETERS_CELL_T));

            while (index <= moudule->table_info.used_index)
            {
                Parameters_Cell_SYNC(moudule, index, 1, 4);
                index++;
            }
            Parameters_Info_SYNC(moudule, 1, 4);
            status = true;
        }
    }
    return status;
}