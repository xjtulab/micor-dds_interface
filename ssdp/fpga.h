#ifndef _FPGA_H_
#define _FPGA_H_
#include "HelloWorld.h"

#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <unistd.h>
/* success return 0 */
int ssdp_init_client(char* ip, char* port, const char* stopic, const char* rtopic);

void ssdp_send_data(char* message, int len);

void ssdp_receive_data(char* buf, int maxlen);

#endif