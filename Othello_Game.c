#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <assert.h>

#define Board_Size 8
#define TRUE 1
#define FALSE 0

void Delay(unsigned int mseconds);
void Read_File( char *c );//open a file and get the next move, for play by file
char Load_File( void );//load a file and start a game

void Init();
int Play_a_Move( int x, int y);
void  Show_Board_and_Set_Legal_Moves( void );
int  Put_a_Stone( int x, int y );

int  In_Board( int x, int y );
int  Check_Cross( int x, int y, int update );
int  Check_Straight_Army( int x, int y, int d, int update );

int  Find_Legal_Moves(int color);
int  Check_EndGame( void );
int  Compute_Grades(int flag);

void Computer_Think( int *x, int *y);
int Search(int myturn, int mylevel);
int search_next(int x, int y, int myturn, int mylevel, int alpha, int beta);


int Search_Counter;
int Computer_Take;
int Winner;
int Now_Board[ Board_Size ][ Board_Size ];
//mark the dead stone
//int Dead_Stone[ Board_Size ][ Board_Size ];
int Legal_Moves[ Board_Size ][ Board_Size ];
int HandNumber;
int sequence[100];

int Black_Count, White_Count;
int Turn = 0;// 0 is black or 1 is white
int Stones[2]= {1,2}; // 1: black, 2: white
int DirX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
int DirY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

int LastX, LastY;
//int Auto_Check_Dead;

int search_deep = 6;
int count=0;
int alpha_beta_option = TRUE;
int resultX, resultY;
//int search_level_now;
//int board_weight[ Board_Size ][ Board_Size ];
int board_weight[8][8] =
//a, b, c, d, e, f, g,  h
{
    {64, -10, 10, 10, 10, 10, -10, 64},//1
    {-10, -20, 1, 1, 1, 1, -20, -10},//2
    {10, 1, 1, 1, 1, 1, 1, 10},//3
    {10, 1, 1, 1, 1, 1, 1, 10},//4
    {10, 1, 1, 1, 1, 1, 1, 10},//5
    {10, 1, 1, 1, 1, 1, 1, 10},//6
    {-10, -20, 1, 1, 1, 1, -20, -10},//7
    {64, -10, 10, 10, 10, 10, -10, 64}
};//8

typedef struct location
{
    int i;
    int j;
    int g;
} Location;

//---------------------------------------------------------------------------

int main()
{
    char compcolor = 'W', c[10];
    int column_input, row_input ;
    int rx, ry, m=0, n;
    FILE *fp;

    Init();
    printf("Computer take?(B/W/All/File play as first/file play as Second/Load and play): ");
    scanf("%c", &compcolor);
    Show_Board_and_Set_Legal_Moves();

    if (compcolor == 'L' || compcolor == 'l')
        compcolor = Load_File();

    if ( compcolor == 'B' || compcolor == 'b')
    {
        Computer_Think( &rx, &ry );
        printf("Computer played %c%d\n", rx+97, ry+1);
        Play_a_Move( rx, ry );
        Show_Board_and_Set_Legal_Moves();
    }

    if ( compcolor == 'A' || compcolor == 'a')
        while ( m++ < 64 )
        {
            Computer_Think( &rx, &ry );
            if ( !Play_a_Move( rx, ry ))
            {
                printf("Wrong Computer moves %c%d\n", rx+97, ry+1);
                scanf("%d", &n);
                break;
            }
            if ( rx == -1 )
                printf("Computer Pass\n");
            else
                printf("Computer played %c%d\n", rx+97, ry+1);

            if ( Check_EndGame() )
                return 0;
            Show_Board_and_Set_Legal_Moves();
        }

    if ( compcolor == 'F')
    {
        Computer_Think( &rx, &ry );
        Play_a_Move( rx, ry );
    }

    while ( m++ < 64 )
    {
        while (1)
        {
            if ( compcolor == 'F' || compcolor == 'S' )
            {
                fp = fopen( "of.txt", "r" );
                fscanf( fp, "%d", &n );
                fclose( fp );
                if (compcolor == 'F' )
                {
                    if ( n % 2 == 0 )
                        Read_File( c );
                    else
                    {
                        Delay(100);
                        continue;
                    }
                }
                else
                {
                    if ( n % 2 == 1 )
                        Read_File( c );
                    else
                    {
                        Delay(100);
                        continue;
                    }
                }

             }


            if ( compcolor == 'B' )
            {
                printf("input White move:(a-h 1-8), or PASS\n");
                scanf("%s", c);
            }
            else if ( compcolor == 'W' )
            {
                printf("input Black move:(a-h 1-8), or PASS\n");
                scanf("%s", c);
            }

            if ( c[0] == 'P' || c[0] == 'p')
                row_input = column_input = -1;
            else if ( c[0] == 'M' || c[0] == 'm' )
            {
                Computer_Think( &rx, &ry );
                if ( !Play_a_Move( rx, ry ))
                {
                    printf("Wrong Computer moves %c%d\n", rx+97, ry+1);
                    scanf("%d", &n);
                    break;
                }
                if ( rx == -1 )
                    printf("Computer Pass");
                else
                    printf("Computer played %c%d\n", rx+97, ry+1);
                if ( Check_EndGame() )
                    break;
                Show_Board_and_Set_Legal_Moves();
            }
            else
            {
                row_input= c[0] - 97;
                column_input = c[1] - 49;
            }
//            printf("%d, %d\n", row_input, column_input);

            if ( !Play_a_Move(row_input, column_input) )
                printf("%c%d is a Wrong move\n", c[0], column_input+1);
            else
                break;
        }
        if ( Check_EndGame() )
            return 0;;
        Show_Board_and_Set_Legal_Moves();

        Computer_Think( &rx, &ry );
        printf("Computer played %c%d\n", rx+97, ry+1);
        Play_a_Move( rx, ry );
        if ( Check_EndGame() )
            return 0;;
        Show_Board_and_Set_Legal_Moves();

    }

    printf("Game is over");
    scanf("%d", &n);

    return 0;
}
//---------------------------------------------------------------------------
void Delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
//---------------------------------------------------------------------------

void Read_File( char *c )
{
    FILE *fp;
    char tc[10];

    fp = fopen( "of.txt", "r" );
    while ( (fscanf( fp, "%s", tc ) ) != EOF )
    {
        c[0] = tc[0];
        c[1] = tc[1];
    }
}
//---------------------------------------------------------------------------

char Load_File( void )
{
    FILE *fp;
    char tc[10];
    int row_input, column_input, n;

    fp = fopen( "of.txt", "r" );
    assert( fp != NULL );

    fscanf( fp, "%d", &n );

    while ( (fscanf( fp, "%s", tc ) ) != EOF )
    {
        row_input= tc[0] - 97;
        column_input = tc[1] - 49;
        if ( !Play_a_Move(row_input, column_input) )
            printf("%c%d is a Wrong move\n", tc[0], column_input+1);

        Show_Board_and_Set_Legal_Moves();
    }
    return ( n%2 == 1 )? 'B' : 'W';
}
//---------------------------------------------------------------------------

void Init()
{
    Computer_Take = 0;
    memset(Now_Board, 0, sizeof(int) * Board_Size * Board_Size);

    Now_Board[3][3] = Now_Board[4][4] = 2;//white, dark
    Now_Board[3][4] = Now_Board[4][3] = 1;//black, light

    HandNumber = 0;
    memset(sequence, -1, sizeof(int) * 100);
    Turn = 0;

    LastX = LastY = -1;
    Black_Count = White_Count = 0;

//  Computer_Thinking = FALSE;
    //debug_value = 0;
    Search_Counter = 0;

    //Computer_Take = 0;
    Winner = 0;
}
//---------------------------------------------------------------------------

int Play_a_Move( int x, int y)
{
    FILE *fp;

    if ( x == -1 && y == -1)
    {
        fp = fopen( "of.txt", "r+" );

        fprintf( fp, "%2d\n", HandNumber+1 );
        fclose(fp);
        fp = fopen("of.txt", "a");
        fprintf( fp, "p9\n" );
        fclose( fp );

        sequence[HandNumber] = -1;
        HandNumber ++;
        Turn = 1 - Turn;
        return 1;
    }

    if ( !In_Board(x,y))
        return 0;
    Find_Legal_Moves( Stones[Turn] );
    if( Legal_Moves[x][y] == FALSE )
        return 0;

    if( Put_a_Stone(x,y) )
    {
        Check_Cross(x,y,1);
        Compute_Grades( TRUE );

//        Show_Board_and_Set_Legal_Moves();

//        printf("Play %c%d\n", x+97, y+1);
        return 1;
    }
    else
        return 0;
    return 1;
}
//---------------------------------------------------------------------------

int Put_a_Stone(int x, int y)
{
    FILE *fp;

    if( Now_Board[x][y] == 0)
    {
        sequence[HandNumber] = Turn;
        if (HandNumber == 0)
            fp = fopen( "of.txt", "w" );
        else
            fp = fopen( "of.txt", "r+" );
        fprintf( fp, "%2d\n", HandNumber+1 );
        HandNumber ++;
        fclose(fp);

        Now_Board[x][y] = Stones[Turn];
        fp = fopen("of.txt", "a");
        fprintf( fp, "%c%d\n", x+97, y+1 );;
        fclose( fp );

        LastX = x;
        LastY = y;

        Turn = 1 - Turn;

        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------

void Show_Board_and_Set_Legal_Moves( void )
{
    int i,j;

    Find_Legal_Moves( Stones[Turn] );

    printf("a b c d e f g h\n");
    for(i=0 ; i<Board_Size ; i++)
    {
        for(j=0 ; j<Board_Size ; j++)
        {
            if( Now_Board[j][i] > 0 )
            {
                if ( Now_Board[j][i] == 2 )
                    printf("O ");//white
                else
                    printf("X ");//black
            }

            if (Now_Board[j][i] == 0)
            {
                if ( Legal_Moves[j][i] == 1)
                    printf("? ");
                else printf(". ");
            }
        }
        printf(" %d\n", i+1);
    }
    printf("\n");
}
//---------------------------------------------------------------------------

int Find_Legal_Moves( int color )
{
    int i,j;
    int me = color;
    int legal_count = 0;

    for( i = 0; i < Board_Size; i++ )
        for( j = 0; j < Board_Size; j++ )
            Legal_Moves[i][j] = 0;

    for( i = 0; i < Board_Size; i++ )
        for( j = 0; j < Board_Size; j++ )
            if( Now_Board[i][j] == 0 )
            {
                Now_Board[i][j] = me;
                if( Check_Cross(i,j,FALSE) == TRUE )
                {
                    Legal_Moves[i][j] = TRUE;
                    legal_count ++;
                }
                Now_Board[i][j] = 0;
            }

    return legal_count;
}
//---------------------------------------------------------------------------
int Check_Cross(int x, int y, int update)
{
    int k;
    int dx, dy;

    if( ! In_Board(x,y) || Now_Board[x][y] == 0)
        return FALSE;

    int army = 3 - Now_Board[x][y];
    int army_count = 0;

//   printf("%d, %d, %d, %d\n", x, y, Now_Board[x][y], army);
    for( k=0 ; k<8 ; k++ )
    {
        dx = x + DirX[k];
        dy = y + DirY[k];
        if( In_Board(dx,dy) && Now_Board[dx][dy] == army )
        {
            army_count += Check_Straight_Army( x, y, k, update ) ;
            //printf("%d, ", army_count);
        }
    }

    if(army_count >0)
        return TRUE;
    else
        return FALSE;
}
//---------------------------------------------------------------------------

int Check_Straight_Army(int x, int y, int d, int update)
{
    int me = Now_Board[x][y];
    int army = 3 - me;
    int army_count=0;
    int found_flag=FALSE;
    int flag[ Board_Size ][ Board_Size ] = {{0}};

    int i, j;
    int tx, ty;

    tx = x;
    ty = y;

    for(i=0 ; i<Board_Size ; i++)
    {
        tx += DirX[d];
        ty += DirY[d];

        if(In_Board(tx,ty) )
        {
            if( Now_Board[tx][ty] == army )
            {
                army_count ++;
                flag[tx][ty] = TRUE;
            }
            else if( Now_Board[tx][ty] == me)
            {
                found_flag = TRUE;
                break;
            }
            else
                break;
        }
        else
            break;
    }

    if( (found_flag == TRUE) && (army_count > 0) && update)
    {
        for(i=0 ; i<Board_Size ; i++)
            for(j=0; j<Board_Size ; j++)
                if(flag[i][j]==TRUE)
                {
                    //Dead_Stone[i][j] = TRUE;
                    if(Now_Board[i][j] != 0)
                        Now_Board[i][j]= 3 - Now_Board[i][j];
                }
    }
    if( (found_flag == TRUE) && (army_count > 0))
        return army_count;
    else return 0;
}
//---------------------------------------------------------------------------

int In_Board(int x, int y)
{
    if( x >= 0 && x < Board_Size && y >= 0 && y < Board_Size )
        return TRUE;
    else
        return FALSE;
}
//---------------------------------------------------------------------------

int Compute_Grades(int flag)
{
    int i,j;
    int B,W, BW, WW;

    B = BW = W = WW = 0;

    for(i=0 ; i<Board_Size ; i++)
        for(j=0 ; j<Board_Size ; j++)
        {
            if( Now_Board[i][j]==1 )
            {
                B++;
                BW = BW + board_weight[i][j] ;
            }
            else if( Now_Board[i][j]==2 )
            {
                W++;
                WW = WW + board_weight[i][j] ;
            }
        }

    if(flag)
    {
        Black_Count = B;
        White_Count = W;
        count=Black_Count+White_Count;
        if(count>=16 && count<=40){
            search_deep=8;
        }
        else if(count>40){
            search_deep=10;
        }
        printf("Grade: Black %d, White %d\n", B, W);
    }

    return ( BW - WW );
}
//---------------------------------------------------------------------------

int Check_EndGame( void )
{
    int lc1, lc2;

    lc2 = Find_Legal_Moves( Stones[1 - Turn] );
    lc1 = Find_Legal_Moves( Stones[Turn] );

    if ( lc1==0 && lc2==0 )
    {
        if(Black_Count > White_Count)
        {
            printf("Black Win!\n");
            if(Winner == 0)
                Winner = 1;
        }
        else if(Black_Count < White_Count)
        {
            printf("White Win!\n");
            if(Winner == 0)
                Winner = 2;
        }
        else
        {
            printf("Draw\n");
            Winner = 0;
        }
        Show_Board_and_Set_Legal_Moves();
        printf("Game is over");
        scanf("%d", &lc1);
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------

void Computer_Think( int *x, int *y )
{
    time_t clockBegin, clockEnd;
    static int think_time=0;
    int flag;

    clockBegin = clock();

    resultX = resultY = -1;
    Search_Counter = 0;

    flag = Search( Turn, 0);

    clockEnd = clock();
    think_time += clockEnd - clockBegin ;
    printf("used thinking time= %d min. %d.%d sec.\n", think_time/60000, (think_time%60000)/1000,  (think_time%60000)%1000);

    if( flag )
    {
        *x = resultX;
        *y = resultY;
    }
    else
    {
        *x = *y = -1;
        return;
    }

}
//---------------------------------------------------------------------------

//MinMax search
int Search(int myturn, int mylevel)
{
    int i,j;

    Location min, max;

    min.i = min.j = -1;
    min.g = 99999;

    max.i = max.j = -1;
    max.g = -99999;

    int B[ Board_Size ][ Board_Size ];
    int L[ Board_Size ][ Board_Size ];

    memcpy( B, Now_Board, sizeof(int) * Board_Size * Board_Size );
    //search_level_now = 0;

    int c = Find_Legal_Moves( Stones[myturn] );
    if(c <= 0) return FALSE;

    memcpy( L, Legal_Moves, sizeof(int) * Board_Size * Board_Size );

    int alpha = -99999;
    int beta = 99999;
    int g = -1;

    for( i = 0; i < Board_Size; i++ )
        for( j = 0; j < Board_Size; j++ )
            if(L[i][j] == TRUE)//you could add move ordering
            {
                memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size );
                Now_Board[i][j] = Stones[myturn];
                Check_Cross(i,j,TRUE) ;

                g = search_next(i,j, 1-myturn, mylevel+1, alpha, beta);

                if( myturn == 0 )  // max level
                    if( g > max.g )
                    {
                        max.g = g;
                        max.i = i;
                        max.j = j;
                    }

                if( myturn == 1 )
                    if( g < min.g )
                    {
                        min.g = g;
                        min.i = i;
                        min.j = j;
                    }
                /*
                if( alpha_beta_option == TRUE)
                    if( alpha >= beta)
                    {
                    i = Board_Size;
                    j = Board_Size;
                    }
                */
            }

    memcpy( Now_Board, B, sizeof(int) * Board_Size * Board_Size );

    if( myturn == 0 )
    {
        resultX = max.i;
        resultY = max.j;
        return TRUE;
    }
    else if( myturn == 1 )
    {
        resultX = min.i;
        resultY = min.j;
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------

int search_next( int x, int y, int myturn, int mylevel, int alpha, int beta )
{
    int g;

    Search_Counter ++;

    if( mylevel >= search_deep )
    {
        g = Compute_Grades( FALSE );
        return g;
    }
    else
    {
        int i,j;
        ///int my_alpha = alpha; int my_beta = beta;

        //Location min; min.i = -1; min.j = -1; min.g = 99999;
        //Location max; max.i = -1; max.i = -1; max.g = -99999;

        int B[ Board_Size ][ Board_Size ];
        int L[ Board_Size ][ Board_Size ];

        int c = Find_Legal_Moves( Stones[myturn] );
        if(c <= 0)
        {
            g = Compute_Grades( FALSE );
            return g;
        }

        memcpy( B, Now_Board, sizeof(int) * Board_Size * Board_Size );
        memcpy( L, Legal_Moves, sizeof(int) * Board_Size * Board_Size );

        for(i=0; i<Board_Size ; i++)
            for(j=0 ; j<Board_Size ; j++)
                if(L[i][j] == TRUE)//you could add move ordering
                {

                    memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size );
                    Now_Board[i][j] = Stones[myturn];
                    Check_Cross(i,j, TRUE) ;

                    g = search_next(i,j, 1-myturn, mylevel+1, alpha, beta);// could use transposition table

                    //Memo1->Lines->Add("lv=" + IntToStr(mylevel) + ",g=" + IntToStr(g) );

                    if(myturn==0)  // max ply
                    {
                        if(g > alpha)
                            alpha = g;
                    }
                    if(myturn==1)// min ply
                    {
                        if(g < beta)
                            beta = g;
                    }

                    if( alpha_beta_option == TRUE)
                        if( alpha >= beta )// cutoff
                        {
                            i = Board_Size;
                            j = Board_Size;
                            break;
                        }
                }

        memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size);

        if(myturn == 0) // max level
            return alpha; //max.g;
        else
            return beta; //min.g;
    }
}
//---------------------------------------------------------------------------
