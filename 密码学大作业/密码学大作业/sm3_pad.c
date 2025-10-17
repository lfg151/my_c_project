#include "sm3_pad.h"
#include <stdio.h>

// ������������0������������ֵ���ֽ�����
static size_t calculate_zero_bytes(size_t original_bits) {
    // ��ʽ��l + 1 + k �� 448 mod 512 �� k = (448 - (l + 1) % 512) mod 512
    size_t k_bits = (448 - (original_bits + 1) % 512) % 512;
    return k_bits / 8;  // ת��Ϊ�ֽ�����k_bits��Ϊ8�ı�����
}

Sm3PaddedData sm3_pad(const uint8_t* input, size_t input_len) {
    Sm3PaddedData result = { NULL, 0 };
    if (input == NULL && input_len != 0) {
        return result;  // ����Ϊ��ָ���ҳ��ȷ�0�����ش���
    }

    // 1. ����ԭʼ��Ϣ���ȣ���λ��bit��
    size_t original_bits = input_len * 8;

    // 2. ���������ܳ��ȣ���λ���ֽڣ�
    // �ܱ����� = original_bits + 1����1�� + k_bits����0�� + 64�������ȣ�
    size_t zero_bytes = calculate_zero_bytes(original_bits);
    result.len = input_len + 1 + zero_bytes + 8;  // 1�ֽڲ�1 + zero_bytes��0 + 8�ֽڴ泤��

    // 3. �����ڴ沢�������
    result.data = (uint8_t*)malloc(result.len);
    if (result.data == NULL) {
        result.len = 0;
        return result;
    }

    // 3.1 ����ԭʼ����
    if (input_len > 0) {
        memcpy(result.data, input, input_len);
    }

    // 3.2 ��1��0x80 = 10000000B��
    result.data[input_len] = 0x80;

    // 3.3 ��0����input_len+1��input_len+1+zero_bytes-1��
    if (zero_bytes > 0) {
        memset(result.data + input_len + 1, 0x00, zero_bytes);
    }

    // 3.4 ��ԭʼ���ȣ�64bit�������洢��
    uint64_t len_bits = original_bits;
    // ����򣺸�λ�ֽ���ǰ
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