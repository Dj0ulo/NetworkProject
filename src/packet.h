#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */
#include <stdio.h>  /* ssize_t */
#include <sys/types.h>

#include "utils.h"

/* Types de paquets */
typedef enum {
    PTYPE_DATA = 1,
    PTYPE_ACK = 2,
    PTYPE_NACK = 3,
} ptypes_t;

/* Taille maximale permise pour le payload */
#define MAX_PAYLOAD_SIZE 512
#define MAX_PACKET_SIZE 8+4+MAX_PAYLOAD_SIZE+4
/* Taille maximale de Window */
#define MAX_WINDOW_SIZE 31

/* Valeur de retours des fonctions */
typedef enum {
    PKT_OK = 0,     /* Le paquet a ete traite avec succes */
    E_TYPE,         /* Erreur liee au champs Type */
    E_TR,           /* Erreur liee au champ TR */
    E_LENGTH,       /* Erreur liee au champs Length  */
    E_CRC,          /* CRC invalide */
    E_WINDOW,       /* Erreur liee au champs Window */
    E_SEQNUM,       /* Numero de sequence invalide */
    E_NOMEM,        /* Pas assez de memoire */
    E_NOHEADER,     /* Le paquet n'a pas de header (trop court) */
    E_UNCONSISTENT, /* Le paquet est incoherent */
} pkt_status_code;

typedef struct pkt pkt_t;
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

/* Alloue et initialise une struct pkt
 * @return: NULL en cas d'erreur */
pkt_t* pkt_new();
/* Libere le pointeur vers la struct pkt, ainsi que toutes les
 * ressources associees
 */
void pkt_del(pkt_t*);

void pkt_print(const pkt_t* pkt);
int pkt_copy(pkt_t* dst, const pkt_t* src);

/*
 * Decode des donnees recues et cree une nouvelle structure pkt.
 * Le paquet recu est en network byte-order.
 * La fonction verifie que:
 * - Le CRC32 du header recu est le mÃªme que celui decode a la fin
 *   du header (en considerant le champ TR a 0)
 * - S'il est present, le CRC32 du payload recu est le meme que celui
 *   decode a la fin du payload
 * - Le type du paquet est valide
 * - La longueur du paquet et le champ TR sont valides et coherents
 *   avec le nombre d'octets recus.
 *
 * @data: L'ensemble d'octets constituant le paquet recu
 * @len: Le nombre de bytes recus
 * @pkt: Une struct pkt valide
 * @post: pkt est la representation du paquet recu
 *
 * @return: Un code indiquant si l'operation a reussi ou representant
 *         l'erreur rencontree.
 */
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt);

/*
 * Encode une struct pkt dans un buffer, prÃªt a Ãªtre envoye sur le reseau
 * (c-a-d en network byte-order), incluant le CRC32 du header et
 * eventuellement le CRC32 du payload si celui-ci est non nul.
 *
 * @pkt: La structure a encoder
 * @buf: Le buffer dans lequel la structure sera encodee
 * @len: La taille disponible dans le buffer
 * @len-POST: Le nombre de d'octets ecrit dans le buffer
 * @return: Un code indiquant si l'operation a reussi ou E_NOMEM si
 *         le buffer est trop petit.
 */
pkt_status_code pkt_encode(const pkt_t*, char *buf, size_t *len);

/* Accesseurs pour les champs toujours presents du paquet.
 * Les valeurs renvoyees sont toutes dans l'endianness native
 * de la machine!
 */
ptypes_t pkt_get_type     (const pkt_t*);
uint8_t  pkt_get_tr       (const pkt_t*);
uint8_t  pkt_get_window   (const pkt_t*);
uint8_t  pkt_get_seqnum   (const pkt_t*);
uint16_t pkt_get_length   (const pkt_t*);
uint32_t pkt_get_timestamp(const pkt_t*);
uint32_t pkt_get_crc1     (const pkt_t*);
/* Renvoie un pointeur vers le payload du paquet, ou NULL s'il n'y
 * en a pas.
 */
const char* pkt_get_payload(const pkt_t*);
/* Renvoie le CRC2 dans l'endianness native de la machine. Si
 * ce field n'est pas present, retourne 0.
 */
uint32_t pkt_get_crc2(const pkt_t*);

/* Setters pour les champs obligatoires du paquet. Si les valeurs
 * fournies ne sont pas dans les limites acceptables, les fonctions
 * doivent renvoyer un code d'erreur adapte.
 * Les valeurs fournies sont dans l'endianness native de la machine!
 */
pkt_status_code pkt_set_type     (pkt_t*, const ptypes_t type);
pkt_status_code pkt_set_tr       (pkt_t*, const uint8_t tr);
pkt_status_code pkt_set_window   (pkt_t*, const uint8_t window);
pkt_status_code pkt_set_seqnum   (pkt_t*, const uint8_t seqnum);
pkt_status_code pkt_set_length   (pkt_t*, const uint16_t length);
pkt_status_code pkt_set_timestamp(pkt_t*, const uint32_t timestamp);
pkt_status_code pkt_set_crc1     (pkt_t*, const uint32_t crc1);
/* Defini la valeur du champs payload du paquet.
 * @data: Une succession d'octets representants le payload
 * @length: Le nombre d'octets composant le payload
 * @POST: pkt_get_length(pkt) == length */
pkt_status_code pkt_set_payload(pkt_t*,
                                const char *data,
                                const uint16_t length);
/* Setter pour CRC2. Les valeurs fournies sont dans l'endianness
 * native de la machine!
 */
pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2);


/*
 * Decode un varuint (entier non signe de taille variable  dont le premier bit indique la longueur)
 * encode en network byte-order dans le buffer data disposant d'une taille maximale len.
 * @post: place à l'adresse retval la valeur en host byte-order de l'entier de taille variable stocke
 * dans data si aucune erreur ne s'est produite
 * @return:
 *
 *          -1 si data ne contient pas un varuint valide (la taille du varint
 * est trop grande par rapport à la place disponible dans data)
 *
 *          le nombre de bytes utilises si aucune erreur ne s'est produite
 */
ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval);


/*
 * Encode un varuint en network byte-order dans le buffer data disposant d'une
 * taille maximale len.
 * @pre: val < 0x8000 (val peut être encode en varuint)
 * @return:
 *           -1 si data ne contient pas une taille suffisante pour encoder le varuint
 *
 *           la taille necessaire pour encoder le varuint (1 ou 2 bytes) si aucune erreur ne s'est produite
 */
ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len);


/*
 * @pre: data pointe vers un buffer d'au moins 1 byte
 * @return: la taille en bytes du varuint stocke dans data, soit 1 ou 2 bytes.
 */
size_t varuint_len(const uint8_t *data);


/*
 * @return: la taille en bytes que prendra la valeur val
 * une fois encodee en varuint si val contient une valeur varuint valide (val < 0x8000).
            -1 si val ne contient pas une valeur varuint valide
 */
ssize_t varuint_predict_len(uint16_t val);


/*
 * Retourne la longueur du header en bytes si le champs pkt->length
 * a une valeur valide pour un champs de type varuint (i.e. pkt->length < 0x8000).
 * Retourne -1 sinon
 * @pre: pkt contient une adresse valide (!= NULL)
 */
ssize_t predict_header_length(const pkt_t *pkt);


#endif  /* PACKET_H_INCLUDED*/
