#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

#include "../src/connection.h"
#include "../src/cons_manage.h"
#include "../src/socket.h"
#include "../src/utils.h"
#include "../src/packet.h"



int init_suite1()
{

	return 0;
}
int clean_suite1()
{


	return 0;
}

void testPkt()
{
    const uint16_t x = 0x8000-1;

    pkt_t * pkt = pkt_new();
    pkt_set_payload(pkt,"hello world",11);
    pkt_set_window(pkt,28);
    pkt_set_seqnum(pkt,0x7b);
    pkt_set_timestamp(pkt, 0xdeadbeef);

    CU_ASSERT(memcmp(pkt_get_payload(pkt),"hello world",11)==0);
    CU_ASSERT(pkt_get_window(pkt) == 28);

    char buf[MAX_PACKET_SIZE];
    size_t len = MAX_PACKET_SIZE;
    pkt_encode(pkt, buf, &len);

    pkt_t *pRec = pkt_new();
    pkt_decode(buf, len,pRec);

    CU_ASSERT(memcmp(pkt_get_payload(pRec),"hello world",11)==0);
    CU_ASSERT(pkt_get_seqnum(pRec)==0x7b);
    CU_ASSERT(pkt_get_timestamp(pRec)==0xdeadbeef);

    uint8_t *data = malloc(2);
    varuint_encode(x,data,2);
    uint16_t ret;
    varuint_decode(data,2,&ret);

    CU_ASSERT(ret==x);

    free(data);
    pkt_del(pRec);
    pkt_del(pkt);
}

int main()
{
	CU_pSuite pSuite = NULL;
	if(CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
	if (NULL == pSuite) {
    	CU_cleanup_registry();
    	return CU_get_error();
   	}

   	/* add the tests to the suite */
   	/* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   	if (
		(NULL == CU_add_test(pSuite,"Teste l'encodage et le decodage de packets", testPkt))
		)
   	{
    	CU_cleanup_registry();
    	return CU_get_error();
   	}

    system("chmod +x tests/sender");

   	printf("Test : reception de 4 fichiers en meme temps\n");
   	system("./tests/sender ::1 13457 -f tests/input/file | ./tests/sender ::1 13457 -f tests/input/txt | ./tests/sender ::1 13457 -f tests/input/pi | ./tests/sender ::1 13457 -f \"tests/input/Jokem - Stone Piano.flac\" | ./receiver :: 13457 -t 6 -o \"file_recv%d.txt\"");

   	/* Run all tests using the CUnit Basic interface */
   	CU_basic_set_mode(CU_BRM_VERBOSE);
   	CU_basic_run_tests();

	CU_cleanup_registry();
	return CU_get_error();
}
