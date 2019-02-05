#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Berkeley sockets, so assumes POSIX compliant OS //
#include <netdb.h>                                 //
#include <arpa/inet.h>                             //
#include <sys/socket.h>                            //
#include <netinet/in.h>                            //
/////////////////////////////////////////////////////

//html file related definitions
FILE* html_file;
char* buffer;
u_int64_t number_of_bytes;

//response is http response message; contents of the html file will be added to response
char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n";

int main(int argc, char* argv[]) {
    //Socket related definitions
    int32_t server_fd, new_socket;
    int32_t address_length = sizeof(struct sockaddr_in);
    static const u_int16_t PORT = 8080;


    // Writes the contents of the html file to char* buffer
    if (argc == 1) {
        printf("\x1b[31mMissing name of the .html file\x1b[0m\n");
        exit(EXIT_FAILURE);
    } else {
        html_file = fopen(argv[1], "r");

        if (html_file == NULL) {
            printf("\x1b[31mFile does not exist\n\x1b[0m");
            exit(EXIT_FAILURE);
        }

        fseek(html_file, 0L, SEEK_END);
        number_of_bytes = ftell(html_file);
        fseek(html_file, 0L, SEEK_SET);
        buffer = (char*)calloc(number_of_bytes, sizeof(char));	
        fread(buffer, sizeof(char), number_of_bytes, html_file);
        fclose(html_file);

        printf("\nFile \x1b[32m%s\x1b[0m contains the following content:\n\n\x1b[34m%s\x1b[0m\n\n\n", argv[1], buffer);


    }
    
    // Adds the content of the html file and \r\n to response to form an http compliant response
    strcat(response, buffer);
    strcat(response, "\r\n");
    //free(buffer);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("\x1b[31mEncountered an error in socket()\x1b[0m");
        exit(EXIT_FAILURE);
    }


    // Standard setup code for POSIX sockets
    struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(PORT)};

    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr* )&address, sizeof(address)) < 0) {
        perror("\x1b[31mEncountered an error in bind()\x1b[0m");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("\x1b[31mEncountered an error in listen()\x1b[0m");
        exit(EXIT_FAILURE);
    }


    do { //Starts the loop
        printf("\x1b[33mListening for incoming requests ...\n\x1b[0m\n");
       
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_length))<0) {
            perror("\x1b[31mEncountered an error in accept()\x1b[0m");
            exit(EXIT_FAILURE);
        }

        char request_buffer[3000] = {0}; 
        read(new_socket, request_buffer, 3000); //writes information about the request into request_buffer
        printf("\x1b[36mReceived a request:\x1b[0m\n%s\n", request_buffer);
        
        write(new_socket, response, strlen(response)); //sends response to the client
        printf("\x1b[32mSent a response\x1b[0m\n\n");

        close(new_socket);

    } while (true);

    return 0;

}