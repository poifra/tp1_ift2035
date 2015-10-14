#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//définition primitive des structures de gestion de nombres
struct num { 
	int positif; 
	int longueur;
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

//fonction ménage
void superFree(num*);
void recursiveSuperFree(cell*);

int main()
{
	char *line;
		// TESTS
		num *a = malloc(sizeof(num));
		num *b = malloc(sizeof(num));
		numCreator("2454456456451849542040640624096504621213490855",a);
		numCreator("2544564564518495420999544564564518495420",b);
		num *r = addition(a,b);

		printNum(r);
		superFree(a);
		superFree(b);
		superFree(r);
	//	FIN TESTS
		// while(1)
		// {
		// 	printf(">");
		// 	line = entreeDynamique(stdin);
		// 	if(line == NULL)
		// 	{
		// 		printf("memoire epuisee");
		// 		continue;
		// 	}

		// 	char varName = '\0';
		// 	int i;
		// 	for(i = 0; i < strlen(line); i++)
		// 	{
		// 		if(line[i] == '=')
		// 		{
		// 			//il n'y a pas d'espace entre le = et le nom de la variable
		// 			varName = line[i+1];
		// 			break;
		// 		}
		// 	}

		// 	if(varName != '\0')
		// 	{

		// 	}
		// 	printf("%s\n",line); //test

		// 	free(line);
		// }
	
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

	if(a->longueur < b->longueur)
	{
		printf("inverse\n");
		num *temp = a;
		a = b;
		b = temp;
		cA = a->nombre;
		cB = b->nombre;
	}

	cell* dernier = b->dernier;
	cell* newZero = NULL;
	while(b->longueur < a->longueur) //ajustement de la longueur du plus petit nombre
	{
		newZero = malloc(sizeof(cell));
		if(newZero == NULL)
		{
			printf("memoire epuisee\n");
			return NULL;
		}
		newZero->precedent = dernier;
		dernier->suivant = newZero;
		dernier = newZero;
		b->longueur++;
	}

	int fini = 0;
	int intermediaire = 0;
	int carry = 0;


	cell *newUnit = NULL;
	do
	{
		fini = cA->suivant == NULL && cB->suivant == NULL;
		intermediaire = cA->chiffre + cB->chiffre + carry;
		if(intermediaire > 9)
		{
			carry = 1;
			intermediaire -= 10;
		}
		else
			carry = 0;

		result->chiffre = intermediaire;

		if(!fini)
			newUnit = malloc(sizeof(cell));

		if(result == NULL || newUnit == NULL)
		{
			printf("memoire epuisee\n");
			return NULL;
		}


		if(!fini)
		{
			newUnit->precedent = result;

			result->suivant = newUnit;
			result = newUnit;
		}
		cA = cA->suivant;
		cB = cB->suivant;

	}
	while(!fini);

	if(carry)
	{
		cell* carryCell = malloc(sizeof(cell));
		carryCell->chiffre = carry;
		carryCell->precedent = result;
		carryCell->suivant = NULL;
		r->dernier = carryCell;
		r->positif = 1;
	}
	else
	{
		newUnit->suivant = NULL;
		r->positif = 1;
		r->dernier = newUnit;
	}

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

void superFree(num* toFree)
{
	cell *c = toFree->nombre;
	if(c != NULL)
		recursiveSuperFree(c);
	free(toFree);
}

void recursiveSuperFree(cell* c)
{
	if(c->suivant == NULL)
		free(c);
	else
		recursiveSuperFree(c->suivant);
}


//transform a string number to a linked list of infinite precision. 
//retourne -1 si il n'y a pas assez de mémoire pour stocker le nombre
int numCreator(char *str, num *toCreate)
{
	int i;
	cell **c = &toCreate->nombre;
	cell *precedent = NULL;
	toCreate->positif=1;
	toCreate->longueur = 0;

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
		toCreate->longueur++;
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

	int position = toPrint->longueur; //cas particulier si le nombre est 0.
	while(nombre->chiffre == 0 && position != 1)
	{
		nombre = nombre->precedent;
		position--;
	}

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