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
#ifndef _AT_MODULE_H_
#define _AT_MODULE_H_

#define AT_MODULE_DATA_MAX_LEN      1024

typedef struct at_module_data_struct
{
    uint16_t data_rec_index;
    uint8_t  state;
    unsigned char data[AT_MODULE_DATA_MAX_LEN];
} at_module_data_t;

enum
{
    AT_MODULE_REC_STATE_WAIT_DATA = 0,
    AT_MODULE_REC_STATE_HANDLING,
    AT_MODULE_REC_STATE_CONTINUE_RECEIVE,
    AT_MODULE_REC_STATE_EXIT,
};

typedef struct at_module_process_struct
{
    uint8_t state;
    uint8_t sendRepeatTimesSet;
    uint8_t sendRepeatTimes;
    uint8_t corfirm_cnt;
    void (*dataHandler)(at_module_data_t *rec_data);
    struct at_module_process_struct *prev_process;
    struct at_module_process_struct *next_process;
} at_module_process_t;

extern void at_module_send_data_to_process(at_module_process_t *head_process, at_module_data_t *rec_data);

extern void at_module_add_process(at_module_process_t *head_process, at_module_process_t *wait_for_add);

extern void at_module_delete_process(at_module_process_t *wait_for_delete);

extern uint8_t at_module_is_process_running(at_module_process_t *head_process, at_module_process_t *wait_for_add);

extern void at_module_startup_process_Init(at_module_process_t *head_process,  uint8_t sendRepeatTimesSet, void (*dataHandler)(at_module_data_t *rec_data));

#define AT_BUFFER_NO_MATCH         0xffff

extern uint16_t at_module_buffer_match_search(unsigned char *det, uint16_t det_len, unsigned char *part, uint16_t part_len, uint8_t max_len, uint8_t direction);

#endif /* _AT_MODULE_H_ */
