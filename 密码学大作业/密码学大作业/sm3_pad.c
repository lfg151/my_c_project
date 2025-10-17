#include "sm3_pad.h"
#include <stdio.h>

// 计算填充所需的0比特数（返回值：字节数）
static size_t calculate_zero_bytes(size_t original_bits) {
    // 公式：l + 1 + k ≡ 448 mod 512 → k = (448 - (l + 1) % 512) mod 512
    size_t k_bits = (448 - (original_bits + 1) % 512) % 512;
    return k_bits / 8;  // 转换为字节数（k_bits必为8的倍数）
}

Sm3PaddedData sm3_pad(const uint8_t* input, size_t input_len) {
    Sm3PaddedData result = { NULL, 0 };
    if (input == NULL && input_len != 0) {
        return result;  // 输入为空指针且长度非0，返回错误
    }

    // 1. 计算原始消息长度（单位：bit）
    size_t original_bits = input_len * 8;

    // 2. 计算填充后总长度（单位：字节）
    // 总比特数 = original_bits + 1（补1） + k_bits（补0） + 64（补长度）
    size_t zero_bytes = calculate_zero_bytes(original_bits);
    result.len = input_len + 1 + zero_bytes + 8;  // 1字节补1 + zero_bytes补0 + 8字节存长度

    // 3. 分配内存并填充数据
    result.data = (uint8_t*)malloc(result.len);
    if (result.data == NULL) {
        result.len = 0;
        return result;
    }

    // 3.1 复制原始数据
    if (input_len > 0) {
        memcpy(result.data, input, input_len);
    }

    // 3.2 补1（0x80 = 10000000B）
    result.data[input_len] = 0x80;

    // 3.3 补0（从input_len+1到input_len+1+zero_bytes-1）
    if (zero_bytes > 0) {
        memset(result.data + input_len + 1, 0x00, zero_bytes);
    }

    // 3.4 补原始长度（64bit，大端序存储）
    uint64_t len_bits = original_bits;
    // 大端序：高位字节在前
    result.data[result.len - 8] = (len_bits >> 56) & 0xFF;
    result.data[result.len - 7] = (len_bits >> 48) & 0xFF;
    result.data[result.len - 6] = (len_bits >> 40) & 0xFF;
    result.data[result.len - 5] = (len_bits >> 32) & 0xFF;
    result.data[result.len - 4] = (len_bits >> 24) & 0xFF;
    result.data[result.len - 3] = (len_bits >> 16) & 0xFF;
    result.data[result.len - 2] = (len_bits >> 8) & 0xFF;
    result.data[result.len - 1] = len_bits & 0xFF;

    return result;
}

void sm3_pad_free(Sm3PaddedData* padded_data) {
    if (padded_data != NULL && padded_data->data != NULL) {
        free(padded_data->data);
        padded_data->data = NULL;
        padded_data->len = 0;
    }
}