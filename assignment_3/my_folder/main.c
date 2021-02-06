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
	int next_sockfd,sockfd,previous_sockfd,turn_leader_sockfd,winner_sockfd, portno, n,turn_leader,cont,Available;
	votation_t votation_message;
	handshake_t hshmsg; //attento potrebbe essere scritta male
	node_id my_id=0;
	node_id next_id;
	struct sockaddr_in* p_cli_addr;
	struct timeval tv;


	bzero(MY_IP_STR,64);
	strcpy(MY_IP_STR,"52.233.227.134");
	bzero(next_address,64);
	portno=ARPNET_STD_PORTNO;
	printf("\n\n");

	if(hsh_imfirst(MY_IP_STR)==1){
		printf("sono il nodo zero\n");
		hsh_init(&hshmsg);	
		printf("\nAvailable: se 1 ok?%d\n",hsh_check_availability(my_id,&hshmsg));

		next_id=iptab_get_next(my_id);
		strcpy(next_address,iptab_getaddr(next_id));	

		//attiva il server -- gestione errori?
		sockfd=net_server_init();

		//contatta un server
		next_id=my_id;
		do{

			next_id=iptab_get_next(next_id);
			bzero(next_address,64);
			strcpy(next_address,iptab_getaddr(next_id));
			printf("\nNext Address:%s\n",next_address);	
			next_sockfd=net_client_connection(next_address);
		}while(next_sockfd);
		write(next_sockfd,&hshmsg,sizeof(hshmsg));
		printf("messaggio inviato\n");

		//aspetta un client
		previous_sockfd=net_accept_client(sockfd,p_cli_addr);
		printf("cliente accettato\n");

		//riceve messaggio
		read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		hsh_update_iptab(&hshmsg);
		printf("messaggio ricevuto\n");


		//ripetiamo l'anello
		close(next_sockfd);
		next_id=my_id;
		do{

			next_id=iptab_get_next(next_id);
			bzero(next_address,64);
			strcpy(next_address,iptab_getaddr(next_id));
			printf("\nNext Address:%s\n",next_address);	
			next_sockfd=net_client_connection(next_address);
		}while(next_sockfd);
		write(next_sockfd,&hshmsg,sizeof(hshmsg));
		printf("Inizio secondo anello:\nMessaggio inviato\n");


		read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		hsh_update_iptab(&hshmsg);
		printf("messaggio ricevuto\n");
		//fine aggiornamento tabella



		//init step
		vote_init(&votation_message);
		vote_do_votation(&votation_message);

		write(next_sockfd,&votation_message,sizeof(votation_message));
		printf("votazione iniziata\n");

		read(previous_sockfd,&votation_message,sizeof(votation_message));
		printf("votazione terminata\n");

		turn_leader=vote_getWinner(&votation_message);
		printf("the winner is: %d\n",turn_leader);

		if(my_id!=turn_leader)
		{
			message_t message_turn_leader;
			msg_init(&message_turn_leader);
			msg_set_ids(&message_turn_leader,my_id,turn_leader);

			turn_leader_sockfd=net_client_connection(iptab_getaddr(turn_leader));
			write(turn_leader_sockfd,&message_turn_leader,sizeof(message_turn_leader));
			close(turn_leader_sockfd);
			printf("messaggio inviato al turn leader\n");
		}
	}
	else
	{
		printf("Non sono il nodo zero");
		


		//attiva il server
		sockfd=net_server_init();


		//aspetta un client
		previous_sockfd=net_accept_client(sockfd,p_cli_addr);
		printf("cliente accettato\n");

		//riceve messaggio
		read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		printf("messaggio ricevuto\n");
		
		Available=hsh_check_availability(my_id,&hshmsg);
		printf("\nAvailable: se 1 ok?%d\n",Available);
		
		if(Available==1)
		{

			//contatta un server
			next_id=my_id;
			do{

				next_id=iptab_get_next(next_id);
				bzero(next_address,64);
				strcpy(next_address,iptab_getaddr(next_id));
				printf("\nNext Address:%s\n",next_address);	
				next_sockfd=net_client_connection(next_address);
				
			}while(next_sockfd);
			write(next_sockfd,&hshmsg,sizeof(hshmsg));
			printf("messaggio inviato\n");
			
			
			
		}
		
		//inizio secondo anello
		read(previous_sockfd,&hshmsg,sizeof(hshmsg));
		printf("messaggio ricevuto\n");
		
		hsh_update_iptab(&hshmsg);
		
		//contatta un server
		close(next_sockfd);
		next_id=my_id;
		do{

			next_id=iptab_get_next(next_id);
			bzero(next_address,64);
			strcpy(next_address,iptab_getaddr(next_id));
			printf("\nNext Address:%s\n",next_address);	
			next_sockfd=net_client_connection(next_address);

		}while(next_sockfd);
		write(next_sockfd,&hshmsg,sizeof(hshmsg));
		printf("messaggio inviato\n");
		//qui termina il secondo anello
		
		//inizia la votazione
		read(previous_sockfd,&votation_message,sizeof(votation_message));
		vote_do_votation(&votation_message);
		write(next_sockfd,&votation_message,sizeof(votation_message));
		printf("votazione compiuta\n");


		//aspetta il messaggio del leader
		message_t winner_message;
		winner_sockfd=net_accept_client(sockfd,p_cli_addr);
		read(winner_sockfd,&winner_message,sizeof(winner_message));
		
		printf("Il turn leader e' :%d\n ",msg_get_turnLeader(&winner_message));	
		
		
	}
	printf("\n\n");
	return 0;
}
