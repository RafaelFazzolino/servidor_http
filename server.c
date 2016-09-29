#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MINHAPORTA 20000    /* Porta que os usuarios irão se conectar*/
#define BACKLOG 10     /* Quantas conexões pendentes serão indexadas */

main()
{
	int Meusocket, Novosocket;  /* escuta em Meusocket, nova conexão
	                               em Novosocket */
	struct sockaddr_in meu_endereco;    /* informação do meu endereco */
	struct sockaddr_in endereco_dele; /* informação do endereco do conector */
	int tamanho;

	if ((Meusocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}

	meu_endereco.sin_family = AF_INET;
	meu_endereco.sin_port = htons(MINHAPORTA);
	meu_endereco.sin_addr.s_addr = INADDR_ANY; /* coloca IP automaticamente */
	bzero(&(meu_endereco.sin_zero), 8);        /* Zera o resto da estrutura */

	if (bind(Meusocket, (struct sockaddr *)&meu_endereco, sizeof(struct sockaddr))== -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(Meusocket, BACKLOG) < 0) {
		perror("listen");
		exit(1);
	}

	while(1) {
		tamanho = sizeof(struct sockaddr_in);
		if ((Novosocket = accept(Meusocket, (struct sockaddr *)&endereco_dele,&tamanho)) < 0){
			perror("accept");
			continue;
		}
		printf("Servidor: chegando conexão de %s\n",inet_ntoa(endereco_dele.sin_addr));
		if (!fork()) {
			if (send(Novosocket, "Seja bem vindo!\n", 16, 0) == -1)
			{
				perror("send");
				close(Novosocket);
				exit(0);
			}
		}
		close(Novosocket);
		while(waitpid(-1,NULL,WNOHANG) > 0); /* Limpa o processo crianca.fork() */
	}
}