/*
 * ssdp_messageQ.h
 *
 */

#ifndef SRC_SSDP_MESSAGEQ_H_
#define SRC_SSDP_MESSAGEQ_H_
#include <stdbool.h>
#include "HelloWorld.h"
#include <uxr/client/client.h>
#include <ucdr/microcdr.h>

#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <unistd.h>

#define STREAM_HISTORY  8
#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU* STREAM_HISTORY

bool ssdp_init_udp(char *ip,char* port , const char *send_topic_name, const char* recv_topic_name, int key_num);
bool ssdp_pub_topic(char *topic);
bool ssdp_sub_topic(char *topic);
int ssdp_udp_sent_data(char *reVal);
int ssdp_udp_recv_data(char *buf); //return the length of buffer

#endif /* SRC_SSDP_MESSAGEQ_H_ */