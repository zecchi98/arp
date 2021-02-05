#include "addresstable.h"

// the IP address table
iptab_addr_t __ip_table[ __IP_TABLE_LEN ] =
{
    { "127.0.0.1", __NOD_AVAILABLE },
    { "0.0.0.0", __NOD_AVAILABLE },
    { "255.255.0.255", __NOD_AVAILABLE },
};

int __ip_table_available_nodes = __IP_TABLE_LEN;

// ------------------------
//  BASIC OPERATIONS
// ------------------------

// get the total lenght of the table
int iptab_len()
{
    return __IP_TABLE_LEN;
}

// get the IP address (as string) of a certain node
char* iptab_getaddr(node_id n)
{
    if(
        n >= __IP_TABLE_LEN ||
        n < 0 ||
        !__ip_table[n].ip_avail )
        return NULL;
    else
        return __ip_table[n].ip_addr;
}

// get the ID of a certain node given its IP address
// -1 is returned if such IP doesn't exist in the table (or it is not available)
int iptab_get_ID_of(char* ip)
{
    int res = -1;
    for( int i=0; i<__IP_TABLE_LEN; i++ )
        if( strcasecmp( ip, __ip_table[i].ip_addr ) == 0 )
        {
            res = i;
            break;
        }

    if( (res < 0) || !iptab_is_available( res ) )
        return -1;

	return res;
}

// get the ID of the next available node in the table (in order)
node_id iptab_get_next( node_id my )
{
    node_id n = my;
    do  n = (n+1) % __IP_TABLE_LEN;
    while ( __ip_table[n].ip_avail == __NOD_NOT_AVAILABLE );

    return n;
}

/* DEPRECATED
// get the ID of the previous node in the table (in order)
node_id iptab_get_prev( node_id my )
{
    node_id n = my;
    do
    {
        n--;
        if( n < 0 ) n = __IP_TABLE_LEN - 1;
    }
    while ( __ip_table[n].ip_avail == __NOD_NOT_AVAILABLE );

    return n;
}
*/

// ------------------------
//  OPS ON "AVAILABLE" FLAG
// ------------------------

// get how many addresses are available in the table
int iptab_len_av()
{
    return __ip_table_available_nodes;
}

// check if a node is available (0 if it is not)
int iptab_is_available(node_id n)
{
    return (
        (n < __IP_TABLE_LEN) &&
        (n >= 0) &&
        __ip_table[n].ip_avail );
}

// mark a node as unavailable
// 0 is returned if the operation is done
int iptab_set_unavailable(node_id n)
{
    if( n < 0 || n >= __IP_TABLE_LEN )
        return -1;

    __ip_table[n].ip_avail = __NOD_NOT_AVAILABLE;
    __ip_table_available_nodes--;

    return 0;
}

// set the node as available
// 0 is returned if the operation is done
int iptab_set_available(node_id n)
{
    if( n < 0 || n >= __IP_TABLE_LEN )
        return -1;

    __ip_table[n].ip_avail = __NOD_AVAILABLE;
    __ip_table_available_nodes++;

    return 0;
}

// reset all the flags "available" as "yes" (default state)
void iptab_reset_flags()
{
    for( int i=0; i<__IP_TABLE_LEN; i++ )
        __ip_table[i].ip_avail = __NOD_AVAILABLE;

    __ip_table_available_nodes = __IP_TABLE_LEN;
}
