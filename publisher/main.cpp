#include <iostream>
#include "device.h"
using namespace std;

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

    Device* dev1 = new Device("dsp");
    dev1->init_client(ip,port, "HelloWorldTopic",0xaaaabbbb);
    for(;;){
        string s;
        cin>>s;
        dev1->send_cmd(s.data());
    }


    return 0;
}
