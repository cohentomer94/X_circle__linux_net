#ifndef TICTACTOE_H
#define TICTACTOE_H

#define PLAYER2 'O'
#define PLAYER1 'X'

#define PLAYER1_WINS -10
#define PLAYER2_WINS 10
#define TIE 0

typedef struct {
    int x;
    int y;
} coordinate;

int max(int num1, int num2);

int min(int num1, int num2);

int won(char** board, int size, char player, coordinate last_played);

int check_winner(char** board, int size);

int minimax(char** board, int size, int is_maximizing, int alpha, int beta);

coordinate best_move(char** board, int size);

void print_board(char** board, int size,char *buf_game);

#endif
