#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug"

//définition primitive des structures de gestion de nombres
struct num { int positif; struct cell *chiffres; };
struct cell { int chiffre; struct cell *suivant; };

typedef struct num num; //permet d'utiliser la structure num comme un type

char* entreeDynamique(FILE*);

//définition des opérateurs
num soustraction(num, num);
num addition(num, num);
num multiplication(num, num);

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
//retourne NULL si il n'y a pas assez de mémoire
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
	if(taille > defaut)
	{
		taille++;
		mot = realloc(mot, sizeof(char)*taille);
	}
	mot[taille]='\0'; //terminer la string

	return mot;
}

num addition(num a, num b)
{
	num result;
	return result;
}
num soustraction(num a, num b)
{
	num result;
	return result;
}
num multiplication(num a, num b)
{
	num result;
	return result;
}