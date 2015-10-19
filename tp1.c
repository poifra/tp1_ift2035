#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//parce que c'est penible le copy/paste
#define memcheck(x) if(x == NULL) {\
					printf("memoire epuisee \n");\
					return NULL; }

//définition primitive des structures de gestion de nombres
struct num { 
	int positif; 
	int longueur;
	int nbRef;
	struct cell *nombre;
	struct cell *dernier;
};

struct var {
	char symbole;
	struct num valeur;
};

struct cell {
	int chiffre; 
	struct cell *suivant;
	struct cell *precedent;
};

typedef struct num num; //permet d'utiliser la structure num comme un type
typedef struct cell cell;

int strToBigNum(char*, num*);

char* entreeDynamique(FILE*);

void printNum(num*);
void* setupNombres(num*, num*);

//définition des opérateurs
num* soustraction(num*, num*);
num* addition(num*, num*);
num* multiplication(num*, num*);

//fonction ménage
void superFree(num*);
void recursiveSuperFree(cell*);

int main()
{
	char *line;
		// TESTS
	num *a = malloc(sizeof(num));
	num *b = malloc(sizeof(num));
	num *c = malloc(sizeof(num));

	strToBigNum(entreeDynamique(stdin),a);
	strToBigNum(entreeDynamique(stdin),b);
	//strToBigNum(entreeDynamique(stdin),c);
	num *r = multiplication(a,b);

	if(r != NULL)
		printNum(r);
	else
		printf("mauvais calcul :(");

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
	setupNombres(a,b);

	cell *cA = a->nombre;
	cell *cB = b->nombre;
	printNum(a);
	printNum(b);

	if(!cA)
	{
		printf("erreur nombre a");
		return NULL;
	}
	if(!cB)
	{
		printf("erreur nombre b");
		return NULL;
	}

	cell *result = malloc(sizeof(cell));

	num* r = malloc(sizeof(num));

	memcheck(r)
	memcheck(result)

	r->longueur = 0;

	int fini = 0;
	int intermediaire = 0;
	int carry = 0;

	cell *newUnit = NULL;
	r->nombre = result;
	while(!fini)
	{
		fini = cA->suivant == NULL && cB->suivant == NULL;

		if( cA->suivant != NULL && cB->suivant == NULL)
		{
			printf("a pas null b null\n");
			printNum(b);
		}
		if( cA->suivant == NULL && cB->suivant != NULL)
		{
			printf("a null b pas null\n");
			printNum(a);
		}

		intermediaire = cA->chiffre + cB->chiffre + carry;

		if(intermediaire > 9)
		{
			carry = 1;
			intermediaire -= 10;
		}
		else
			carry = 0;

		result->chiffre = intermediaire;
		printf("int= %d\n", result->chiffre);
		if(!fini)
		{
			//ajustement des pointeurs pour le prochain round si pertinent
			newUnit = malloc(sizeof(cell));
			memcheck(newUnit)

			newUnit->precedent = result;

			result->suivant = newUnit;
			result = newUnit;
		}

		cA = cA->suivant;
		cB = cB->suivant;
		r->longueur++;
	}
	
	if(carry)
	{
		cell* carryCell = malloc(sizeof(cell));

		memcheck(carryCell)

		carryCell->chiffre = carry;
		carryCell->precedent = result;
		carryCell->suivant = NULL;
		r->dernier = carryCell;
		r->longueur++;
	}
	else
	{
		if(newUnit == NULL) //cas particulier ou on a pas de carry et 1 seul digit dans chaque operande ex : 2+2
		{
			result->suivant = NULL;
			r->dernier = result;
		}
		else
		{
			newUnit->suivant = NULL;
			r->dernier = newUnit;
		}
	}
	r->positif = 1;
	return r;
}
num* soustraction(num *a, num *b)
{

	cell *result = malloc(sizeof(cell));
	cell *newUnit = NULL;
	cell *cA = NULL;
	cell *cB = NULL;

	int fini = 0;
	int retenue = 0;
	int intermediaire = 0;

	num* r = malloc(sizeof(num));

	memcheck(result)
	memcheck(r)

	r->longueur = 0;

	if(numComparator(a,b) == -1)
	{
		num *temp = a;
		a = b;
		b = temp;
		r->positif = 0;
	}
	else
		r->positif = 1;

	setupNombres(a,b);
	cA = a->nombre;
	cB = b->nombre;
	r->nombre = result;
	do
	{
		fini = cA->suivant == NULL && cB->suivant == NULL;
		intermediaire = cA->chiffre - cB->chiffre;

		if(intermediaire < 0)
		{
			cA->chiffre += 10;
			if(cA->suivant != NULL)
				cA->suivant->chiffre--;

			intermediaire = cA->chiffre - cB->chiffre;
		}
		result->chiffre = intermediaire;

		if(!fini)
		{
			//ajustement des pointeurs pour le prochain round si pertinent
			newUnit = malloc(sizeof(cell));
			memcheck(newUnit)

			newUnit->precedent = result;

			result->suivant = newUnit;
			result = newUnit;
		}
		r->longueur++;
		cA = cA->suivant;
		cB = cB->suivant;
	}
	while(!fini);

	if(newUnit == NULL)
	{
		result->suivant = NULL;
		r->dernier = result;
	}
	else
	{
		newUnit->suivant = NULL;
		r->dernier = newUnit;
	}
	return r;
}
num* multiplication(num *a, num *b)
{
	cell *cA = a->nombre;
	cell *cB = b->nombre;
	cell *result = NULL;

	num *listeToAdd = malloc(b->longueur * sizeof(num));
	memcheck(listeToAdd)

	num *r = malloc(sizeof(num));

	memcheck(r)

	r->longueur = 0;

	int finiCB = 0;
	int finiCA = 0;
	int intermediaire = 0;
	int carry = 0;
	int chiffreCourant = 0;
	int decalageUnit = 0;

	cell *newUnit = NULL;
	do
	{	
		chiffreCourant = cB->chiffre;
		cA = a->nombre;
		result = malloc(sizeof(cell));
		memcheck(result);
		result->suivant = NULL;
		result->precedent = NULL;
		r->nombre = result;
		r->longueur = 0;
		r->positif = 1;
		carry = 0;

		do
		{ //multiplication en tant que tel
			finiCA = cA->suivant == NULL;
			intermediaire = chiffreCourant * cA->chiffre + carry;
			carry = 0;
			while(intermediaire > 9)
			{
				carry++;
				intermediaire -= 10;
			}

			result->chiffre = intermediaire;

			if(!finiCA)
			{ 
				//ajustement des pointeurs pour le prochain round si pertinent
				newUnit = malloc(sizeof(cell));

				memcheck(newUnit)
				newUnit->precedent = result;

				result->suivant = newUnit;
				result = newUnit;
			}
			cA = cA->suivant;
			r->longueur++;
		}
		while(!finiCA);

	
		if(newUnit == NULL) //cas particulier 1 seul digit dans chaque operande ex : 2*2
		{
			result->suivant = NULL;
			r->dernier = result;
		}
		else
		{
			newUnit->suivant = NULL;
			r->dernier = newUnit;
		}
		//ajuster le carry si il faut
		if(carry)
		{
			cell* carryCell = malloc(sizeof(cell));

			memcheck(carryCell)

			carryCell->chiffre = carry;
			carryCell->precedent = result;
			result->suivant = carryCell;
			carryCell->suivant = NULL;
			r->dernier = carryCell;
			r->longueur++;
		}

		// ajuster les 0
		cell *newZero = NULL;
		cell *currentZero = NULL;
		cell *premierZero = NULL;
		int i = 0;
		if(decalageUnit > 0)
		{
			currentZero = malloc(sizeof(cell));
			memcheck(currentZero)
			currentZero->chiffre = 0;
			currentZero->precedent = NULL;
			premierZero = currentZero;

			for(i = 1; i < decalageUnit; i++)
			{
				newZero = malloc(sizeof(cell));
				memcheck(newZero);
				newZero->precedent = currentZero;
				newZero->chiffre = 0;
				currentZero->suivant = newZero;
				currentZero = newZero;
			}

			r->nombre->precedent = currentZero;
			currentZero->suivant = r->nombre;
			r->nombre = premierZero;

			r->longueur += i;
		}
		//fin ajustement 0

		listeToAdd[decalageUnit] = *r;
		cB = cB->suivant;
		finiCB = cB == NULL;
		decalageUnit++;
	}
	while(!finiCB);
	int j;
	num* somme = malloc(sizeof(num));
	memcheck(somme)
	strToBigNum("0",somme);

	for(j = 0; j < b->longueur; j++)
	{
		somme = addition(somme,&listeToAdd[j]);
	}
	
	free(listeToAdd);
	return somme;
}

void superFree(num* toFree)
{
	cell *c = toFree->nombre;
	if(c != NULL)
		recursiveSuperFree(c);
	free(toFree);
	toFree = NULL;
}

void recursiveSuperFree(cell* c)
{
	if(c->suivant == NULL)
	{
		free(c);
		c = NULL;
	}
	else
		recursiveSuperFree(c->suivant);
}

//valide les nombres entrés et ajuste leur longueur, au besoin
void* setupNombres(num *a, num *b)
{

	if(a->longueur < b->longueur)
	{
		num *temp = a;
		a = b;
		b = temp;
	}


	cell* dernier = b->dernier;
	cell* newZero = NULL;
	while(b->longueur < a->longueur) //ajustement de la longueur du plus petit nombre
	{
		newZero = malloc(sizeof(cell));
		memcheck(newZero)
		newZero->chiffre = 0;
		newZero->precedent = dernier;
		dernier->suivant = newZero;
		dernier = newZero;
		b->dernier = dernier;
		b->longueur++;
	}
	dernier->suivant = NULL;
	
}

//transform a string number to a linked list of infinite precision. 
//retourne -1 si il n'y a pas assez de mémoire pour stocker le nombre
int strToBigNum(char *str, num *toCreate)
{
	int i;
	cell **c = &toCreate->nombre;
	cell *precedent = NULL;
	int positif = str[0] != '-';
	toCreate->positif= positif;
	toCreate->longueur = 0;

	//on stocke les nombres en little endian pour faciliter les calculs
	//si le nombre est positif, 1-positif = 0
	for(i = strlen(str)-1; i >= 1 - positif; i--)
	{
		*c = malloc(sizeof(cell));

		if(*c == NULL)
		{
			printf("memoire epuisee \n");
			superFree(toCreate);
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

/*
compare les quantités de chaque nombre 
retourne 0 si les nombres sont égaux,
retourne 1 si a > b.
retourne -1 si a < b.
*/
int numComparator(num *a, num *b)
{
	if(a->positif > b->positif)
		return 1;
	if(a->positif < b->positif)
		return -1;

	//les nombres ont le meme signe
	if(a->longueur > b->longueur)
		return a->positif ? 1 : -1;
	if(a->longueur < b->longueur)
		return a->positif ? -1 : 1;

	if(a->longueur == 1 && b->longueur == 1)
	{
		return (a->dernier->chiffre) > (b->dernier->chiffre) ? 1 : -1;
	}
	//les deux nombres on la meme longueur > 1 et le meme signe, on compare chaque
	//en commencant par la plus singificative.
	cell *cA = a->dernier;
	cell *cB = b->dernier;

	while(cA->precedent != NULL)
	{
		if(cA->chiffre > cB->chiffre)
			return a->positif ? 1 : -1;
		if(cA->chiffre < cB->chiffre)
			 return a->positif ? -1 : 1;

		cA = cA->precedent;
		cB = cB->precedent;
	}

	return 0;
}

void printNum(num *toPrint)
{
	if(!toPrint->positif) //nombre negatif
		printf("-");

	cell *nombre = toPrint->dernier;

/*	int position = toPrint->longueur; //cas particulier si le nombre est 0.
	while(nombre->chiffre == 0 && position != 1)
	{
		nombre = nombre->precedent;
		position--;
	}*/

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

	memcheck(mot)
	
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
			memcheck(mot)
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