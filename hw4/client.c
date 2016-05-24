/* tcp-client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
  /* create TCP client socket (endpoint) */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );
  
  if ( sock < 0 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }
  
  struct hostent * hp = gethostbyname( "127.0.0.1" );
  if ( hp == NULL )
  {
    perror( "gethostbyname() failed" );
    exit( EXIT_FAILURE );
  }
  char buff[2000];
  char buff2[2000];
  int i;
  for (i=0;i<2000;i++){
    buff[i] = ' ';
    buff2[i] = ' ';
  }
  struct sockaddr_in server;
  server.sin_family = PF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr,
      hp->h_length );
  unsigned short port = 9753;
  server.sin_port = htons( port );
  
  printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );
  
  if ( connect( sock, (struct sockaddr *)&server,
            sizeof( server ) ) < 0 )
  {
    perror( "connect() failed" );
    exit( EXIT_FAILURE );
  }
  while (1){
    printf( "connection success, enter your command: " );
  
    while (fgets(buff, 2000 , stdin)!=NULL)
    {
      
      if (buff[0]=='S'&& buff[1]=='T'&& buff[2]=='O'&& buff[3]=='R'&& buff[4]=='E'){
          fgets(buff+strlen(buff),2000,stdin);
      }
      int n = write( sock, buff, strlen( buff ) );
      if ( n < strlen( buff ) )
      {
        perror( "write() failed" );
        exit( EXIT_FAILURE );
      }
      
      char buffer[ BUFFER_SIZE ];
      n = read( sock, buffer, BUFFER_SIZE );  // BLOCK
      if ( n < 0 )
      {
        perror( "read() failed" );
        exit( EXIT_FAILURE );
      }
      else
      {
        buffer[n] = '\0';
        printf( "Received message from server: %s\n", buffer );
      }
      
      close( sock );
      
      return EXIT_SUCCESS;
    }
  }
}