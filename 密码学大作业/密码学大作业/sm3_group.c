#include "sm3_group.h"
#include <assert.h>
#include <stdio.h>

// ѭ�����ƺ���
static uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// �û�����P1
static uint32_t P1(uint32_t x) {
    return x ^ rotl(x, 15) ^ rotl(x, 23);
}

int sm3_check_padded_length(const Sm3PaddedData* padded_data) {
    if (padded_data == NULL) return 0;
    // �����ܳ��ȱ�����512bit��64�ֽڣ���������
    return (padded_data->len % 64 == 0) ? 1 : 0;
}

uint8_t** sm3_split_groups(const Sm3PaddedData* padded_data, size_t* group_count) {
    *group_count = 0;
    if (padded_data == NULL || padded_data->data == NULL || padded_data->len == 0) {
        return NULL;
    }

    // ���ԣ����󳤶ȱ�����64�ֽڵ������������������ģ���ѱ�֤��
    assert(sm3_check_padded_length(padded_data) == 1 && "Padded data length is not 512bit aligned");

    // �����������
    *group_count = padded_data->len / 64;
    if (*group_count == 0) {
        return NULL;
    }

    // ������������ڴ�
    uint8_t** groups = (uint8_t**)malloc(*group_count * sizeof(uint8_t*));
    if (groups == NULL) {
        *group_count = 0;
        return NULL;
    }

    // ��ַ��飨ÿ������64�ֽڣ�
    for (size_t i = 0; i < *group_count; i++) {
        groups[i] = (uint8_t*)malloc(64 * sizeof(uint8_t));
        if (groups[i] == NULL) {
            // �ڴ����ʧ��ʱ�ͷ��ѷ���ķ���
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

    // 1. ��ʼ��W[0]~W[15]���ӷ�������ȡ32bit�֣������
    for (int j = 0; j < 16; j++) {
        // ÿ��W[j]��4���ֽ���ɣ�����򣺸�λ�ֽ���ǰ��
        expanded->W[j] = (uint32_t)group[j * 4] << 24 |
            (uint32_t)group[j * 4 + 1] << 16 |
            (uint32_t)group[j * 4 + 2] << 8 |
            (uint32_t)group[j * 4 + 3];
    }

    // 2. ����W[16]~W[67]
    for (int j = 16; j < 68; j++) {
        uint32_t temp = expanded->W[j - 16] ^ expanded->W[j - 9] ^ rotl(expanded->W[j - 3], 15);
        expanded->W[j] = P1(temp) ^ rotl(expanded->W[j - 13], 7) ^ expanded->W[j - 6];
    }

    // 3. ����W'[0]~W'[63]
    for (int j = 0; j < 64; j++) {
        expanded->W1[j] = expanded->W[j] ^ expanded->W[j + 4];
    }
}