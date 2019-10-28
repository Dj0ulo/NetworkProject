#ifndef CONS_MANAGE_H_INCLUDED
#define CONS_MANAGE_H_INCLUDED

#include "connection.h"

/*
 *  Initialise les nbConnections slots de Connection possibles
 *  @return un tableau de Connection*
 */
co_t** malloc_connections(const int nbConnections);

/*
 *  Libere les Connections allouées
 */
void free_connections();

/*
 *  Cherche une Connection via son adresse dans le tableau de Connection
 *  @return l'undice de la Connection dans le tableau
 */
int get_index_client(const SOCKADDR_IN6 *addr,const socklen_t addrSize);

/*
 *  Ajoute une connection au tableau et l'initialise
 *  @return 0 si tout est ok, -1 sinon
 */
int add_connection(const SOCKADDR_IN6 *addr,const socklen_t addrSize);

/*
 *  Gère la reception d'un packet et les nouvelles Connections entrantes (par extension)
 *  @return 0 si tout est ok, -1 sinon
 */
int handle_reception();

/*
 *  Check si chaque Connection active a passé son timeout
 */
void check_times_out();

#endif // CONS_MANAGE_H_INCLUDED
