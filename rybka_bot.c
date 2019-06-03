#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "templates.h"

void print_game_info(Plansza plansza, GameParameters params)
{
    printf("faza gry: %s\nn_pingwinow: %d\ninputFile: %s\noutputFile: %s\n", \
        params.phase ? "movement":"placement", params.penguins, params.inputboardfile, params.outputboardfile);
    printf("zebrane ryby: %d\n", plansza.ilosc_ryb);
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

int read_file(FILE *fp_in, Plansza *plansza, GameParameters params)
{
    int n_rows, n_cols;
    fscanf(fp_in,"%d", &n_rows);
    fscanf(fp_in,"%d", &n_cols);

    plansza->n_rows = n_rows;
    plansza->n_cols = n_cols;
    plansza->pole = malloc(n_rows*sizeof(POLE *));
    for(int i=0; i<n_rows; i++)
    {
        plansza->pole[i]=malloc(n_cols*sizeof(POLE));
    }
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
    int myidx = 0;
    int i=0;
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
    if(params.phase == movement)
    {
        plansza->n_players = i;
        plansza->ilosc_ryb = plansza->players_stats[myidx].n_ryb;
    }
    return 0;
}
int save_to_file(FILE *fp_in, FILE *fp_out, Plansza *plansza, GameParameters params)
{
    fprintf(fp_out,"%d %d\n",plansza->n_rows, plansza->n_cols);
    for(int i=0; i<plansza->n_rows; i++)
    {
        for(int j=0; j<plansza->n_cols; j++)
        {
            fprintf(fp_out,"%d%d ",plansza->pole[i][j].ileRyb, plansza->pole[i][j].nrGracza);
        }
        fprintf(fp_out,"\n");
    }
    for(int i=0; i<plansza->n_players; i++)
    {
        char *name = plansza->players_stats[i].name;
        int n_ryb = plansza->players_stats[i].n_ryb;
        int nr_gracza = plansza->players_stats[i].nr_gracza;
        fprintf(fp_out, "%s %d %d\n", name, nr_gracza, n_ryb);
    }
    return 0;
}
int split(char *arg, char *value)
{
    if(!arg)
        return -1;

    char ch;
    int len = 0;
    int loc = 0;
    int max_len = strlen(arg);
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
    if(params.penguins <= plansza->ilosc_ryb)
    {
        printf("nie zgadza sie liczba ryb do liczby pingwinow przy rozstawianiu!\n");
        return 2;
    }
    if(plansza->pole[x][y].nrGracza!=0 || plansza->pole[x][y].ileRyb!=1)
        {
            printf("Na tym polu nie mozna ustawic pingwina!\n");
            return 1;
        }
        else
        {
            plansza->pole[x][y].nrGracza=TEAM_NR;
            plansza->ilosc_ryb += plansza->pole[x][y].ileRyb;
            plansza->pole[x][y].ileRyb=0;
        }
        return 0;
}


int main(int argc, char **argv)
{
    GameParameters params;
    Plansza plansza;
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
            int read_result = read_file(fp_in, &plansza, params);
            if(read_result != 0)
                return read_result;
            if(plansza.pole == NULL)
                printf("pole = NULL\n");
            print_game_info(plansza, params);
            srand(time(NULL));
            int x;
            int y;
            int result;
            do
            {
                x = rand()%plansza.n_rows;
                y = rand()%plansza.n_cols;
                result = ustaw_pingwina(&plansza, params, x, y);
                if(result == 2)
                    break;
            } while (result != 0);
            //
        } break;

        case movement:
        {
            // wybierz pingwina, jeśli nie mozesz się ruszyć return 1
            //RuszPingwina
        } break;

    }
    FILE *fp_out = fopen(params.outputboardfile, "wt");
    save_to_file(fp_in, fp_out, &plansza, params);
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}
