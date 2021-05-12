#include <stdio.h>
#include "fpga.h"


int main(int args, char** argv)
{
    /* code */
    // CLI
    if (3 > args || 0 == atoi(argv[2]))
    {
        printf("usage: program [-h | --help] | ip port [<max_topics>]\n");
        return 0;
    }

    char* ip = argv[1];
    char* port = argv[2];
    uint32_t max_topics = (args == 4) ? (uint32_t)atoi(argv[3]) : UINT32_MAX;
    
    char buf[64];
    ssdp_init_client(ip, port, "a", "b");

    
   printf("before receive data!\n");
   ssdp_receive_data(buf, 64);
   //ssdp_receive_data(buf, 64);



    return 0;
}
