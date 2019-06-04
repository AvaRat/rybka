#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "templates.h"

void clear_memory(Plansza *plansza)
{
    printf("czyszczenie pamieci i zamykanie plikow\n");
    for(int i=0; i<plansza->n_rows; i++)
    {
        free(plansza->pole[i]);
    }
    free(plansza->pole);
}
void print_game_info(Plansza plansza, GameParameters params)
{
    printf("\nfaza gry: %s\nn_pingwinow: %d\ninputFile: %s\noutputFile: %s\n", \
        params.phase ? "movement":"placement", params.penguins, params.inputboardfile, params.outputboardfile);
    printf("liczba_graczy: %d\nzebrane ryby: %d\nnasz_nr: %d\n", params.n_players, *plansza.nasza_ilosc_ryb, plansza.nasz_nr);
    printf("\n    ");
    for(int i=0; i<plansza.n_cols; i++)
    {
        printf("%d ", i+1);
        if(i < 9)
            printf(" ");
    }
    printf("\n");
    for(int i=0; i<plansza.n_rows; i++)
    {
        printf("%d:  ", i+1);
        for(int j=0; j<plansza.n_cols; j++)
        {
            printf("%d%d ", plansza.pole[i][j].ileRyb, plansza.pole[i][j].nrGracza);
        }
        printf("\n");
    }
}

int read_file(FILE *fp_in, Plansza *plansza, GameParameters *params)
{
    int n_rows, n_cols;
    fscanf(fp_in,"%d", &n_rows);
    fscanf(fp_in,"%d", &n_cols);

    plansza->n_rows = n_rows;
    plansza->n_cols = n_cols;
    printf("alokowanie pamieci ...\t");
    plansza->pole = malloc(n_rows*sizeof(POLE *));
    for(int i=0; i<n_rows; i++)
    {
        plansza->pole[i]=malloc(n_cols*sizeof(POLE));
    }
    printf("ok\n");
    for(int i=0; i<n_rows; i++)
    {
        for(int j=0; j<n_cols; j++)
        {
            char a;
            do
            {
                a=fgetc(fp_in);
                if(a>='0' && a<='3')
                {
                    plansza->pole[i][j].ileRyb=a-'0';
                }
            }
            while((a==' ' || a=='\n'));
            do
            {
                a=fgetc(fp_in);
                if(a>='0' && a<='9')
                    plansza->pole[i][j].nrGracza=a-'0';
                else
                {
                    printf("Blad wewnetrzny pliku!\n");
                    return -2;
                }
            }
            while((a==' ' || a=='\n'));
        }
    }
    char player_name[20];
    int n_ryb, nr_gracza;
    players_stats_type player;
    int myidx = -1;
    int i=0;
    printf("pobieranie statystyk graczy ...\t");
    while(fscanf(fp_in, "%s %d %d", player_name, &nr_gracza, &n_ryb) != EOF)
    {        
        player.n_ryb = n_ryb;
        player.nr_gracza = nr_gracza;
        plansza->players_stats[i] = player;
        strcpy(plansza->players_stats[i].name, player_name);
        if(strcmp(player_name, TEAM_NAME) == 0)
            myidx = i;  // index of our team name
        i++;
    }

    if(i == 0 || myidx < 0)  //nie ma jeszcze zadnych graczy
    {
        assert(params->phase == placement);
        // pierwsze uruchomienie naszego programu
        plansza->players_stats[i].nr_gracza = i+1;
        plansza->players_stats[i].n_ryb = 0;
        strcpy(plansza->players_stats[i].name, TEAM_NAME);
        if(myidx < 0)
            myidx = i;
        else myidx = 0;
        i++;
    }
    params->n_players = i;
    plansza->nasz_nr = myidx+1;
    plansza->nasza_ilosc_ryb = &(plansza->players_stats[myidx].n_ryb);
    printf("ok\n");
    return 0;
}
int save_to_file(FILE *fp_in, FILE *fp_out, Plansza *plansza, GameParameters params)
{
    printf("zapisywanie do pliku ...\t");
    fprintf(fp_out,"%d %d\n",plansza->n_rows, plansza->n_cols);
    for(int i=0; i<plansza->n_rows; i++)
    {
        for(int j=0; j<plansza->n_cols; j++)
        {
            fprintf(fp_out,"%d%d ",plansza->pole[i][j].ileRyb, plansza->pole[i][j].nrGracza);
        }
        fprintf(fp_out,"\n");
    }
    printf("ok\nzapisywanie statystyk ...\t");
    //zapisz statystyki graczy
    for(int i=0; i<params.n_players; i++)
    {
        char *name = plansza->players_stats[i].name;
        int n_ryb = plansza->players_stats[i].n_ryb;
        int nr_gracza = plansza->players_stats[i].nr_gracza;
        fprintf(fp_out, "%s %d %d\n", name, nr_gracza, n_ryb);
    }
    printf("ok\n");
    return 0;
}
int split(char *arg, char *value)
{
    if(!arg)
        return -1;

    char ch;
    int len = 0;
    int loc = 0;
    for(len=0, ch=arg[0]; ch != '\0'; ch=arg[len++])
    {
        if(ch == '\0')
            return -1;
        if(ch == '=')
            loc = len;
    }
    if(loc == 0)
        return -1;
    for(int n=0; n<len; n++)
        value[n] = arg[n+loc];
    arg[loc-1] = '\0';
    return 0;
}

int get_params(int argc, char **argv, GameParameters *params)
{
    phase_type phase;
    int penguins = 0;
    char inputboardfile[40];
    char outputboardfile[40];

    for(int i=1; i<argc; i++)
    {
        char *arg = argv[i];
        char value[40];
        if(split(arg, value) != 0)
        {
            printf("error in split function\n");
            return 3;
        }
        if(!strcmp(arg, "phase"))
        {
            if(!strcmp(value, "placement"))
                phase = placement;
            else
                phase = movement;
        }
        else if(!strcmp(arg, "penguins"))
        {
            sscanf(value, "%d", &penguins);
        }
        else if(!strcmp(arg, "inputboardfile"))
            strcpy(inputboardfile,value);
        else if(!strcmp(arg, "outputboardfile"))
            strcpy(outputboardfile, value);
        else
        {
            printf("wrong arguments!!\n");
            return -2;
        }
    }
    strcpy(params->inputboardfile, inputboardfile);
    strcpy(params->outputboardfile, outputboardfile);
    params->penguins = penguins;
    params->phase = phase;
    return 0;
}

/**
 * ustawia pingwina na zadanym polu
 * zwraca 
 *  0->ok
 *  2->nie mam wiecej pingwinow
 *  1->na tym polu nie mozna ustawic pingwina
*/
int ustaw_pingwina(Plansza *plansza, GameParameters params, int x, int y)
{
    assert(params.phase == placement);
    if(params.penguins <= *plansza->nasza_ilosc_ryb)
    {
        printf("nie mam juz zadnych pingwinow do rozlozenia\n");
        return 1;
    }
    if(plansza->pole[x][y].nrGracza!=0 || plansza->pole[x][y].ileRyb!=1)
    {
            printf("Na tym polu nie mozna ustawic pingwina!\n");
            return -1;
    } else
    {
        plansza->pole[x][y].nrGracza=TEAM_NR;
        *(plansza->nasza_ilosc_ryb) += plansza->pole[x][y].ileRyb;
        plansza->pole[x][y].ileRyb=0;
    }
    return 0;
}
/**
 *zwraca
 *  0->ok
 *  1->nie mozna tam isc
 * 
*/ 
int move_penguins(Plansza *plansza, GameParameters params, Wspolrzedne_pola pingwin, Wspolrzedne_pola ruch)
{
    int x = pingwin.row;
    int y = pingwin.column;
    int z = ruch.row;
    int t = ruch.column;
    assert(params.phase == movement);
    if(x == z && y>=t)
    {
        for(int i=1; i<=y-t; i++)
        {
            if(y-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
            if(plansza->pole[x][y-i].ileRyb==0 || plansza->pole[x][y-i].nrGracza!=0 || y-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
        }
        plansza->pole[x][t].nrGracza=TEAM_NR;
        plansza->pole[x][y].nrGracza=0;
        plansza->nasza_ilosc_ryb+= plansza->pole[x][t].ileRyb;
        plansza->pole[x][t].ileRyb=0;
        return 0;
    }
    else if(x==z && y<=t)
    {
        for(int i=1; i<=t-y; i++)
        {
            if(y+i>=plansza->n_cols-1)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
            if(plansza->pole[x][y+i].ileRyb==0 || plansza->pole[x][y+i].nrGracza!=0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
        }
        plansza->pole[x][t].nrGracza=TEAM_NR;
        plansza->pole[x][y].nrGracza=0;
        plansza->nasza_ilosc_ryb+= plansza->pole[x][t].ileRyb;
        plansza->pole[x][t].ileRyb=0;
        return 0;
    }
    else if(z<=x && y==t)
    {
        for(int i=1; i<=x-z; i++)
        {
            if(x-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
            if(plansza->pole[x-i][y].ileRyb==0 || plansza->pole[x-i][y].nrGracza!=0 || x-i<0)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
        }
        plansza->pole[z][y].nrGracza=TEAM_NR;
        plansza->pole[x][y].nrGracza=0;
        plansza->nasza_ilosc_ryb+= plansza->pole[z][y].ileRyb;
        plansza->pole[z][y].ileRyb=0;
        return 0;
    }
    else if(z>=x && y==t)
    {
        for(int i=1; i<=z-x; i++)
        {
            if(x+i>plansza->n_rows)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
            if(plansza->pole[x+i][y].ileRyb==0 || plansza->pole[x+i][y].nrGracza!=0 || x+i>plansza->n_rows)
            {
                printf("Nie mozesz przejsc na to pole! Rusz sie inaczej.\n");
                return 1;
            }
        }
        plansza->pole[z][y].nrGracza=TEAM_NR;
        plansza->pole[x][y].nrGracza=0;
        plansza->nasza_ilosc_ryb+= plansza->pole[z][y].ileRyb;
        plansza->pole[z][y].ileRyb=0;
        return 0;
    }
    return 1;
}
int znajdz_nasze_pingwiny(Plansza *plansza, GameParameters params)
{
    printf("znajdywanie naszych pingwinow: ...\t");
    assert(params.phase==movement && plansza != NULL);
    
    int i=0;
    plansza->nasze_pingwiny = malloc(sizeof(Wspolrzedne_pola) *params.penguins);
    for(int r=0; r<plansza->n_rows; r++)
    {
        for(int c=0; c<plansza->n_cols; c++)
        {
            if(plansza->pole[r][c].nrGracza == plansza->nasz_nr)
            {
                plansza->nasze_pingwiny[i].row = r;
                plansza->nasze_pingwiny[i++].column = c;
            }
        }
    }
    assert(i == params.penguins);
    printf("ok\n");
    return 0;
}

void exit_program(int exit_code)
{
    if(exit_code == 3)
    {
        printf("blad wewnetrzny programu\n");
    }
    else if(exit_code == 2)
    {
        printf("blad danych wejsciowych\n");
    }
    else if(exit_code == 1)
    {
        printf("nie moge wykonac ruchu, ale wszystko jest OK\n");
    }
    else if(exit_code == 0)
    {
        printf("wszystko ok\n");
    }
    else
    {
        printf("nieznany kod\n");
    }
    
}


int main(int argc, char **argv)
{
    GameParameters params;
    Plansza plansza;
    int exit_code = 0;
    if(get_params(argc, argv, &params) != 0)
        return 3;
    FILE *fp_in = fopen(params.inputboardfile, "rt");
    if(!fp_in)
    {
        printf("\nerror while opening a file\n");
        return 2; 
    }
        
    switch (params.phase)
    {
        case placement:
        {
            exit_code = read_file(fp_in, &plansza, &params);
            if(exit_code != 0)
            {
                exit_program(exit_code);
                clear_memory(&plansza);
                fclose(fp_in);
                return exit_code;
            }
            assert(plansza.pole != NULL);
            print_game_info(plansza, params);
            srand(time(NULL));
            int x;
            int y;
            for(exit_code=2; exit_code != 0;)
            {
                x = rand()%plansza.n_rows;
                y = rand()%plansza.n_cols;
                exit_code = ustaw_pingwina(&plansza, params, x, y);
                if(exit_code == 1)
                    break;
                else if(exit_code==0)
                    printf("pingwin ustawiony\n");
            }
          //  print_game_info(plansza, params);
            //
        } break;

        case movement:
        {
            exit_code = read_file(fp_in, &plansza, &params);
            if(exit_code != 0)
            {
                exit_program(exit_code);
                clear_memory(&plansza);
                fclose(fp_in);
                return exit_code;
            }
            znajdz_nasze_pingwiny(&plansza, params);
            for(int i=0; i<params.penguins; i++)
                printf("(x,y) : (%d,%d)\n", \
                plansza.nasze_pingwiny[i].row, plansza.nasze_pingwiny[i].column);
            int x, y, t;
            t = rand()%params.penguins; //losuj nr naszego pingwina
            Wspolrzedne_pola nasz_pingwin = plansza.nasze_pingwiny[t];
            Wspolrzedne_pola ruch;
            for(exit_code=1; exit_code != 0;)
            {
                x = rand()%plansza.n_rows;
                y = rand()%plansza.n_cols;
                ruch.row = x;
                ruch.column = y;
                exit_code = move_penguins(&plansza, params, nasz_pingwin, ruch);
                if(exit_code==0)
                    printf("pingwin ruszony\n");
            }
        } break;

    }
    FILE *fp_out = fopen(params.outputboardfile, "wt");
    save_to_file(fp_in, fp_out, &plansza, params);
    fclose(fp_in);
    fclose(fp_out);
    clear_memory(&plansza);
    printf("\n\texit_code: %d\n", exit_code);
    return exit_code;
}
