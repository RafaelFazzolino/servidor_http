#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void get_text(char *mensagem, int clienteSocket){
	int tamanhoMensagem;
	char requisicao[200];

	strcpy(requisicao, "GET /");
	strcat(requisicao, mensagem);
	strcat(requisicao, " HTTP/1.1\r\n\r\n"); //sempre terminar uma requisição com \r\n\r\n
	printf("requisicao: %s\n", requisicao);
	tamanhoMensagem = strlen(requisicao);
	if(send(clienteSocket, requisicao, tamanhoMensagem, 0) != tamanhoMensagem)
		printf("Erro no envio: numero de bytes enviados diferente do esperado\n");
}

int main(int argc, char *argv[]) {
	int clienteSocket;
	struct sockaddr_in servidorAddr;
	unsigned short servidorPorta;
	char *IP_Servidor;
	char *mensagem;
	char buffer[200];
	unsigned int tamanhoMensagem;
	int bytesRecebidos;
	int totalBytesRecebidos;
	if ((argc < 3) || (argc > 4)) {
		printf("Uso: %s <IP do Servidor> <Porta> <Mensagem>\n", argv[0]);
		exit(1);
	}
	IP_Servidor = argv[1];
	servidorPorta = atoi(argv[2]);
	mensagem = argv[3];
	// Criar Socket
	if((clienteSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		printf("Erro no socket()\n");
	// Construir struct sockaddr_in
	memset(&servidorAddr, 0, sizeof(servidorAddr)); // Zerando a estrutura de dados
	servidorAddr.sin_family = AF_INET;
	servidorAddr.sin_addr.s_addr = inet_addr(IP_Servidor);
	servidorAddr.sin_port = htons(servidorPorta);
	// Connect
	if(connect(clienteSocket, (struct sockaddr *) &servidorAddr, 
							sizeof(servidorAddr)) < 0)
		printf("Erro no connect()\n");

	get_text(mensagem, clienteSocket);

	if((bytesRecebidos = recv(clienteSocket, buffer, 199, 0)) <= 0)
		printf("Alguns bytes foram perdidos\n");
	buffer[bytesRecebidos] = '\0';
	printf("%s\n", buffer);

	close(clienteSocket);
	exit(0);
}