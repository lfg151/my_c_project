#pragma once
#ifndef SM3_PAD_H
#define SM3_PAD_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ��������ݽṹ�����ֽ����볤�ȣ�
typedef struct {
    uint8_t* data;     // ������ֽ���
    size_t len;        // ������ܳ��ȣ���λ���ֽڣ�
} Sm3PaddedData;

/**
 * ��Ϣ��亯��������GM/T 0004-2012��׼��
 * @param input ԭʼ�����ֽ���
 * @param input_len ԭʼ���볤�ȣ���λ���ֽڣ�
 * @return ��������ݣ������sm3_pad_free�ͷ��ڴ棩
 */
Sm3PaddedData sm3_pad(const uint8_t* input, size_t input_len);

/**
 * �ͷ�����������ڴ�
 * @param padded_data ���������
 */
void sm3_pad_free(Sm3PaddedData* padded_data);

#endif // SM3_PAD_H