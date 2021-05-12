

#include "fpga.h"


#define STREAM_HISTORY  8
#define BUFFER_SIZE     UXR_CONFIG_UDP_TRANSPORT_MTU* STREAM_HISTORY



uxrSession session;
uxrStreamId reliable_out;
uxrStreamId reliable_in;
uxrObjectId datawriter_id;
uxrObjectId datareader_id;
uint16_t read_data_req; 
int count = 0;


void on_topic(
        uxrSession* session,
        uxrObjectId object_id,
        uint16_t request_id,
        uxrStreamId stream_id,
        struct ucdrBuffer* ub,
        uint16_t length,
        void* args)
{
    (void) session; (void) object_id; (void) request_id; (void) stream_id; (void) length;

    HelloWorld topic;
    HelloWorld_deserialize_topic(ub, &topic);

    printf("Received topic: %s, id: %i\n", topic.message, topic.index);

    uint32_t* count_ptr = (uint32_t*) args;
    (*count_ptr)++;
}

int ssdp_init_client(char* ip, char* port, const char* stopic, const char* rtopic){
        // Transport
    uxrUDPTransport transport;
    if (!uxr_init_udp_transport(&transport, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return 1;
    }

    // Session
    uxr_init_session(&session, &transport.comm, 0xAAAABBBB);
    uxr_set_topic_callback(&session, on_topic, &count);
    if (!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    // Streams
    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    reliable_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);
    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    reliable_in = uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);

    // Create entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
            "<participant>"
            "<rtps>"
            "<name>default_xrce_participant</name>"
            "</rtps>"
            "</participant>"
            "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, reliable_out, participant_id, 0,
                    participant_xml, UXR_REPLACE);

    uxrObjectId send_topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* send_topic_xml = "<dds>"
            "<topic>"
            "<name>HelloWorldTopic1</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</dds>";
    uint16_t send_topic_req = uxr_buffer_create_topic_xml(&session, reliable_out, send_topic_id, participant_id, send_topic_xml,
                    UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, reliable_out, publisher_id, participant_id,
                    publisher_xml, UXR_REPLACE);

    datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* datawriter_xml = "<dds>"
            "<data_writer>"
            "<topic>"
            "<kind>NO_KEY</kind>"
            "<name>HelloWorldTopic1</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</data_writer>"
            "</dds>";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id,
                    datawriter_xml, UXR_REPLACE);


    /* init topic 2*/
    uxrObjectId recv_topic_id = uxr_object_id(0x02, UXR_TOPIC_ID);
    const char* recv_topic_xml = "<dds>"
            "<topic>"
            "<name>HelloWorldTopic1</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</dds>";
    uint16_t recv_topic_req = uxr_buffer_create_topic_xml(&session, reliable_out, recv_topic_id, participant_id, recv_topic_xml,
                    UXR_REPLACE);
    uxrObjectId subscriber_id = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
    const char* subscriber_xml = "";
    uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&session, reliable_out, subscriber_id, participant_id,
                    subscriber_xml, UXR_REPLACE);

    uxrObjectId datareader_id = uxr_object_id(0x01, UXR_DATAREADER_ID);
    const char* datareader_xml = "<dds>"
            "<data_reader>"
            "<topic>"
            "<kind>NO_KEY</kind>"
            "<name>HelloWorldTopic2</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</data_reader>"
            "</dds>";
    uint16_t datareader_req = uxr_buffer_create_datareader_xml(&session, reliable_out, datareader_id, subscriber_id,
                    datareader_xml, UXR_REPLACE);

            

    // Send create entities message and wait its status
    uint8_t status[7];
    uint16_t requests[7] = {
        participant_req, send_topic_req, recv_topic_req, publisher_req, subscriber_req, datareader_req, datawriter_req
    };


    if (!uxr_run_session_until_all_status(&session, 1000, requests, status, 7))
    {
        printf("Error at create entities: participant: %i topic: %i subscriber: %i datareader: %i\n", status[0],
                status[1], status[2], status[3]);
        return 1;
    }
    
    uxrDeliveryControl delivery_control = {
        0
    };
     delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    read_data_req = uxr_buffer_request_data(&session, reliable_out, datareader_id, reliable_in,
                    &delivery_control);


    return 0;
}


void ssdp_send_data(char* message, int len){
    // Write topics
    bool connected = true;
    HelloWorld topic;
    topic.index = count++;
    strcpy(topic.message, message);

    ucdrBuffer ub;
    uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);
    uxr_prepare_output_stream(&session, reliable_out, datawriter_id, &ub, topic_size);
    HelloWorld_serialize_topic(&ub, &topic);
    printf("Send topic: %s, id: %i\n", topic.message, topic.index);
    connected = uxr_run_session_time(&session, 1000);
    if(connect == false){
        printf("connected failed\n");
    }
    //set receive
    //uint8_t read_data_status;
    //connected = uxr_run_session_until_all_status(&session, UXR_TIMEOUT_INF, &read_data_req, &read_data_status, 1);

    


    
}

void ssdp_receive_data(char* buf, int maxlen){
    uint8_t read_data_status;
    bool connect = false;
    while(connect == false){
        sleep(1);
        bool connected = uxr_run_session_until_all_status(&session, UXR_TIMEOUT_INF, &read_data_req, &read_data_status, 1);
    }
    printf("Received data done!");
    
}