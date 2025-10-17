#include "sm3_group.h"
#include <assert.h>
#include <stdio.h>

// 循环左移函数
static uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// 置换函数P1
static uint32_t P1(uint32_t x) {
    return x ^ rotl(x, 15) ^ rotl(x, 23);
}

int sm3_check_padded_length(const Sm3PaddedData* padded_data) {
    if (padded_data == NULL) return 0;
    // 填充后总长度必须是512bit（64字节）的整数倍
    return (padded_data->len % 64 == 0) ? 1 : 0;
}

uint8_t** sm3_split_groups(const Sm3PaddedData* padded_data, size_t* group_count) {
    *group_count = 0;
    if (padded_data == NULL || padded_data->data == NULL || padded_data->len == 0) {
        return NULL;
    }

    // 断言：填充后长度必须是64字节的整数倍（理论上填充模块已保证）
    assert(sm3_check_padded_length(padded_data) == 1 && "Padded data length is not 512bit aligned");

    // 计算分组数量
    *group_count = padded_data->len / 64;
    if (*group_count == 0) {
        return NULL;
    }

    // 分配分组数组内存
    uint8_t** groups = (uint8_t**)malloc(*group_count * sizeof(uint8_t*));
    if (groups == NULL) {
        *group_count = 0;
        return NULL;
    }

    // 拆分分组（每个分组64字节）
    for (size_t i = 0; i < *group_count; i++) {
        groups[i] = (uint8_t*)malloc(64 * sizeof(uint8_t));
        if (groups[i] == NULL) {
            // 内存分配失败时释放已分配的分组
            for (size_t j = 0; j < i; j++) {
                free(groups[j]);
            }
            free(groups);
            *group_count = 0;
            return NULL;
        }
        memcpy(groups[i], padded_data->data + i * 64, 64);
    }

    return groups;
}

void sm3_expand_group(const uint8_t* group, Sm3GroupExpanded* expanded) {
    if (group == NULL || expanded == NULL) return;

    // 1. 初始化W[0]~W[15]（从分组中提取32bit字，大端序）
    for (int j = 0; j < 16; j++) {
        // 每个W[j]由4个字节组成（大端序：高位字节在前）
        expanded->W[j] = (uint32_t)group[j * 4] << 24 |
            (uint32_t)group[j * 4 + 1] << 16 |
            (uint32_t)group[j * 4 + 2] << 8 |
            (uint32_t)group[j * 4 + 3];
    }

    // 2. 计算W[16]~W[67]
    for (int j = 16; j < 68; j++) {
        uint32_t temp = expanded->W[j - 16] ^ expanded->W[j - 9] ^ rotl(expanded->W[j - 3], 15);
        expanded->W[j] = P1(temp) ^ rotl(expanded->W[j - 13], 7) ^ expanded->W[j - 6];
    }

    // 3. 计算W'[0]~W'[63]
    for (int j = 0; j < 64; j++) {
        expanded->W1[j] = expanded->W[j] ^ expanded->W[j + 4];
    }
}