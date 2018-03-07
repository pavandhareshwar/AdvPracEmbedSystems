#ifndef IPC_COMM_H
#define IPC_COMM_H

#define IPC_MSG_SIZE        (256)

enum led_ctrl
{
    LED_ON,
    LED_OFF
};

struct ipc_msg 
{
    char message[IPC_MSG_SIZE];
    int msg_len;
};

struct ipc_msg_struct
{
    enum led_ctrl usr_led_ctrl;
    struct ipc_msg msg; 
};

#endif
