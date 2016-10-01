#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * get_data(char *texto){
	char * data;
	char * texto_completo;

	FILE *fp;
	fp = fopen(texto, "r");//abrindo arquivo somente para leitura
	
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

	printf("Recebi algo: %s \n", buffer);

	//É uma requisição GET?
	if(buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T'){
		i=5;//o nome do arquivo começa na posição 5 do vetor, acabando no próximo espaço em branco.
		while(buffer[i] != ' '){ //faço o parser para obter o arquivo desejado pelo cliente
			texto[i-5] = buffer[i];
			++i;
		}
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
}
int main(int argc, char *argv[]) {
	int socket_servidor;
	int socket_cliente;
	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
	unsigned short servidorPorta;
	unsigned int cliente_length;
	if (argc != 2) { //Caso não tenha entrado com os parâmetros certos
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
		
		trata_cliente(socket_cliente);
		close(socket_cliente);
	}
	close(socket_servidor);
}