#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//définition primitive des structures de gestion de nombres
struct num { int positif; struct cell *chiffres; };
struct cell { int chiffre; struct cell *suivant; };


char* entreeDynamique(FILE*);

//définition des opérateurs
int soustraction(int, int);
int addition(int, int);
int multiplication(int, int);

int main()
{
	char *line;
	while(1)
	{
		printf(">");
		line = entreeDynamique(stdin);
		if(line == NULL)
		{
			printf("memoire epuisee");
			continue;
		}
		printf("%s\n",line);
	}
}

//lit une ligne d'une longueur arbitraire et la retourne
char* entreeDynamique(FILE* input)
{
	int defaut = 32;//32 bytes par defaut;
	char* mot = malloc(sizeof(char)*defaut);
	int taille=0;
	char c;

	if(mot ==NULL)
	return NULL;
	
	while(1)
	{
		c = fgetc(input);
		if(c == EOF || c == '\n')
			break;

		mot[taille] = c;
		taille++;

		//realloc si la ligne est plus grande que l'espace alloué
		if(taille >= defaut)
		{
			defaut+=32;
			mot = realloc(mot, sizeof(char)*defaut);
			if(mot == NULL)
				return NULL;
		}
	}
	mot[taille]='\0'; //terminer la string
	taille++;
	mot = realloc(mot, sizeof(char)*taille);
	return mot;
}

int soustraction(int a, int b)
{
	return 0;
}
int addition(int a, int b)
{
	return 0;
}
int multiplication(int a, int b)
{
	return 0;
}