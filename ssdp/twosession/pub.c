#include "ssdp_messageQ.h"

int main(int argc, char const *argv[])
{
    char *ip = "127.0.0.1";
    char* port = "2021";
    ssdp_init_udp(ip, port, "HelloWorldTopic", "o", 0x1, 0x2);
    printf("init done!\n");
    for(int i=0;i<10000;i++){
        ssdp_udp_sent_data("my world!");
    }
    return 0;
}
