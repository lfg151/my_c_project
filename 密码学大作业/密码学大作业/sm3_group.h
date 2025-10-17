#pragma once
#ifndef SM3_GROUP_H
#define SM3_GROUP_H

#include <stdint.h>
#include <stdlib.h>
#include "sm3_pad.h"

// ���鴦����������չ�֣�
typedef struct {
    uint32_t W[68];    // ��չ��W_0~W_67
    uint32_t W1[64];   // ��չ��W'_0~W'_63
} Sm3GroupExpanded;

/**
 * �������������Ƿ����512bit���루���ڶ��ԣ�
 * @param padded_data ���������
 * @return 1-���ϣ�0-������
 */
int sm3_check_padded_length(const Sm3PaddedData* padded_data);

/**
 * ����������ϢΪN��512bit����
 * @param padded_data ���������
 * @param group_count �������������
 * @return �������飨ÿ������64�ֽڣ������free�ͷţ�
 */
uint8_t** sm3_split_groups(const Sm3PaddedData* padded_data, size_t* group_count);

/**
 * �Ե������������չ������W��W'��
 * @param group 512bit���飨64�ֽڣ�
 * @param expanded �������չ��Ľ��
 */
void sm3_expand_group(const uint8_t* group, Sm3GroupExpanded* expanded);

#endif // SM3_GROUP_H