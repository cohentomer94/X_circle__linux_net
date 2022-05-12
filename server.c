
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "tictactoe.h"

#define size 3

#define PLAYER2 'O'
#define PLAYER1 'X'
#define PORT "9034"   // port we're listening on
int won(char** board, char player, coordinate last_played) {
    int i;

    // check horizontal
    i = 0;
    while (i < size && board[last_played.x][i] == player) {
        i++;
    }
    if (i == size) {
        return 1;
    }

    // check vertical
    i = 0;
    while (i < size && board[i][last_played.y] == player) {
        i++;
    }
    if (i == size) {
        return 1;
    }

    // check diagonal
    if (last_played.x == last_played.y || (last_played.x == size - 1 && last_played.y == 0) || (last_played.x == 0 && last_played.y == size - 1)) {
        i = 0;
        while (i < size && board[i][i] == player) {
            i++;
        }
        if (i == size) {
            return 1;
        }

        i = 0;
        while (i < size && board[i][size - i - 1] == player) {
            i++;
        }
        if (i == size) {
            return 1;
        }
    }
    return 0;
}
int move_player(char ***board,int row,int col,int player){

	if (!(0 <= row && row < size && 0 <= col && col < size) || (*board)[row][col]) {
	
		printf("no good");
                return 2;

            }
            printf("You entered %d and %d\n", row, col);
            if(player==0) (*board)[row][col] = PLAYER1;
            else (*board)[row][col] = PLAYER2;
            return 1;

            	
	
	

}

char **build_game(fd_set master ,int fdmax,int listener){
        char** board = (char**)calloc(size, sizeof(char*));
        for (int i = 0; i < size; i++) {
            board[i] = (char*)calloc(size, sizeof(char));
            
}
for(int j = 0; j <= fdmax; j++) {
    if (FD_ISSET(j, &master)) {
    	if (j != listener) {
		send(j, "lets start play", 100, 0);}}}
return board;
}
void print_board(char** board, fd_set master ,int fdmax,int listener) {
    char buf_game1[1000];
    char *buf_game=buf_game1;
    int l=0;
    l+=sprintf((buf_game+l),"\n");
    l+=sprintf((buf_game+l),"  ");
    for (int i = 0; i < size; i++) {
        l+=sprintf((buf_game+l)," %d", i);
        
    }
    
    l+=sprintf((buf_game+l),"\n");
  //  puts(buf_game);
    for (int i = 0; i < size; i++) {
        l+=sprintf((buf_game+l),"%d |", i);

        for (int j = 0; j < size; j++) {
            if (!board[i][j]) {
                l+=sprintf((buf_game+l),".|");
            } else {
                l+=sprintf((buf_game+l),"%c|", board[i][j]);
            }
        }
        l+=sprintf((buf_game+l),"\n");
    }
    
    puts(buf_game);
    for(int j = 0; j <= fdmax; j++) {
    // send to everyone!
    	if (FD_ISSET(j, &master)) {
    // except the listener and ourselves
    	if (j != listener) {
     			send(j, buf_game, 100, 0);
    			//perror("send");
}}
}}
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

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

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
    int players=0,player_turn=1,move_play=0,status_input=0,moves=0;
    char **board=NULL;
    int row=0,col=0,player1=0,player2=0,flag=0;
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
                            	players +=1;
                            if (players==1) {
                            	player1=newfd;
                            	send(player1, "wait for player", 50, 0);
                            }
                            if (players==2){
                            	board = build_game(master , fdmax, listener);
                            	print_board(board,master,fdmax,listener);
                            	player2=newfd;
                            	player_turn=player1;
                            	send(player1, "lets start, its your turn!\nplease press row press Enter end press column:", 50, 0);
                            	send(player2, "lets start, wait for your turn", 50, 0);
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
                    	if (player_turn==i){ 
					move_play+=1;
			    	if (move_play == 1){
			    		row= buf[0] -'0';}
			    	if (move_play==2){
			    		col = buf[0] -'0';
			    	if(player_turn == player1)
			    		status_input = move_player(&board,row,col,0);
			    	else 
		    			status_input = move_player(&board,row,col,1);
		    	move_play=0;
		            	}
		            /* 1 - god mov (betwen 0-2)
		               2 - bad move 
		             */  
		             if (status_input ==1 ){	//this player end turn
		    		print_board(board,master,fdmax,listener);
		    		status_input=0;
		    		if(player_turn == player1)
		            		player_turn = player2;
		            	else 
		    			player_turn = player1;
		            	moves++;
	    			if (won(board, PLAYER1, (coordinate){row, col})) {
					send(i, "Congratiolations, you won!\n", 50, 0);
		             			for(j = 0; j <= fdmax; j++) {
		                    			if (FD_ISSET(j, &master)) {
		                        			if (j != listener && j != i) {
		                            				 send(j, "you lose", 20, 0) ;
		                            				 flag=1;
		                            				 }
		                    				}
		            				}
	    				}
		            	if (won(board, PLAYER2, (coordinate){row, col})) {
					send(i, "Congratiolations, you won!\n", 50, 0);//printf("Congratiolations, you won!\n");
		             			for(j = 0; j <= fdmax; j++) {
		                    			if (FD_ISSET(j, &master)) {
		                        			if (j != listener && j != i) {
		                            				 send(j, "you lose", 20, 0) ;
		                            				 flag=1;
		                            				}
		                            			}
		                    			}
		            			}
		                if (moves >= size * size) {
		                	for(j = 0; j <= fdmax; j++) {
		                		if (FD_ISSET(j, &master)&&(j != listener)) {
		                			 send(j, "Tie", 20, 0) ;
		                			 flag=1;
		        					}  
		                            		}
		                    		}
		                if(player_turn == player1){
		        		send(player1, "It's your turn!\nplease press row press Enter end press column:", 200, 0);
		        	 	send(player2, "wait for you turn.", 50, 0);
		                	}
				else{
					send(player2, "It's your turn!\nplease press row press Enter end press column:", 200, 0);
		        	 	send(player1, "wait for you turn.", 50, 0);
			 		}
	 			}
			     if (status_input ==2 ){
		             	printf("enter egain");
		             	move_play=status_input=0;
		             	}
             		}

                     else{
                     send(i, "its not your turn!!!!", 30, 0);   
                      }        
                    }
                    if (flag==1) exit;
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}
