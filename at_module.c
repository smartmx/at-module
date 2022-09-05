/*
 * Copyright (c) 2022, smartmx <smartmx@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-22     smartmx      the first version
 *
 */
#include "at_module.h"

void at_module_send_data_to_process(at_module_process_t *head_process, at_module_data_t *rec_data)
{
    at_module_process_t *data_send_to_process_temp;
    if (head_process == NULL)      /* 头结点不可为空 */
    {
        return;
    }
    rec_data->state = AT_MODULE_REC_STATE_HANDLING;
    data_send_to_process_temp = head_process;
    while (1)
    {
        if(data_send_to_process_temp->dataHandler != NULL)
        {
            data_send_to_process_temp->dataHandler(rec_data);           /* 调用不同进程的dataHandler */
        }
        if((rec_data->data_rec_index == 0) || (rec_data->state != AT_MODULE_REC_STATE_HANDLING))
        {
            /* 线程接收了数据 */
            break;
        }
        if(data_send_to_process_temp->next_process != NULL)
        {
            data_send_to_process_temp = data_send_to_process_temp->next_process;
        }
        else
        {
            break;  /* 直到进程链表结束 */
        }
    }

    /* 没有线程认领数据，丢弃 */
    if (rec_data->state == AT_MODULE_REC_STATE_HANDLING)
    {
        rec_data->data_rec_index = 0;
        rec_data->state = AT_MODULE_REC_STATE_WAIT_DATA;
    }
    else if (rec_data->state == AT_MODULE_REC_STATE_EXIT)
    {
        /* 退出线程 */
        rec_data->data_rec_index = 0;
        rec_data->state = AT_MODULE_REC_STATE_WAIT_DATA;
        at_module_delete_process(data_send_to_process_temp);
    }
}

void at_module_add_process(at_module_process_t *head_process, at_module_process_t *wait_for_add)
{
    at_module_process_t *add_process_temp = head_process;
    if (head_process == NULL)   /* 头结点不可为空 */
    {
        return;
    }
    while ((add_process_temp->next_process) != NULL)
    {
        if(wait_for_add == add_process_temp)
        {
            return; /* 已经在链表中 */
        }
        add_process_temp = add_process_temp->next_process;
    }
    add_process_temp->next_process = wait_for_add;
    wait_for_add->prev_process = add_process_temp;
    wait_for_add->next_process = NULL;
}


void at_module_delete_process(at_module_process_t *wait_for_delete)  /* 头结点不可删除。 */
{
    if (wait_for_delete->prev_process == NULL)
    {
        /* 无上一个结点，为NULL，此节点不在链表中，或者为头结点。直接返回。 */
        return;
    }
    wait_for_delete->prev_process->next_process = wait_for_delete->next_process;    /* 解绑上一个节点 */
    if (wait_for_delete->next_process != NULL)
    {
        wait_for_delete->next_process->prev_process = wait_for_delete->prev_process;    /* 解绑下一个节点 */
    }
    wait_for_delete->prev_process = NULL;
    wait_for_delete->next_process = NULL;
}

/*
 * 函数功能：在字符串中查找另一个字符串
 * 参数1：源字符串首地址
 * 参数2：源字符串长度
 * 参数3: 待查找字符串首地址
 * 参数4：待查找字符串长度
 * 参数5：最多查找到开始的几个字符
 * 参数6：查找的方向，从前还是从后
 * 返回值：出现的位置，返回0xffff则说明没有找到。
 */
uint16_t at_module_buffer_match_search(unsigned char *det, uint16_t det_len, unsigned char *part, uint16_t part_len, uint8_t max_len, uint8_t direction)
{
    uint16_t i, j;
    if (det_len >= part_len)    /* 比较长度 */
    {
        if (direction == 1)
        {
            for (i = 0; i <= det_len - part_len; i++)   /* 从源字符串长度开始查找 */
            {
                if (det[i] == part[0])
                {
                    for (j = 1; j < part_len; j++)
                    {
                        if (det[i + j] != part[j])      /* 一一比对 */
                        {
                            break;
                        }
                    }
                    if (j == part_len)  /* 匹配成功 */
                    {
                        return i;       /* 返回det中查找到part的的字符位置 */
                    }
                }
                if(i >= max_len)
                {
                    break;
                }
            }
        }
        else if (direction == 0)
        {
            /* 等于0，反向 */
            if((det_len - part_len) < max_len)
            {
                max_len = 0;
            }
            else
            {
                max_len = det_len - part_len - max_len;
            }
            for (i = det_len - part_len; i >= 0; i--)       /* 从源字符串长度开始查找 */
            {
                if (det[i] == part[0])
                {
                    for (j = 1; j < part_len; j++)
                    {
                        if (det[i + j] != part[j])      /* 一一比对 */
                        {
                            break;
                        }
                    }
                    if (j == part_len)  /* 匹配成功 */
                    {
                        return i;       /* 返回det中查找到part的的字符位置 */
                    }
                }
                if(i <= max_len)
                {
                    break;
                }
            }
        }
    }
    return AT_BUFFER_NO_MATCH;      /* 这是不可能的值。因为part_len在控制中至少2个字节 */
}
