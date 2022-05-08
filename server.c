#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ctype.h>


#include "tictactoe.h"


#define PORT "9034"   // port we're listening on



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    int numplayer = 0;
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    char buf_game[500];
    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                            
                            
                            numplayer+=1;
                         printf("num of connect : %d",numplayer);
                         if(numplayer==2){
/////////////////////////////////////////////////
				
   printf("You are playing Tic Tac Toe.\n");

    int gameloop = 1;
    char buf_game[1000];
    int moves = 0;
    int row, col;
    while (gameloop) {
        int size =3;

        char** board = (char**)calloc(size, sizeof(char*));
        for (int i = 0; i < size; i++) {
            board[i] = (char*)calloc(size, sizeof(char));
        }
	
	
        print_board(board, size,buf_game);
        send(4, buf_game, 200, 0);sleep(1);                 
        send(5, buf_game, 200, 0); sleep(1);  
        send(4, ">PLAYER1 Enter row betwen 0-2 ", 200, 0);sleep(1);
        send(5, ">PLAYER2 please wait", 200, 0);sleep(1);
         while (1) {
            // PLAYER1 MAKES MOVE

            while ((nbytes = recv(4, buf, sizeof buf, 0)) == 0);
            	row=buf[0]-'0';sleep(1);
            	send(4, ">PLAYER1 Enter col betwen 0-2 ", 100, 0); sleep(1);
            while ((nbytes = recv(4, buf, sizeof buf, 0)) == 0);
            	col=buf[0]-'0';
            while (!(0 <= row && row < size && 0 <= col && col < size) || board[row][col]) {
                send(4,"The values for row and col must be greater than 0 and smaller than 3, make sure you select an empty spot.\n", 			 200, 0);sleep(0.5);
               send(4, ">PLAYER1 Enter row betwen 0-2 ", 50, 0); sleep(0.5);
            	while ((nbytes = recv(4, buf, sizeof buf, 0)) == 0);
            		row=buf[0]-'0';
            	send(4, ">PLAYER1 Enter col betwen 0-2 ", 50, 0); sleep(0.5);
            	while ((nbytes = recv(4, buf, sizeof buf, 0)) == 0);
            		col=buf[0]-'0';
            }

            board[row][col] = PLAYER1;
                    send(4, buf_game, 200, 0);sleep(0.5);
                    send(5, buf_game, 200, 0) ;sleep(0.5);
            

            moves++;
            print_board(board, size,buf_game);
                    send(4, buf_game, 200, 0);sleep(0.5);
                    send(5, buf_game, 200, 0) ;sleep(0.5);
            puts(buf_game);

            if (won(board, size, PLAYER1, (coordinate){row, col})) {
                send(4,"you won!",20, 0); sleep(0.5);
                send(5,"you loss",20, 0); sleep(0.5);
                break;
            }

            if (moves >= size * size) {
                send(4,"Tie.\n",20, 0); sleep(0.5);
                send(5,"Tie.\n",20, 0); sleep(0.5);
                break;
            }

	
            // PLAYER2 MAKES MOVE

            send(5, ">PLAYER2 Enter row betwen 0-2 ", 50, 0);sleep(0.5);            
            send(4, ">PLAYER's turn 1 ", 50, 0);  sleep(0.5);
            while ((nbytes = recv(5, buf, sizeof buf, 0)) == 0||buf=="\n");
            row=buf[0]-'0';
            send(5, ">PLAYER2 Enter col betwen 0-2 ", 50, 0); 
            while ((nbytes = recv(5, buf, sizeof buf, 0)) == 0);
            col=buf[0]-'0';
            while (!(0 <= row && row < size && 0 <= col && col < size) || board[row][col]) {
                send(5,"The values for row and col must be greater than 0 and smaller than 3, make sure you select an empty spot.\n", 			 200, 0);sleep(0.5);
                send(5, ">PLAYER2 Enter row betwen 0-2 ", 50, 0);sleep(0.5); 
		
            	while ((nbytes = recv(5, buf, sizeof buf, 0)) == 0);
            	row=buf[0]-'0';
            	send(5, ">PLAYER2 Enter col betwen 0-2 ", 50, 0);sleep(0.5); 
            	while ((nbytes = recv(5, buf, sizeof buf, 0)) == 0);
            	col=buf[0]-'0';

            }

            send(5,buf_game,100, 0); sleep(0.5); 
            board[row][col] = PLAYER2;
                    send(4, buf_game, 200, 0);
                    send(5, buf_game, 200, 0) ;
            moves++;
            print_board(board, size,buf_game);
            send(4, buf_game, 200, 0);
            send(5, buf_game, 200, 0) ;
            if (won(board, size, PLAYER2, (coordinate){row, col})) {
                send(5,"you won!",20, 0); sleep(0.5); 
                send(4,"you loss",20, 0); sleep(0.5); 
                break;
            }

            if (moves >= size * size) {
                send(4,"Tie.\n",20, 0); sleep(0.5);
                send(5,"Tie.\n",20, 0); sleep(0.5);
                break;
            }
}
        moves = 0;

        for (int i = 0; i < size; i++) {
            free(board[i]);
            board[i] = NULL;
        }
        free(board);
        board = NULL;

        printf("Do you want to play again? (y/n): ");
        char c;
        scanf(" %c", &c);
        gameloop = tolower(c) == 'y';
    }

    printf("Thank you for playing.\n");


/////////////////////////////////////////////////
                         
                            }

			 
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        for(j = 0; j <= fdmax; j++) {
                        
                        
                     //   int byte_count = recv(4, buf, sizeof buf, 0);
			// printf("recv()'d %d bytes of data in buf\n", byte_count);
			 //byte_count = recv(newfd+1, buf, sizeof buf, 0);
			 //printf("recv()'d %d bytes of data in buffrom: %d", byte_count,newfd+1);
			 
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
