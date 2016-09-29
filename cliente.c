#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORTA 20000    /* Porta para conectar */
#define MAXDATASIZE 100 /* máximo número de bytes que poderemos enviar
                           por vez */

int main(int argc, char *argv[])
{
	int Meusocket, numbytes;
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in seu_endereco;

	if (argc != 2) {
		fprintf(stderr,"Uso: cliente hostname\n");
		exit(1);
	}

	if ((he=gethostbyname(argv[1])) == NULL)   /* envia host info */
	{
		herror("gethostbyname");
		exit(1);
	}
	if ((Meusocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}

	seu_endereco.sin_family = AF_INET;
	seu_endereco.sin_port = htons(PORTA);
	seu_endereco.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(seu_endereco.sin_zero), 8);

	if (connect(Meusocket,(struct sockaddr *)&seu_endereco, sizeof(struct sockaddr)) ==-1) 
	{
		perror("connect");
		exit(1);
	}

	if ((numbytes=recv(Meusocket, buf, MAXDATASIZE, 0)) == -1) 
	{
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	printf("Recebido: %s",buf);
	close(Meusocket);
	return 0;
}