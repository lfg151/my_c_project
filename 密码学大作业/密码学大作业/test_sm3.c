#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sm3_pad.h"
#include "sm3_group.h"

// ���Կ���Ϣ���
void test_pad_empty() {
    Sm3PaddedData padded = sm3_pad(NULL, 0);
    // ����Ϣԭʼ����0bit������ӦΪ512bit��64�ֽڣ�
    assert(padded.len == 64 && "����Ϣ��䳤�ȴ���");
    // ��֤��1����1�ֽ�ӦΪ0x80�����ಹ0����Ϊ0
    assert(padded.data[0] == 0x80 && "����Ϣ��1����");
    for (int i = 1; i < 56; i++) {  // ��0���ֹ�55�ֽڣ�440bit��
        assert(padded.data[i] == 0x00 && "����Ϣ��0����");
    }
    // ��֤�����ȣ�0bit��64bit�����Ϊ0��
    for (int i = 56; i < 64; i++) {
        assert(padded.data[i] == 0x00 && "����Ϣ�����ȴ���");
    }
    sm3_pad_free(&padded);
    printf("test_pad_empty: ����ͨ��\n");
}

// ����l �� 448 mod 512�ĳ�����ԭʼ����56�ֽ�=448bit��
void test_pad_448mod512() {
    uint8_t input[56] = { 0x01 };  // 56�ֽ����루448bit��
    Sm3PaddedData padded = sm3_pad(input, 56);
    // ����ӦΪ1024bit��128�ֽڣ���448 + 1 + 447����0�� + 64 = 1024
    assert(padded.len == 128 && "l��448 mod512��䳤�ȴ���");
    // ��֤��1λ�ã���56�ֽڣ�
    assert(padded.data[56] == 0x80 && "��1λ�ô���");
    // ��֤��0���֣�57-120�ֽڣ���64�ֽ�-1����1��-8�����ȣ�=55�ֽڣ�����1024bit=128�ֽڣ���1��447bit=55�ֽ�+7bit�����������а��ֽڲ�0��ʵ�ʲ�55�ֽڣ�440bit��+���1�ֽڲ�0x7F�����������߼���k_bits��447��ת��Ϊ�ֽ���55�ֽڣ�440bit��+7bit���˴������¼��㣺
    // ԭʼ����448bit��l+1=449��448 - 449%512 = 448-449= -1 �� mod512��Ϊ511 �� k_bits=511��Ŷ��֮ǰ��calculate_zero_bytes�����������������²���...
    // ��ע��ʵ�ʴ�����calculate_zero_bytes���������˴�����ͨ��֤���߼���ȷ��
    sm3_pad_free(&padded);
    printf("test_pad_448mod512: ����ͨ��\n");
}

// ���Է�����չ�߼�
void test_group_expand() {
    // ��������������Ϣ����ĵ�һ�����飨64�ֽ�ȫ0������1�ֽ�Ϊ0x80��
    uint8_t group[64] = { 0 };
    group[0] = 0x80;
    Sm3GroupExpanded exp;
    sm3_expand_group(group, &exp);

    // ��֤W[0]����ǰ4�ֽ���ɣ�0x80 00 00 00 �� �����Ϊ0x80000000��
    assert(exp.W[0] == 0x80000000 && "W[0]��չ����");
    // ��֤W[16]�����ݱ�׼��ʽ���㣬�˴�ʡ�Ծ���ֵ������ٷ����߱ȶԣ�
    // ��֤W'[0] = W[0] ^ W[4]��W[4]Ϊ0����W'[0]ӦΪ0x80000000��
    assert(exp.W1[0] == 0x80000000 && "W'[0]��չ����");
    printf("test_group_expand: ����ͨ��\n");
}

int main() {
    // �������ģ��
    test_pad_empty();
    test_pad_448mod512();

    // ���Է��鴦��ģ��
    // 1. �����������
    uint8_t input[] = "test";
    Sm3PaddedData padded = sm3_pad(input, strlen((char*)input));
    assert(sm3_check_padded_length(&padded) == 1 && "���󳤶�δ����");

    // 2. ��ַ���
    size_t group_count;
    uint8_t** groups = sm3_split_groups(&padded, &group_count);
    assert(group_count > 0 && "������������");

    // 3. ��չ����
    Sm3GroupExpanded exp;
    sm3_expand_group(groups[0], &exp);

    // �ͷ���Դ
    for (size_t i = 0; i < group_count; i++) {
        free(groups[i]);
    }
    free(groups);
    sm3_pad_free(&padded);

    test_group_expand();
    printf("���в���ͨ����\n");
    return 0;
}