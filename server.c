#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

//gcc -o server server.c -dbg -lcrypt

char * hash_data(char *texto){//Criando hash da mensagem:
	printf("encriptando: %s\n", texto);
	texto = crypt(texto, "ab");
	printf("Hash: %s\n", texto);
    return texto;
}
char * get_data(char *texto){
	char * data;
	char * texto_completo;
	FILE *fp;

	fp = fopen(texto, "r");
	if(fp == NULL){//abrindo arquivo somente para leitura
		printf("404 Not Found");
		return "404 Not Found\n";
	}

	data = malloc(sizeof(char*));
	texto_completo = malloc(sizeof(char*));

	while(fgets(data, 100, (FILE*)fp) != NULL){//Enquanto não chegar ao fim do arquivo
		strcat(texto_completo, data);//concatenando cada linha do arquivo para gerar o texto completo
	}

	fclose(fp);
	return texto_completo;
}

void trata_cliente(int socket_cliente) {
	char buffer[200];
	char texto[200];
	int tamanho_recebido, tamanho_envio, i;
	//pega a info, retornando o tamanho dessa info
	if((tamanho_recebido = recv(socket_cliente, buffer, 200, 0)) < 0)
		printf("Erro no recv()\n");
	buffer[tamanho_recebido] = '\0';//adicionando final da string

	printf("recebido:%s\n\n", buffer);
	//É uma requisição GET?
	if(buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T'){
		printf("olha: %s\n", buffer);
		//o nome do arquivo começa na posição 4 do vetor, acabando no próximo espaço em branco.
		for(i=4 ; i<=strlen(buffer)+4 ; i++){ //faço o parser para obter o arquivo desejado pelo cliente
			texto[i-4] = buffer[i];
		}
		strcpy(texto, get_data(texto));//Pegando os dados do arquivo desejado
		while (tamanho_recebido > 0) {
			tamanho_envio = strlen(texto);//pegando o tamanho do texto
			//verifica se todos os bytes foram enviados
			if(send(socket_cliente, texto, tamanho_envio, 0) != tamanho_envio)
				printf("Erro no envio - send()\n");
			
			if((tamanho_recebido = recv(socket_cliente, buffer, 200, 0)) < 0)
				printf("Erro no recv()\n");
		}
	}else 
		if(buffer[0] == 'H' && buffer[1] == 'A' && buffer[2] == 'S' && buffer[3] == 'H' && buffer[4] == 'M' && buffer[5] == 'E' ){
			//o nome do arquivo começa na posição 8 do vetor, acabando no próximo espaço em branco.
			for(i=7 ; i<=strlen(buffer)+7 ; i++){ //faço o parser para obter o arquivo desejado pelo cliente
				texto[i-7] = buffer[i];
			}
			strcpy(texto, hash_data(texto));
			while (tamanho_recebido > 0) {
				tamanho_envio = strlen(texto);//pegando o tamanho do texto
				//verifica se todos os bytes foram enviados
				if(send(socket_cliente, texto, tamanho_envio, 0) != tamanho_envio)
					printf("Erro no envio - send()\n");
				
				if((tamanho_recebido = recv(socket_cliente, buffer, 200, 0)) < 0)
					printf("Erro no recv()\n");
			}
		}else{
			printf("Requisição desconhecida!\n");
			return;
		}
}
int main(int argc, char *argv[]) {
	pthread_t *thread;
	int socket_servidor;
	int socket_cliente;
	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
	unsigned short servidorPorta;
	unsigned int cliente_length;

	//thread = malloc(sizeof(pthread_t));
	if (argc != 2) { //Caso não tenha entrado com os parâmetros certos
		printf("Uso: %s <Porta>\n", argv[0]);
		exit(1);
	}
	servidorPorta = atoi(argv[1]);
	// Abrir Socket
	if((socket_servidor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) //Gerando um socket tcp
		printf("falha no socket do Servidor\n");
	
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
		
		trata_cliente(socket_cliente);
		close(socket_cliente);
	}
	close(socket_servidor);
}