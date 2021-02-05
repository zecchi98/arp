// Federico Zecchi node zero for 

#include "arpnet.h"
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main()
{

	char MY_IP_STR[64];
	char host[64],next_address[64];
	int next_sockfd,previous_sockfd, portno, n;
	handshake_t hshmsg; //attento potrebbe essere scritta male
	node_id my_id=0;
	node_id next_id;
	struct sockaddr_in* p_cli_addr;
	struct timeval tv;
	int nfds,fd_ready;
	fd_set rfds;
	
	
	bzero(MY_IP_STR,64);
	strcpy(MY_IP_STR,"52.233.227.134");
	printf("\n\n");
	/*Steps for node ZER0:
	1.	After checking that we are the node zero (*hsh_imfirst(MY_IP_STR)*)…
	2.	Prepare the message for the handshake (*hsh_init(&hshmsg)*)
	3.	We should not forget to mark ourselves as available! (*hsh_check_availability(…)*)
	4.	Try and find the next available node (see *connection.h* header); we have to use sockets for checking if one of the 
		nodes next to us is available (see addresstable.h, in particular the method *iptab_get_next()*, paying attention to 
		the fact that the field *"my"* shouldn’t necessarily be our node ID*); we should avoid directly using the flag “available” of 
		the message in this step! All the functions are already provided to us for updating our (local) address table.
	5.	When we find the next available node, we have to pass the message to it
	6.	We should wait for the message to come back here after going all along the "ring" of nodes
	7.	When this message is returned to us, we have to update our address table (see *hsh_update_iptab(…)*)
	8.	Then the message should once again be sent to the next *available* node, so that all other nodes can update their tables as well.
	9.	The second time the message comes back we can kick on the next phase, the first *turn*.
	>> 	(*) eg. check your next node, if the connection works keep going on, else try with the following node, calling 
		iptab_get_next(my_ID+1, msg) and so on*/
	
	printf("First: se 1 ok?%d\n",hsh_imfirtst(MY_IP_STR));
	
	hsh_init(&hshmsg);	
	printf("\nAvailable: se 1 ok?%d\n",hsh_check_availability(my_id,&hshmsg));
	
	
	
	portno=ARPNET_STD_PORTNO;
	
	bzero(host,64);
	bzero(next_address,64);
	next_id=iptab_get_next(my_id);
	strcpy(next_address,iptab_getaddr(next_id));
	printf("\nNext id:%d\n",next_id);
	printf("\nNext Address:%s\n",next_address);	
	
	
	 
	 //attiva il server -- gestione errori?
	 previous_sockfd=net_server_init();
	 
	 
	 //contatta un server
	 next_sockfd=net_client_connection(next_address);
	 write(next_sockfd,&hshmsg,sizeof(hshmsg)); //SI SCRIVE COSI IL MESSAGGIO?
	  
	 //aspetta un client
	 previous_sockfd=net_accept_client(previous_sockfd,p_cli_addr);
	 
	 //riceve messaggio
	 nfds=previous_sockfd+1;
	 FD_ZERO(&rfds);
	 FD_SET(previous_sockfd,&rfds);
	 fd_ready=select(nfds,&rfds,NULL,NULL,NULL);
	 if(FD_ISSET(previous_sockfd,&rfds))
	 {
		 read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		 hsh_update_iptab(&hshmsg);
	 }
	 
	 //ripetiamo l'anello
	 write(next_sockfd,&hshmsg,sizeof(hshmsg)); //SI SCRIVE COSI IL MESSAGGIO?
	 

	 //riceve messaggio
	 nfds=previous_sockfd+1;
	 FD_ZERO(&rfds);
	 FD_SET(previous_sockfd,&rfds);
	 fd_ready=select(nfds,&rfds,NULL,NULL,NULL);
	 if(FD_ISSET(previous_sockfd,&rfds))
	 {
		 read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		 hsh_update_iptab(&hshmsg);
	 }
	 
	 


	

	printf("\n\n");
	return 0;
}
