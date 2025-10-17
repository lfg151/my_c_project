#pragma once
#ifndef SM3_GROUP_H
#define SM3_GROUP_H

#include <stdint.h>
#include <stdlib.h>
#include "sm3_pad.h"

// 分组处理结果（含扩展字）
typedef struct {
    uint32_t W[68];    // 扩展字W_0~W_67
    uint32_t W1[64];   // 扩展字W'_0~W'_63
} Sm3GroupExpanded;

/**
 * 检查填充后的数据是否符合512bit对齐（用于断言）
 * @param padded_data 填充后的数据
 * @return 1-符合，0-不符合
 */
int sm3_check_padded_length(const Sm3PaddedData* padded_data);

/**
 * 拆分填充后的消息为N个512bit分组
 * @param padded_data 填充后的数据
 * @param group_count 输出：分组数量
 * @return 分组数组（每个分组64字节，需调用free释放）
 */
uint8_t** sm3_split_groups(const Sm3PaddedData* padded_data, size_t* group_count);

/**
 * 对单个分组进行扩展（生成W和W'）
 * @param group 512bit分组（64字节）
 * @param expanded 输出：扩展后的结果
 */
void sm3_expand_group(const uint8_t* group, Sm3GroupExpanded* expanded);

#endif // SM3_GROUP_H