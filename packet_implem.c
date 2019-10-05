#include "packet_interface.h"

/* Extra #includes */
//#ifdef WIN32
#include <winsock.h>
//#endif // WIN32
//#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>
#include <zlib.h>

typedef uint8_t bool;

#define bytearray char*

struct __attribute__((__packed__)) pkt {
	ptypes_t type;
	bool tr;
	uint8_t window;
	uint16_t length;
	uint8_t seqnum;
	uint32_t timestamp;
	uint32_t crc1;
	bytearray payload;
	uint32_t crc2;
};

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t* newPkt = malloc(sizeof(pkt_t));
    if(!newPkt)
        return NULL;
    newPkt->type = 0;
    newPkt->tr = 0;
    newPkt->window = 0;
    newPkt->length = 0;
    newPkt->seqnum = 0;
    newPkt->timestamp = 0;
    newPkt->crc1 = 0;
    newPkt->payload = NULL;
    newPkt->crc2 = 0;
	return newPkt;
}

void pkt_del(pkt_t *pkt)
{
    if(!pkt)
		return;

	if(pkt->payload)
		free(pkt->payload);

	free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    if(len < 7+4)
        return E_NOHEADER;
    pkt_status_code psc;

    size_t off = 0;
    psc = pkt_set_type(pkt, data[off] & 0b11000000);
    if(psc) return psc;

    psc = pkt_set_tr(pkt, data[off] & 0b00100000);
    if(psc) return psc;

    if(pkt_get_tr(pkt) != 0 && pkt_get_type(pkt) != PTYPE_DATA)
        return E_UNCONSISTENT;

    psc = pkt_set_window(pkt, data[off] & 0b00011111);
    if(psc) return psc;

    uint16_t lenPL;
    ssize_t sizeLenField = varuint_decode((uint8_t*)&data[off += 1], 2, &lenPL);
    if(sizeLenField == 2 && len < 8+4)
        return E_NOHEADER;
    psc = pkt_set_length(pkt, lenPL);
    if(psc) return psc;

    psc = pkt_set_seqnum(pkt, data[off += sizeLenField]);
    if(psc) return psc;

    psc = pkt_set_crc1(pkt, (uint32_t*)&data[off += 1]);
    if(psc) return psc;

    if(lenPL == 0)
        return PKT_OK;

    if(len < off + 4 + lenPL + 4)
        return E_UNCONSISTENT;

    psc = pkt_set_payload(pkt, &data[off += 4], lenPL);
    if(psc) return psc;

    psc = pkt_set_crc2(pkt, (uint32_t*)&data[off += lenPL]);
    if(psc) return psc;

    return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    const uint16_t lenPL = pkt_get_length(pkt);
    ssize_t predSize = predict_header_length(pkt);
    if(predSize == -1)
        return E_LENGTH;
    predSize += 4;//size crc1
    if(lenPL>0)
        predSize += lenPL + 4;//size crc2

    if((size_t)predSize>*len)
        return E_NOMEM;
    *len = predSize;
    size_t off = 0;

    if(pkt_get_payload(pkt)!=NULL && pkt_get_length(pkt)!=0)
        pkt_set_type(pkt, PTYPE_DATA);
    else
        pkt_set_type(pkt, PTYPE_ACK);

    buf[off] = pkt_get_type(pkt) << 6;
    buf[off] |= pkt_get_tr(pkt) << 5;
    buf[off] |= pkt_get_window(pkt);
    uint8_t tab[2];
    ssize_t sizeLenField = varuint_encode(lenPL, tab, 2);
    memcpy(&buf[off += 1], tab, 2);
    if(sizeLenField == -1)
        return E_LENGTH;
    buf[off += sizeLenField] = pkt_get_seqnum(pkt);
    memcpy(&buf[off += 1], &pkt->timestamp, 4);
    memcpy(&buf[off += 4], &pkt->crc1, 4);
    memcpy(&buf[off += 4], pkt->payload, lenPL);
    if(lenPL>0)
        memcpy(&buf[off += lenPL], &pkt->crc2, 4);
    return PKT_OK;
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
    pkt_status_code lenOk = pkt_set_length(pkt, length);
    if(lenOk!=PKT_OK)
        return lenOk;
    pkt->payload = malloc(pkt_get_length(pkt));
    if(!pkt->payload)
        return E_NOMEM;
    memcpy(pkt->payload, data, pkt_get_length(pkt));

    return PKT_OK;
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    size_t predLen = varuint_len(data);
    if(len < predLen)
        return -1;

    if(predLen == 1)
        *retval = data[0];
    else{
        uint8_t tab[2] = {data[0] ^ (1u<<7), data[1]};
        uint16_t ns = *((uint16_t *)tab);
        *retval = ntohs(ns);
    }

    return predLen;
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
{
    ssize_t predLen = varuint_predict_len(val);

    if(predLen > 0)
    {
        uint16_t ns = htons(val);
        uint8_t *tab = (uint8_t*)&ns;
        if(predLen == 1 && len >= 1){
            data[0] = tab[1];
            return 1;
        }
        else if(predLen == 2 && len >= 2){
            data[0] = tab[0];
            data[1] = tab[1];
            data[0] = data[0] | (1u<<7);
            return 2;
        }
    }
    return -1;
}

size_t varuint_len(const uint8_t *data)
{
    return 1 + !!(data[0] & (1u<<7));
}


ssize_t varuint_predict_len(uint16_t val)
{
    if(val < 0x80)
        return 1;
    else if(val < 0x8000)
        return 2;
    return -1;
}


ssize_t predict_header_length(const pkt_t *pkt)
{
    ssize_t predLen = varuint_predict_len(pkt_get_length(pkt));
    if(predLen == 1)
        return 7;
    else if(predLen == 2)
        return 8;
    return -1;
}
