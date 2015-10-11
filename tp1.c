#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//définition primitive des structures de gestion de nombres
struct num { 
	int positif; 
	struct cell *nombre;
	struct cell *dernier;
};
struct cell {
	int chiffre; 
	struct cell *suivant;
	struct cell *precedent;
};

typedef struct num num; //permet d'utiliser la structure num comme un type
typedef struct cell cell;

int numCreator(char*, num*);

char* entreeDynamique(FILE*);

void printNum(num*);
void freeNumber(num*);

//définition des opérateurs
num soustraction(num, num);
num* addition(num*, num*);
num multiplication(num, num);

int main()
{
	char *line;
		// TESTS
		num *a = malloc(sizeof(num));
		num *b = malloc(sizeof(num));
		numCreator("7021",a);
		numCreator("2973",b);
		printNum(a);
		printNum(b);
		num *r = addition(a,b);

		printNum(r);
		//FIN TESTS

		// printf(">");
		// line = entreeDynamique(stdin);
		// if(line == NULL)
		// {
		// 	printf("memoire epuisee");
		// 	continue;
		// }

		// char varName = '\0';
		// int i;
		// for(i = 0; i < strlen(line); i++)
		// {
		// 	if(line[i] == '=')
		// 	{
		// 		//il n'y a pas d'espace entre le = et le nom de la variable
		// 		varName = line[i+1];
		// 		break;
		// 	}
		// }

		// if(varName != '\0')
		// {

		// }
		// printf("%s\n",line); //test
	
}

num* addition(num *a, num *b)
{
	cell *cA = a->nombre;
	cell *cB = b->nombre;
	cell *result = malloc(sizeof(cell));
	num* r = malloc(sizeof(num));

	if(result == NULL || r == NULL)
	{
		printf("memoire epuisee\n");
		return NULL;
	}

	int fini = 0;
	int intermediaire = 0;
	int carry;

	cell *newUnit;
	do
	{
		intermediaire = cA->chiffre + cB->chiffre;

		result->chiffre = intermediaire;

		newUnit = malloc(sizeof(cell));

		if(result == NULL || r == NULL)
		{
			printf("memoire epuisee\n");
			return NULL;
		}

		newUnit->precedent = result;

		result->suivant = newUnit;
		result = newUnit;

		cA = cA->suivant;
		cB = cB->suivant;

		fini = cA == NULL && cB == NULL;
	}
	while(!fini);

	newUnit->suivant = NULL;
	r->positif = 1;
	r->dernier = newUnit;

	return r;
}
num soustraction(num a, num b)
{
	//utiliser la méthode des compléments?
	num result;
	return result;
}
num multiplication(num a, num b)
{
	num result;
	return result;
}

//transform a string number to a linked list of infinite precision. 
//retourne -1 si il n'y a pas assez de mémoire pour stocker le nombre
int numCreator(char *str, num *toCreate)
{
	int i;
	cell **c = &toCreate->nombre;
	cell *precedent = NULL;
	toCreate->positif=1;

	//on stocke les nombres en little endian pour faciliter les calculs
	for(i = strlen(str)-1; i >= 0; i--)
	{
		*c = malloc(sizeof(cell));

		if(*c == NULL)
		{
			printf("memoire epuisee\n");
			return -1;
		}

		(*c)->precedent = precedent;
		precedent = *c;

		(*c)->chiffre = str[i] - '0'; //transform 'char' to int ie. '5' to 5. fonctionne car on a seulement des nombres 0-9
		
		if(i == 0)
			toCreate->dernier = *c; //on garde une référence sur le dernier element pour imprimer efficacement le nombre

		c = &(*c)->suivant;
	}
	*c = NULL;
	return 0;
}

void printNum(num *toPrint)
{
	if(!toPrint->positif) //nombre negatif
		printf("-");

	cell *nombre = toPrint->dernier;

	while(nombre->chiffre == 0)
		nombre = nombre->precedent;
	
	//on peut afficher les nombres dans l'ordre 
	while(nombre->precedent != NULL)
	{
		printf("%d",nombre->chiffre);
		nombre = nombre->precedent;
	}
	printf("%d",nombre->chiffre); //on affiche le dernier chiffre
	printf("\n");
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