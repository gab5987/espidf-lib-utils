/**
 * @file utils.h
 * @brief Utils functions
 * @date Thu Aug 17 2023
 *
 * @copyright Gabriel Novalski @ MarineTelematics Equipamentos Nauticos
 */

#ifndef __UTILS_H__
#define __UTILS_H__

/**
 * @brief Macro patter to print a byte in binary
 */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"

/**
 * @brief Macro to print a byte in binary
 *
 * @param byte Byte to print
 */
#define BYTE_TO_BINARY(byte)       \
    ((byte)&0x80 ? '1' : '0'),     \
        ((byte)&0x40 ? '1' : '0'), \
        ((byte)&0x20 ? '1' : '0'), \
        ((byte)&0x10 ? '1' : '0'), \
        ((byte)&0x08 ? '1' : '0'), \
        ((byte)&0x04 ? '1' : '0'), \
        ((byte)&0x02 ? '1' : '0'), \
        ((byte)&0x01 ? '1' : '0')

#endif /* __UTILS_H__ */
