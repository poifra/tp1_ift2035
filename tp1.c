#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//définition primitive des structures de gestion de nombres
struct num { int positif; struct cell *chiffres; };
struct cell { int chiffre; struct cell *suivant; };

typedef struct num num; //permet d'utiliser la structure num comme un type
typedef struct cell cell;

typedef int bool; //permet d'utiliser des booleens directement
#define true 1
#define false 0

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

		char varName = NULL;
		int i;
		for(i = 0; i < strlen(line); i++)
		{
			if(line[i] == '=')
			{
				//il n'y a pas d'espace entre le = et le nom de la variable
				varName = line[i+1];
				break;
			}
		}

		if(varName != NULL)
		{

		}

		printf("%s\n",line); //test
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

void printNum(num toPrint)
{
	if(toPrint.positif == 0) //nombre negatif
		printf("-");
	cell *chiffres = toPrint.chiffres;
	while(chiffres != NULL)
	{
		print("%d",chiffres->chiffre);
		chiffres = chiffres->suivant;
	}
}

num addition(num a, num b)
{
	bool calculFini = false;
	int carry = 0;
	cell *chiffresA = a.chiffres;
	cell *chiffresB = a.chiffres;
	cell *chiffresResult;
	int intermediaire = 0;
	num result;
	result.chiffres = chiffresResult;
	do
	{
		calculFini = chiffresA->suivant == NULL && chiffresB->suivant == NULL;
		
		intermediaire = chiffresA->chiffre + chiffresB->chiffre + carry;
		
		if(intermediaire > 9) //retenue
		{
			carry = 1;
			intermediaire -= 10;
		}
		chiffresResult->chiffre = intermediaire;
		chiffresResult->suivant = chiffresResult;
	}
	while(!calculFini);

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