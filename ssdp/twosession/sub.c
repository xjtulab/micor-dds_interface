#include "ssdp_messageQ.h"

int main(int argc, char const *argv[])
{
    char *ip = "127.0.0.1";
    char* port = "2021";
    ssdp_init_udp(ip, port, "other", "HelloWorldTopic", 0x1, 0x2);
    char recv_buff[255];
    for(int i=0;i<10000;i++){
        ssdp_udp_recv_data(recv_buff);
        printf("recv_buf is %s\n", recv_buff);
    }
    return 0;
}
