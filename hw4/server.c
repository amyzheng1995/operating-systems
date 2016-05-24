#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr



//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("ERROR: Could not create socket");
    }
    // puts("Socket created");
     
   int port = 9753;
   
    char fp[] = ".storage";//.storage
    if (mkdir(fp, 0777) ==0){
        puts("no directory exists, it has been created");
    }
    else if (mkdir(fp, 0777) == -1){
        // puts("ERROR: THIS DIRECTORY ALREADY EXIST, DELETE AND RECREATE");
        char command[50];
        strcpy( command, "rm -rf .storage");
        system(command);
        mkdir(fp, 0777);
    }
   else{
      puts("error");
   }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    // puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
     
     
    //Accept and incoming connection
    // puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    printf("Listening on port %d\n", port);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        // puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        printf("Received incoming connection from %s\n", inet_ntoa((struct in_addr)client.sin_addr)) ;
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        // pthread_join( sniffer_thread , NULL);
        // puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}

/*
 * This will handle connection for each client
 * */
 
//HOW TO CONTINUOUS SEND COMMANDS FROM CLIENT SIDE
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char  client_message[2000];
    int keep = 2;
    char tempfn[20];
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        char *token;
        char *bytes;
        char *length;
        char *offset;
        char *fn;
        FILE *f;
        char *fn2;
        char *fn3;
        char response[100000];
        char response2[100000];
        int b;
        char ff[50];
        // When you receive the client message, ignore the content but print everything else
        if (keep != 0){
            char *pch = strstr(client_message, "\n");
            if (pch!= NULL) strncpy(pch, "\0", 1);
            printf("[child %d] Rcvd: %s\n", (int)(pthread_self()), client_message);
            token = strtok(client_message, " ");
        }
        
        // For store
        if (strcmp(token, "STORE") == 0 && keep != 0){
            keep = 1;
            // read in input arguments
            fn = strtok(NULL, " ");
            strcpy(tempfn, fn);
            bytes = strtok(NULL, " ");
            b = atoi(bytes);
            if (fn == NULL || bytes == NULL){
                printf("ERROR: WRONG INPUTS\n");
                continue;
            }
            // chceck if the file exists
            FILE *f1;
            sprintf(ff, ".storage/%s", fn);
            if( access( ff, F_OK ) != -1 ) {
                printf("[child %d] ERROR: FILE EXISTS\n",(int)(pthread_self()));
                sprintf(response, "ERROR: FILE EXISTS\n");
                write(sock, response, strlen(response));
                keep = 3;
                continue;
            } else { //create file if it doesn't exist
                f1 = fopen(ff, "ab");
                struct stat statbuf;
                if(stat(ff, &statbuf) == -1){
                  perror("stat\n");
                }
                /*Checks if current item is of the type file (type 8) and no command line arguments*/
                if(S_ISREG(statbuf.st_mode) == 0)
                    printf("NOT REGULAR\n");
                // else
                //     printf("REGULAR\n");
                
            }
            fclose(f1);
        }
        // For delete
        else if(strcmp(token, "DELETE") == 0){
            fn2 = strtok(NULL, " ");
            char *pch = strstr(fn2, "\n");
            if (pch!= NULL) strncpy(pch, "\0", 1);
            char ff[50];
            sprintf(ff, ".storage/%s", fn2 );
            FILE *f = fopen(ff, "r");
            if (f == NULL){
                sprintf(response, "ERROR: NO SUCH FILE\n");
                write(sock, response, strlen(response));
                printf("[child %d] Sent: ERROR: NO SUCH FILE\n", (int)(pthread_self()));
                continue;
            }
            else{
                //if delete was successful
                if (unlink(ff) == 0){
                    printf("[child %d] Deleted \'%s\' from server\n", (int)(pthread_self()),fn2);
                    sprintf(response, "ACK\n");
                    write(sock , response , strlen(response));
                    printf("[child %d] Sent: %s", (int)(pthread_self()), response);
                }
                else{ //print error if not
                    sprintf(response, "ERROR: DELETING not success\n");
                    write(sock, response, strlen(response));
                    printf("[child %d] Sent: %s", (int)(pthread_self()), response);
                    continue;
                }
            }
            fclose(f);
        }
        // For read
        else if(strcmp(token, "READ") == 0){
            fn3 = strtok(NULL, " ");
            offset = strtok(NULL, " ");
            int o;
            o = atoi(offset);
            length = strtok(NULL, " ");
            int l;
            l = atoi(length);
            char ff[50];
            sprintf(ff, ".storage/%s", fn3 );
            f = fopen(ff, "r");
            char buffer[1000];
            if (f == NULL){
                sprintf(response, "ERROR: NO SUCH FILE\n");
                write(sock, response, strlen(response));
                printf("[child %d] Sent: ERROR: NO SUCH FILE\n", (int)(pthread_self()));
                continue;
            }
            
            else{
                fseek(f, 0, SEEK_END);
                int len;
                len = ftell(f);
                // printf("length: %d, %d\n", len, l+o);
                //error checking for out of range
                if (len < l + o || len-1 < o){
                    sprintf(response, "ERROR: INVALID BYTE RANGE\n");
                    write(sock, response, strlen(response));
                    printf("[child %d] ERROR: INVALID BYTE RANGE\n", (int)(pthread_self()));
                    continue;
                }
                else{
                    //get the length of the content first then start from offset to read
                    sprintf(response, "ACK %d\n", l);
                    write(sock , response , strlen(response));
                    printf("[child %d] Sent: %s", (int)(pthread_self()), response);
                    fseek(f, o, 0);
                    fread(buffer, l, 1, f);
                    buffer[l] = '\0';
                    // printf("%s\n", buffer);
                    sprintf(response2, "%s\n",  buffer);
                    write(sock , response2 , strlen(response2));
                    printf("[child %d] Sent %d bytes of \'%s\' from offset %d\n", (int)(pthread_self()), (int)strlen(buffer), fn3, o);
                }
            }
            fclose(f);
            buffer[0] = '\0';
        }
        else{
            if(keep != 1 && keep != 0){
                sprintf(response, "ERROR: INVALID INPUT\n");
                write(sock, response, strlen(response));
                printf("[child %d] Sent: ERROR: INVALID INPUT\n", (int)(pthread_self()));
                continue;
            }
        }
        //get ready to read content
        if (keep == 1){
            keep = 0;
        }
        //right after STORE, it should go in here to read content
        else if (keep == 0){
            f = fopen(ff, "w+");
            if (f != NULL){
                // fputs(buf, f);
                fwrite(client_message , 1 , b , f );
                printf("[child %d] Stored file \'%s\' (%d bytes)\n", (int)(pthread_self()), tempfn, b);
                sprintf(response, "ACK\n");
                write(sock, response, strlen(response));
                printf("[child %d]: Sent %s", (int)(pthread_self()), response);
            }
            
            keep = 3;
            fclose(f);
        }
        
        
        
    }
  
     
    if(read_size == 0)
    {
        puts("Client closed its socket...terminating");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    // free(socket_desc);
     
    return 0;
}

// gcc server.c -pthread -o s.out