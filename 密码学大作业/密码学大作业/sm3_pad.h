#pragma once
#ifndef SM3_PAD_H
#define SM3_PAD_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// 填充后的数据结构（含字节流与长度）
typedef struct {
    uint8_t* data;     // 填充后的字节流
    size_t len;        // 填充后的总长度（单位：字节）
} Sm3PaddedData;

/**
 * 消息填充函数（符合GM/T 0004-2012标准）
 * @param input 原始输入字节流
 * @param input_len 原始输入长度（单位：字节）
 * @return 填充后的数据（需调用sm3_pad_free释放内存）
 */
Sm3PaddedData sm3_pad(const uint8_t* input, size_t input_len);

/**
 * 释放填充后的数据内存
 * @param padded_data 填充后的数据
 */
void sm3_pad_free(Sm3PaddedData* padded_data);

#endif // SM3_PAD_H