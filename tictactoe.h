#ifndef TICTACTOE_H
#define TICTACTOE_H



#define PLAYER_WINS -10
#define AI_WINS 10
#define TIE 0

typedef struct {
    int x;
    int y;
} coordinate;

int won(char** board,  char player, coordinate last_played);



coordinate best_move(char** board, int size);


#endif
