#ifndef CONS_MANAGE_H_INCLUDED
#define CONS_MANAGE_H_INCLUDED

#include "connection.h"

co_t** malloc_connections(const int nbConnections);
void free_connections();
int get_index_client(const SOCKADDR_IN6 *addr,const socklen_t addrSize);
int add_connection(const SOCKADDR_IN6 *addr,const socklen_t addrSize);
int handle_reception();
void check_times_out();

#endif // CONS_MANAGE_H_INCLUDED
