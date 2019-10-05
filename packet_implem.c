#include "packet_interface.h"

/* Extra #includes */
#include <zlib.h>

typedef uint8_t bool;
typedef char byte;

#define bytearray byte *

struct __attribute__((__packed__)) pkt {
	ptypes_t type;
	bool tr;
	uint8_t window;
	bool l;
	uint16_t length;
	uint8_t seqnum;
	uint32_t timestamp;
	uint32_t crc1;
	bytearray payload = NULL;
	uint32_t crc2 = 0;
};

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t* newPkt = malloc(sizeof(pkt_t));
	return newPkt;
}

void pkt_del(pkt_t *pkt)
{
    if(!pkt)
		return;
	
	//to-do free attributs
	if(pkt->payload)
		free(pkt->payload);
	
	free(pkt);	
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    /* Your code will be inserted here */
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
	return pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return pkt->crc1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    if(type<1 || type>3)    return E_TYPE;
    pkt->type = type;   return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    if(tr!=0 && tr!=1)    return E_TR;
    pkt->tr = tr;   return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    if(window>31)    return E_WINDOW;
    pkt->window = window;   return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    pkt->seqnum = seqnum;   return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    if(length>512)  return E_LENGTH;
    pkt->length = length;   return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->timestamp = timestamp; return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->crc1 = crc1;   return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    pkt->crc2 = crc2;   return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    pkt_status_code lenOk = pkt_set_length(length);
    if(lenOk!=PKT_OK)
        return lenOk;
    pkt->payload = malloc(pkt_get_length());
    if(!pkt->payload)
        return E_NOMEM;
    memcpy(pkt->payload, data, pkt_get_length());
    
    return PKT_OK;
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    /* Your code will be inserted here */
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
{
    /* Your code will be inserted here */
}

size_t varuint_len(const uint8_t *data)
{
    /* Your code will be inserted here */
}


ssize_t varuint_predict_len(uint16_t val)
{
    /* Your code will be inserted here */
}


ssize_t predict_header_length(const pkt_t *pkt)
{
    /* Your code will be inserted here */
}
