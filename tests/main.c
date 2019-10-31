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
    free(pRec);
    free(pkt);
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
		(NULL == CU_add_test(pSuite,GREEN"Encodage et le decodage de packets"WHITE, testPkt))
		)
   	{
    	CU_cleanup_registry();
    	return CU_get_error();
   	}

    system("chmod +x tests/sender");
    system("chmod +x tests/test");
    system("chmod +x tests/link_sim");

   	printf(GREEN"Test"WHITE" : reception de "RED"4"WHITE" fichiers en meme temps\n");
   	system("./tests/sender ::1 64500 -f tests/input/file | ./tests/sender ::1 64500 -f tests/input/txt "
           "| ./tests/sender ::1 64500 -f tests/input/pi | ./tests/sender ::1 64500 -f tests/input/li.txt "
           "| ./receiver :: 64500 -t 6 -o \"file_recv%d.txt\"");
    printf(GREEN"Test"WHITE" : reception d'un fichier avec un taux de "RED"5 pourcents"WHITE" d'"BLUE"erreurs"WHITE
           ", de "BLUE"packets tronques"WHITE" et de "BLUE"pertes"WHITE" + "RED"100 ms"WHITE" de latence avec "RED"100 ms"WHITE" de variation (CRTL + V quand c'est fini)\n");
    system("./tests/link_sim -p 64500 -P 64600 -e 5 -c 5 -l 5 -d 100 -j 100 | ./tests/sender ::1 64500 -f tests/input/txt | ./receiver :: 64600 -t 6 -o \"file_erreur5%d.txt\"");
   	/* Run all tests using the CUnit Basic interface */
   	CU_basic_set_mode(CU_BRM_VERBOSE);
   	CU_basic_run_tests();

	CU_cleanup_registry();
	return CU_get_error();
}
