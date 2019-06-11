#include "templates.h"

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
}
