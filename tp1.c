#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

//définition primitive des structures de gestion de nombres
struct num { 
	int positif; 
	struct cell *nombre; 
};
struct cell {
	int chiffre; 
	struct cell *suivant; 
};

typedef struct num num; //permet d'utiliser la structure num comme un type
typedef struct cell cell;

typedef int bool; //permet d'utiliser des booleens directement
#define true 1
#define false 0

char* entreeDynamique(FILE*);

void printNum(num);

//définition des opérateurs
num soustraction(num, num);
num addition(num, num);
num multiplication(num, num);

int main()
{
	char *line;
	while(1)
	{
		// TESTS

		num a;
		num b;
		cell *c = malloc(sizeof(cell));
		cell *c2 = malloc(sizeof(cell));

		c->chiffre = 23;
		c->suivant = NULL;
		c2->chiffre = 23;
		c2->suivant = NULL;

		a.nombre = c;
		b.nombre = c2;
		num r = addition(a,b);
		printNum(r);

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

num addition(num a, num b)
{
	bool calculFini = false;

	cell *nombreA = a.nombre;
	cell *nombreB = b.nombre;

	num result;
	cell *nombreResult = malloc(sizeof(cell));
	result.nombre = nombreResult; //pointeur vers la premiere case de la liste

	int intermediaire = 0;
	int carry = 0;

	do
	{
		
		calculFini = nombreA->suivant == NULL && nombreB->suivant == NULL;

		intermediaire = nombreA->chiffre + nombreB->chiffre + carry;
		if(intermediaire > 9)
		{
			carry = 1;
			intermediaire -= 10;
		}

		nombreResult->chiffre = intermediaire;

	}
	while(!calculFini);

	nombreResult->suivant = newUnit; //termine la liste chainée
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

void printNum(num toPrint)
{
	if(toPrint.positif == 0) //nombre negatif
		printf("-");
	cell *nombre = toPrint.nombre;
	while(nombre != NULL)
	{
		printf("%d\n",nombre->chiffre);
		nombre = nombre->suivant;
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

