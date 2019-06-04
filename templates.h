#define INTERAKTYWNA 1
#define TEAM_NR 1
#define TEAM_NAME "marcel"

typedef struct {
    int ileRyb;
    int nrGracza;
} POLE;


typedef enum p{
    placement = 0, movement = 1
} phase_type;

typedef struct {
  char name[20];
  int nr_gracza;
  int n_ryb;
}players_stats_type;

typedef struct game_params {
    phase_type phase;
    int penguins;
    char inputboardfile[40];
    char outputboardfile[40];
}GameParameters;

typedef struct {
    POLE **pole;
    int n_rows;
    int n_cols;
    int nasza_ilosc_ryb;
    int n_players;
    players_stats_type players_stats[9];
} Plansza;

