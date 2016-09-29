#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void TrataClienteTCP(int socket_cliente) {
	char buffer[200];
	char texto[200];
	int tamanho_recebido;
	if((tamanho_recebido = recv(socket_cliente, buffer, 200, 0)) < 0)
		printf("Erro no recv()\n");
	buffer[tamanho_recebido] = '\0';
	printf("Servidor!\n Recebi algo: %s \n", buffer);

	while (tamanho_recebido > 0) {
		strcpy(texto, "este texto foi modificado pelo servidor!\n\n\0");
		strcat(texto, buffer);
		if(send(socket_cliente, texto, tamanho_recebido, 0) != tamanho_recebido)
			printf("Erro no envio - send()\n");
		
		if((tamanho_recebido = recv(socket_cliente, buffer, 200, 0)) < 0)
			printf("Erro no recv()\n");
	}
}
int main(int argc, char *argv[]) {
	int socket_servidor;
	int socket_cliente;
	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
	unsigned short servidorPorta;
	unsigned int cliente_length;
	if (argc != 2) { //Caso não tenha entrado com os parâmetros
		printf("Uso: %s <Porta>\n", argv[0]);
		exit(1);
	}
	servidorPorta = atoi(argv[1]);
	// Abrir Socket
	if((socket_servidor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) //Gerando um socket tcp
		printf("falha no socker do Servidor\n");
	
	// Montando a estrutura sockaddr_in
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servidorAddr.sin_port = htons(servidorPorta); //definindo a porta

	// Bind
	if(bind(socket_servidor, (struct sockaddr *) &servidorAddr, sizeof(servidorAddr)) < 0)
		printf("Falha no Bind\n");
	// Listen
	if(listen(socket_servidor, 10) < 0) //definindo uma fila máxima de 10 clientes aguardando
		printf("Falha no Listen\n");		
	while(1) {
		cliente_length = sizeof(clienteAddr);
		if((socket_cliente = accept(socket_servidor, 
			                      (struct sockaddr *) &clienteAddr, 
			                      &cliente_length)) < 0)
			printf("Falha no Accept\n");
		
		printf("Conexão do Cliente %s\n", inet_ntoa(clienteAddr.sin_addr));
		
		TrataClienteTCP(socket_cliente);
		close(socket_cliente);
	}
	close(socket_servidor);
}