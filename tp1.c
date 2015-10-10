#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int numCreator(char*, num*);

char* entreeDynamique(FILE*);

void printNum(num*);

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
		numCreator("32",a);
		numCreator("12",b);
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
	num *result = malloc(sizeof(num));
	result->positif = 1;

	cell *cA = a->nombre;
	cell *cB = b->nombre;
	cell *resultat = malloc(sizeof(cell)); //TODO : memory check
	resultat->suivant = NULL; //le digit le moins significatif a rien apres
	int calculFini = 0;
	int carry = 0;
	cell *newUnit;
	do
	{
		calculFini = cA->suivant == NULL && cB->suivant == NULL;
		int intermediaire = cA->chiffre + cB->chiffre + carry;
		if(intermediaire > 9)
		{
			carry = 1;
			intermediaire -= 10;
		}
		else
			carry = 0;

		resultat->chiffre = intermediaire;
		cA = cA->suivant;
		cB = cB->suivant;
		newUnit = malloc(sizeof(cell));
		newUnit->suivant = resultat;
	}
	while(!calculFini);	

	if(carry) // on vérifie le digit le plus significatif
		newUnit->chiffre++;

	if(newUnit->chiffre > 9) //carry dans le chiffre le plus significatif
	{
		newUnit->chiffre -= 10;
		cell *mostSignifiant = malloc(sizeof(cell));
		mostSignifiant->suivant = newUnit;
		mostSignifiant->chiffre = carry;
		result->nombre = mostSignifiant;

	}
	else
		result->nombre = newUnit;

	return result;
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

		(*c)->chiffre = str[i] - '0'; //transform 'char' to int ie. '5' to 5. fonctionne car on a seulement des nombres 0-9
		c = &(*c)->suivant;
		
	}
	*c = NULL;
	return 0;
}

void printNum(num *toPrint)
{
	if(!toPrint->positif) //nombre negatif
		printf("-");

	cell *nombre = toPrint->nombre;
	while(nombre != NULL)
	{
		printf("%d",nombre->chiffre);
		nombre = nombre->suivant;
	}
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