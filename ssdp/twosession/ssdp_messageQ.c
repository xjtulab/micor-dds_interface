

#include "ssdp_messageQ.h"



uxrSession send_session;
uxrSession recv_session;
uxrStreamId send_reliable_out;
uxrStreamId recv_reliable_out;
uxrStreamId recv_reliable_in;
uxrObjectId datawriter_id;
uxrObjectId datareader_id;
uint16_t read_data_req;
uxrUDPTransport transport; 
//uxrUDPTransport send_transport;
//uxrUDPTransport recv_transport;
uint8_t send_output_reliable_stream_buffer[BUFFER_SIZE];
uint8_t send_input_reliable_stream_buffer[BUFFER_SIZE];

uint8_t recv_output_reliable_stream_buffer[BUFFER_SIZE];
uint8_t recv_input_reliable_stream_buffer[BUFFER_SIZE];


int count = 0;

HelloWorld topic;

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

    HelloWorld_deserialize_topic(ub, &topic);

    printf("Received topic: %s, id: %i\n", topic.message, topic.index);

    uint32_t* count_ptr = (uint32_t*) args;
    (*count_ptr)++;
}

bool ssdp_init_udp(char *ip,char* port , const char *send_topic_name, const char* recv_topic_name,  int send_key,
    int recv_key){
    // Transport
    if (!uxr_init_udp_transport(&transport, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return false;
    }

    if (!uxr_init_udp_transport(&transport, UXR_IPv4, ip, port))
    {
        printf("Error at create transport.\n");
        return false;
    }

    // Session
    uxr_init_session(&send_session, &transport.comm, send_key);
    uxr_init_session(&recv_session, &transport.comm, recv_key);
    uxr_set_topic_callback(&recv_session, on_topic, &count);
    if (!uxr_create_session(&send_session))
    {
        printf("Error at create session.\n");
        return false;
    }
    if (!uxr_create_session(&recv_session))
    {
        printf("Error at create session.\n");
        return false;
    }
    

    // Streams
    send_reliable_out = uxr_create_output_reliable_stream(&send_session, send_output_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);
    recv_reliable_out = uxr_create_output_reliable_stream(&recv_session, recv_output_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);
    recv_reliable_in = uxr_create_input_reliable_stream(&recv_session, recv_input_reliable_stream_buffer, BUFFER_SIZE,
                    STREAM_HISTORY);
                    

    // Create entities
    uxrObjectId send_participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* send_participant_xml = "<dds>"
            "<participant>"
            "<rtps>"
            "<name>default_xrce_participant</name>"
            "</rtps>"
            "</participant>"
            "</dds>";
    uint16_t send_participant_req = uxr_buffer_create_participant_xml(&send_session, send_reliable_out, send_participant_id, 0,
                    send_participant_xml, UXR_REPLACE);

    uxrObjectId recv_participant_id = uxr_object_id(0x02, UXR_PARTICIPANT_ID);
    const char* recv_participant_xml = "<dds>"
            "<participant>"
            "<rtps>"
            "<name>default_xrce_participant</name>"
            "</rtps>"
            "</participant>"
            "</dds>";
    uint16_t recv_participant_req = uxr_buffer_create_participant_xml(&recv_session, recv_reliable_out, recv_participant_id, 0,
                    recv_participant_xml, UXR_REPLACE);

    uxrObjectId send_topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* tmp_send_topic_xml = "<dds>"
            "<topic>"
            "<name>%s</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</dds>";
    char send_topic_xml[128];
    sprintf(send_topic_xml,tmp_send_topic_xml, send_topic_name);
    uint16_t send_topic_req = uxr_buffer_create_topic_xml(&send_session, send_reliable_out, send_topic_id, send_participant_id, send_topic_xml,
                    UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&send_session, send_reliable_out, publisher_id, send_participant_id,
                    publisher_xml, UXR_REPLACE);

    datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* tmp_datawriter_xml = "<dds>"
            "<data_writer>"
            "<topic>"
            "<kind>NO_KEY</kind>"
            "<name>%s</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</data_writer>"
            "</dds>";

    char datawriter_xml[128];
    sprintf(datawriter_xml, tmp_datawriter_xml, send_topic_name);
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&send_session, send_reliable_out, datawriter_id, publisher_id,
                    datawriter_xml, UXR_REPLACE);


    /* init topic 2*/
    uxrObjectId recv_topic_id = uxr_object_id(0x02, UXR_TOPIC_ID);
    const char* tmp_recv_topic_xml = "<dds>"
            "<topic>"
            "<name>%s</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</dds>";
    char recv_topic_xml[128];
    sprintf(recv_topic_xml, tmp_recv_topic_xml, recv_topic_name);
    uint16_t recv_topic_req = uxr_buffer_create_topic_xml(&recv_session, recv_reliable_out, recv_topic_id, recv_participant_id, recv_topic_xml,
                    UXR_REPLACE);
    
    uxrObjectId subscriber_id = uxr_object_id(0x01, UXR_SUBSCRIBER_ID);
    const char* subscriber_xml = "";
    uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(&recv_session, recv_reliable_out, subscriber_id, recv_participant_id,
                    subscriber_xml, UXR_REPLACE);

    uxrObjectId datareader_id = uxr_object_id(0x01, UXR_DATAREADER_ID);
    const char* tmp_datareader_xml = "<dds>"
            "<data_reader>"
            "<topic>"
            "<kind>NO_KEY</kind>"
            "<name>%s</name>"
            "<dataType>HelloWorld</dataType>"
            "</topic>"
            "</data_reader>"
            "</dds>";
    char datareader_xml[128];
    sprintf(datareader_xml, tmp_datareader_xml, recv_topic_name);
    uint16_t datareader_req = uxr_buffer_create_datareader_xml(&recv_session, recv_reliable_out, datareader_id, subscriber_id,
                    datareader_xml, UXR_REPLACE);

            
    // Send create entities message and wait its status
    uint8_t send_status[4];
    uint16_t send_requests[4] = {
        send_participant_req, send_topic_req, 
        publisher_req,  datawriter_req
    };

    /*
    if (!uxr_run_session_until_all_status(&send_session, 1000, send_requests, send_status, 4))
    {
        printf("Error at create send entities\n");
        return false;
    }
    */
    
    uint8_t recv_status[4];
    uint16_t recv_requests[4] = {
        recv_participant_req, recv_topic_req, 
        subscriber_req, datareader_req
    };

    if (!uxr_run_session_until_all_status(&recv_session, 1000, recv_requests, recv_status, 4))
    {
        printf("Error at create recv entities\n");
        return false;
    }



    
    uxrDeliveryControl delivery_control = {
        0
    };
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    read_data_req = uxr_buffer_request_data(&recv_session, recv_reliable_out, datareader_id, recv_reliable_in,
                    &delivery_control);

    return 0;
}

int ssdp_udp_sent_data(char *reVal){
        // Write topics
    bool connected = true;
    topic.index = count++;
    strcpy(topic.message, reVal);

    ucdrBuffer ub;
    uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);
    uxr_prepare_output_stream(&send_session, send_reliable_out, datawriter_id, &ub, topic_size);
    HelloWorld_serialize_topic(&ub, &topic);
    printf("Send topic: %s, id: %i\n", topic.message, topic.index);
    connected = uxr_run_session_time(&send_session, 1000);
    if(connected == false){
        printf("connected failed\n");
    }
    return connected;
}
int ssdp_udp_recv_data(char *buf){
    uint8_t read_data_status;
    bool connected = uxr_run_session_until_all_status(&recv_session, UXR_TIMEOUT_INF, &read_data_req, &read_data_status, 1);
    strcpy(buf, topic.message);
    return strlen(buf);
}

bool ssdp_pub_topic(char *topic){return true;}
bool ssdp_sub_topic(char *topic){return true;}