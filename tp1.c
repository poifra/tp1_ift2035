/**
 * IFT2035 - Travail Pratique 1
 *
 * Par : Sulliman Aïad <sulliman.aiad@umontreal.ca>
 *       François Poitras <francois.poitras@umontreal.ca>
 */
//#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pour les tests.
#define memcheck(x) if(x == NULL) {\
						printf("Mémoire epuisée.\n");\
						return NULL;\
					}

struct num { 
	int positif; 
	int longueur;
	int nbRef;
	struct cell* nombre;
	struct cell* dernier;
};

struct var {
	char symbole;
	struct num valeur;
};

struct cell {
	int chiffre; 
	struct cell* suivant;
	struct cell* precedent;
};

typedef struct num num;
typedef struct cell cell;

// Inspiré de "http://groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/stack1.html".
struct pile {
	num** data;
	
	int size;
	int max_size;
};

typedef struct pile pile;

// Ensemble de variables: tableau de 26 références à des nombres à précision infinie.
num* variables[26] = {};

int strToBigNum(char*, num*);

char* entreeDynamique(FILE*);

// Prototypes.
void printNum(num*);
void* setupNombres(num*, num*);
void pile_init(pile* p);
num* pile_peek(pile* p);
void pile_push(pile* p, num* n);
num* pile_pop(pile* p);
int pile_count(pile* p);

// Définition des opérateurs.
num* soustraction(num*, num*, int, int);
num* addition(num*, num*, int, int);
num* multiplication(num*, num*);

// Fonctions de ménage.
void superFree(num*);
void recursiveSuperFree(cell*);

/**
 * Entrée d'une commande (STDIN) et retour d'une réponse appropriée (STDOUT).
 * Retourne -1 lors d'une demande de quitter le programme, ou 0 si on continue.
 *
 * Invite l'utilisateur à entrer des commandes au format postfixe (ex: 3 2 + 5 * =a).
 */
int traitement_commande() {
	printf("> ");
	
	char* entree = entreeDynamique(stdin);
	
	if(entree == NULL) {
		printf("Pas assez de mémoire pour allouer l'entrée.\n");
		return 0;
	}
    
	if(strcmp(entree, "\0") == 0) {
		// Si l'utilisateur quitte ou entre une ligne vide.
		free(entree);
        
		return -1;
	}
	
	// Les différentes parties sont séparées par des espaces.
	char* partie = strtok(entree, " ");
	
    // Pile des commandes entrées, une fois exécutées.
	pile* commandes = malloc(sizeof(pile));
	
	if(commandes == NULL) {
		printf("Entrée invalide. La mémoire maximale a été dépassée.\n");
		free(entree);
        
		return 0;
	}
	
	pile_init(commandes);
    
    // Pile des nombres.
	pile* nombres = malloc(sizeof(pile));
	
	if(nombres == NULL) {
		printf("Entrée invalide. La mémoire maximale a été dépassée.\n");
		free(entree);
        
		return 0;
	}
	
	pile_init(nombres);
	
	do {	  
		if(strcmp(partie, "+") == 0 || strcmp(partie, "-") == 0 || strcmp(partie, "*") == 0) {
			if(pile_count(commandes) >= 2) {
				num* r;
				
				num* b = pile_pop(commandes);
				num* a = pile_pop(commandes);
				
				if(strcmp(partie, "+") == 0) {
					r = addition(a, b, a->positif, b->positif);
				} else if(strcmp(partie, "-") == 0) {
					r = soustraction(a, b, a->positif, b->positif);
				} else if(strcmp(partie, "*") == 0) {
					r = multiplication(a, b);
				}
				
				if(r == NULL) {
					printf("Mauvais calcul !\n");
                    
                    superFree(a);
                    superFree(b);
					
                    free(entree);
					free(commandes);
					
                    return 0;
				}
				
                pile_push(nombres, a);
                pile_push(nombres, b);
                pile_push(nombres, r);
                
				pile_push(commandes, r);
			} else {
				printf("Erreur: il manque une entree pour faire une operation !\n");
                
				free(entree);
				free(commandes);
				
                return 0;
			}
		} else if(partie[0] == '=' && strlen(partie) == 2 && partie[1] >= 'a' && partie[1] <= 'z') {
			// Assignation de variable, on assigne et on continue comme si de rien n'était.
			
			if(pile_count(commandes) > 0) {
				variables[(int) partie[1] % 32] = pile_peek(commandes);
                
                // Si ça devient une variable, on ne veut pas l'éliminer de la mémoire.
                pile_pop(nombres);
			} else {
				printf("Que voulez-vous assigner ? Il manque quelque chose...\n");
                
				free(entree);
				free(commandes);
                
				return 0;
			}
		} else if(strlen(partie) == 1 && partie[0] >= 'a' && partie[0] <= 'z') {
			// Variable, on met sa valeur dans la pile d'opérations.
			
			if(variables[(int) partie[0] % 32] != NULL) {
				pile_push(commandes, variables[(int) partie[0] % 32]);
			} else {
				printf("La variable '%c' n'est pas encore définie.\n", partie[0]);
                
				free(commandes);
				free(entree);
                
				return 0;
			}
		} else {
			int est_un_nombre = 1;
			int i = 0;
									
			while(i < strlen(partie)) {
				if(partie[i] < '0' || partie[i] > '9') {
					est_un_nombre = 0;
					
					break;
				}
				
				i++;
			}
			
			if(est_un_nombre == 1) {
				// Valeur, on la met dans un nombre à précision infinie et on met sa référence dans la pile.
				num* valeur = malloc(sizeof(num));
				
				if(valeur == NULL) {
					printf("Entrée invalide. La mémoire maximale a été dépassée.\n");
                    
					free(entree);
					free(commandes);
                    
					return 0;
				}
				
				strToBigNum(partie, valeur);
				
                pile_push(nombres, valeur);
				pile_push(commandes, valeur);
			} else {
				// Caractère non-valide.
				printf("Les éléments doivent tous être des variables (a-z, minuscule), des opérateurs ou bien des valeurs positives entières.\n");
				
				return 0;
			}
		}
	} while(partie = strtok(NULL, " "));
	
	if(partie != NULL) {
		free(partie);
    }
    
	if(entree != NULL) {
		free(entree);
    }
	
	if(pile_count(commandes) == 1) {
		printNum(pile_pop(commandes));
	} else {
		printf("Erreur de syntaxe, veuillez completer vos calculs !\n");
	}
	
    /*while(pile_peek(nombres) != NULL) {
        num* nombre = pile_pop(nombres);
        
        if(nombre != NULL) {
            int i = 0;
            int est_variable = 0;
            
            while(i < 26) {
                if(variables[i] == nombre) {
                    est_variable = 1;
                    break;
                }
                
                i++;
            }
            
            if(est_variable == 0) {
                superFree(nombre);
                nombre = NULL;
            }
        }
    }*/
    
    free(nombres->data);
    free(nombres);
    
    free(commandes->data);
	free(commandes);
    
	return 0;
}

/**
 * Point d'entrée du programme.
 */
int main(int argc, char** argv) {
	while(traitement_commande() != -1);
	
	printf("Le programme va quitter.\n\n");
	return 0;
}

/**
 * Opération d'addition.
 */
num* addition(num* a, num* b, int ancien_a_positif, int ancien_b_positif) {
    if(a->positif == 0 && b->positif == 1)
	{
		a->positif = 1;
        
		return soustraction(b, a, ancien_b_positif, ancien_a_positif); // Vu que a et b sont inversés, leur "ancien_a_positif" et "ancien_b_positif" aussi.
	}

	if(a->positif == 1 && b->positif == 0)
	{
		b->positif = 1;
        
		return soustraction(a, b, ancien_a_positif, ancien_b_positif);
	}
	if(a->positif == 0 && b->positif == 0)
	{
		b->positif = 1;
        
		return soustraction(a, b, ancien_a_positif, ancien_b_positif);
	}
    
	setupNombres(a, b);

	cell* cA = a->nombre;
	cell* cB = b->nombre;

	if(!cA || !cB) {
		return NULL;
	}

	cell* result = malloc(sizeof(cell));
	memcheck(result);
	
	num* r = malloc(sizeof(num));
	memcheck(r);
	
	r->longueur = 0;

	int fini = 0;
	int intermediaire = 0;
	int carry = 0;

	cell* newUnit = NULL;
	r->nombre = result;
	
	while(!fini) {
		fini = cA->suivant == NULL && cB->suivant == NULL;

		intermediaire = cA->chiffre + cB->chiffre + carry;

		if(intermediaire > 9) {
			carry = 1;
			intermediaire -= 10;
		} else {
			carry = 0;
		}

		result->chiffre = intermediaire;
		
		if(!fini) {
			// Ajustement des pointeurs pour le prochain round si pertinent.
			newUnit = malloc(sizeof(cell));
			memcheck(newUnit);
			
			newUnit->suivant = NULL;
			newUnit->precedent = result;

			result->suivant = newUnit;
			result = newUnit;
		}

		cA = cA->suivant;
		cB = cB->suivant;
		r->longueur++;
	}
	
	if(newUnit == NULL) {
		// Cas particulier où on n'a pas de "carry" et un seul digit dans chaque opérande (ex : 2+2).
		result->suivant = NULL;
		r->dernier = result;
	} else {
		newUnit->suivant = NULL;
		r->dernier = newUnit;
	}

	if(carry) {
		cell* carryCell = malloc(sizeof(cell));
		memcheck(carryCell);

		carryCell->chiffre = carry;
		carryCell->precedent = result;
		
		result->suivant = carryCell;
		carryCell->suivant = NULL;
		
		r->dernier = carryCell;
		r->longueur++;
	}

	r->positif = 1;
    
    a->positif = ancien_a_positif;
    b->positif = ancien_b_positif;
    
	return r;
}

/**
 * Opération de soustraction.
 */
num* soustraction(num* a, num* b, int ancien_a_positif, int ancien_b_positif) 
{
    if(a->positif == 0 && b->positif ==  1)
	{
		a->positif = 1;
        
		num* res = addition(a, b, ancien_a_positif, ancien_b_positif);
        
		res->positif = 0;
        
		return res;
	}
    
	cell* result = malloc(sizeof(cell));
	memcheck(result);
	
	cell* newUnit = NULL;
	cell* cA = NULL;
	cell* cB = NULL;

	int fini = 0;
	int retenue = 0;
	int intermediaire = 0;

	num* r = malloc(sizeof(num));
	memcheck(r);
	
	r->longueur = 0;

	if(numComparator(a,b) == -1) {
		num* temp = a;
		
		a = b;
		b = temp;
		
		r->positif = 0;
	} else {
		r->positif = 1;
	}
	
	setupNombres(a, b);
	
	cA = a->nombre;
	cB = b->nombre;
	
	r->nombre = result;
	
	do {
		fini = cA->suivant == NULL && cB->suivant == NULL;
		intermediaire = cA->chiffre - cB->chiffre;

		if(intermediaire < 0) {
			cA->chiffre += 10;
			
			if(cA->suivant != NULL) {
				cA->suivant->chiffre--;
			}
			
			intermediaire = cA->chiffre - cB->chiffre;
		}
		
		result->chiffre = intermediaire;

		if(!fini) {
			// Ajustement des pointeurs pour le prochain round si pertinent.
			newUnit = malloc(sizeof(cell));
			memcheck(newUnit);

			newUnit->precedent = result;

			result->suivant = newUnit;
			result = newUnit;
		}
		
		r->longueur++;
		cA = cA->suivant;
		cB = cB->suivant;
	} while(!fini);

	if(newUnit == NULL) {
		result->suivant = NULL;
		r->dernier = result;
	} else {
		newUnit->suivant = NULL;
		r->dernier = newUnit;
	}
    
    if(numComparator(b,a) == -1) {
		num* temp_reverse = a;
		
		a = b;
		b = temp_reverse;
    }
    
    a->positif = ancien_a_positif;
    b->positif = ancien_b_positif;
    
	return r;
}

/**
 * Opération de multiplication.
 */
num* multiplication(num* a, num* b) {
	cell* cA = a->nombre;
	cell* cB = b->nombre;
	cell* result = NULL;

	num* listeToAdd = malloc(b->longueur * sizeof(num));
	memcheck(listeToAdd);

	num* r = malloc(sizeof(num));
	memcheck(r);

	r->longueur = 0;
	r->positif = a->positif == b->positif;
	
	int finiCB = 0;
	int finiCA = 0;
	int intermediaire = 0;
	int carry = 0;
	int chiffreCourant = 0;
	int decalageUnit = 0;

	cell* newUnit = NULL;
	
	do {
		chiffreCourant = cB->chiffre;
		cA = a->nombre;
		
		result = malloc(sizeof(cell));
		memcheck(result);
		
		result->suivant = NULL;
		result->precedent = NULL;
		
		r->nombre = result;
		r->longueur = 0;
		
		carry = 0;

		do {
			// Multiplication en tant que telle.
			finiCA = cA->suivant == NULL;
			
			intermediaire = chiffreCourant * cA->chiffre + carry;
			carry = 0;
			
			while(intermediaire > 9) {
				carry++;
				intermediaire -= 10;
			}
			
			result->chiffre = intermediaire;

			if(!finiCA) { 
				// Ajustement des pointeurs pour le prochain round si pertinent.
				newUnit = malloc(sizeof(cell));
				memcheck(newUnit);
				
				newUnit->precedent = result;

				result->suivant = newUnit;
				result = newUnit;
			}
			
			cA = cA->suivant;
			r->longueur++;
		} while(!finiCA);
		
		if(newUnit == NULL) {
			// Cas particulier si un seul digit dans chaque opérande (ex: 2*2).
			result->suivant = NULL;
			r->dernier = result;
		} else {
			newUnit->suivant = NULL;
			r->dernier = newUnit;
		}
		
		// Ajuster le carry s'il le faut.
		if(carry) {
			cell* carryCell = malloc(sizeof(cell));
			memcheck(carryCell);

			carryCell->chiffre = carry;
			carryCell->precedent = result;
			
			result->suivant = carryCell;
			carryCell->suivant = NULL;
			
			r->dernier = carryCell;
			r->longueur++;
		}

		// Ajuster les zéros.
		cell* newZero = NULL;
		cell* currentZero = NULL;
		cell* premierZero = NULL;
		
		int i = 0;
		
		if(decalageUnit > 0) {
			currentZero = malloc(sizeof(cell));
			memcheck(currentZero);
			
			currentZero->chiffre = 0;
			currentZero->precedent = NULL;
			premierZero = currentZero;

			for(i = 1; i < decalageUnit; i++) {
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
		
		listeToAdd[decalageUnit] = *r;
		
		cB = cB->suivant;
		finiCB = cB == NULL;
		
		decalageUnit++;
	} while(!finiCB);
	
	int j;
	
	num* somme = malloc(sizeof(num));
	memcheck(somme);
	
	strToBigNum("0", somme);

	for(j = 0; j < b->longueur; j++) {
		somme = addition(somme, &listeToAdd[j], somme->positif, (&listeToAdd[j])->positif);
	}
	
	superFree(listeToAdd);
	
	return somme;
}

/**
 * Libère un nombre de la mémoire.
 */
void superFree(num* toFree) {
	cell* c = toFree->nombre;
	
	if(c != NULL) {
		recursiveSuperFree(c);
	}
	
	free(toFree);
	
	toFree = NULL;
}

/**
 * Libère une cellule de la mémoire, récursivement.
 */
void recursiveSuperFree(cell* c) {
	if(c->suivant == NULL) {
		free(c);
		
		c = NULL;
	} else {
		recursiveSuperFree(c->suivant);
	}
}

/**
 * Valide les nombres entrés et ajuste leur longueur, au besoin.
 */
void* setupNombres(num* a, num* b) {
	if(a->longueur < b->longueur) {
		num* temp = a;
		
		a = b;
		b = temp;
	}
	
	cell* dernier = b->dernier;
	cell* newZero = NULL;
	
	while(b->longueur < a->longueur) {
		// Ajustement de la longueur du plus petit nombre.
		newZero = malloc(sizeof(cell));
		memcheck(newZero);
		
		newZero->chiffre = 0;
		newZero->precedent = dernier;
		
		dernier->suivant = newZero;
		dernier = newZero;
		
		b->dernier = dernier;
		b->longueur++;
	}
	
	dernier->suivant = NULL;
}

/**
 * Transforme un nombre dans une string en chaîne liée de précision infinie.
 * Retourne -1 s'il n'y a pas assez de mémoire pour stocker le nombre.
 */
int strToBigNum(char* str, num* toCreate) {
	int i;
	
	cell** c = &toCreate->nombre;
	cell* precedent = NULL;
	
	int positif = str[0] != '-';
	
	toCreate->positif= positif;
	toCreate->longueur = 0;

	// Les nombres sont stockés en little endian pour faciliter le calcul.
	// Si le nombre est positif, "1-positif" = 0.
	for(i = strlen(str)-1; i >= 1 - positif; i--) {
		*c = malloc(sizeof(cell));

		if(*c == NULL) {
			printf("Mémoire épuisée.\n");
			superFree(toCreate);
			
			return -1;
		}

		(*c)->precedent = precedent;
		precedent = *c;
		
		// Transforme un char en int. Fonctionne car on a seulement des nombres de 0 à 9.
		(*c)->chiffre = str[i] - '0';
		
		toCreate->longueur++;
		
		if(i == 0) {
			// On garde une référence sur le dernier élément pour imprimer efficacement le nombre.
			toCreate->dernier = *c;
		}

		c = &(*c)->suivant;
	}
	
	*c = NULL;
	
	return 0;
}

/**
 * Compare les quantités de chaque nombre: zéro si nombre égaux, 1 si a > b, -1 si a < b.
 * (Voir "Spaceship operator").
 */
int numComparator(num* a, num* b) {
	if(a->positif > b->positif) {
		return 1;
	}
	
	if(a->positif < b->positif) {
		return -1;
	}

	// Les nombres ont le même signe.
	if(a->longueur > b->longueur) {
		return a->positif ? 1 : -1;
	}
	
	if(a->longueur < b->longueur) {
		return a->positif ? -1 : 1;
	}

	if(a->longueur == 1 && b->longueur == 1) {
		return (a->dernier->chiffre) > (b->dernier->chiffre) ? 1 : -1;
	}
	
	// Les deux nombres ont la même longueur (> 1) et le même signe.
	// On compare en commençant par la partie la plus singificative.
	cell* cA = a->dernier;
	cell* cB = b->dernier;

	while(cA->precedent != NULL) {
		if(cA->chiffre > cB->chiffre) {
			return a->positif ? 1 : -1;
		}
		
		if(cA->chiffre < cB->chiffre) {
			 return a->positif ? -1 : 1;
		}
		
		cA = cA->precedent;
		cB = cB->precedent;
	}

	return 0;
}

/**
 * Affiche un nombre.
 */
void printNum(num* toPrint) {
    if(toPrint->longueur == 1 && toPrint->nombre->chiffre == 0)
	{
		printf("0\n");
		return;
	}

	if(!toPrint->positif) 
	{
		// Nombre négatif, pas 0.
		printf("-");
	}
	
	cell* nombre = toPrint->dernier;

	int position = toPrint->longueur;
	
	// Cas particulier si le nombre est 0.
	while(nombre->chiffre == 0 && position != 1) {
		nombre = nombre->precedent;
		position--;
	}
	
	// On peut afficher les nombres dans l'ordre. 
	while(nombre->precedent != NULL) {
		printf("%d", nombre->chiffre);
		nombre = nombre->precedent;
	}
	
	// On affiche le dernier chiffre.
	printf("%d", nombre->chiffre);
	
	printf("\n");
}

// Lit une ligne d'une longueur arbitraire et la retourne.
// Retourne NULL s'il n'y a pas assez de mémoire.

char* entreeDynamique(FILE* input) {
	// 32 bytes par défaut.
	int defaut = 32;
	
	char* mot = malloc(sizeof(char) * defaut);
	memcheck(mot);
	
	int taille = 0;
	
	char c;
	
	while(1) {
		c = fgetc(input);
		
		if(c == EOF || c == '\n') {
			break;
		}
		
		mot[taille] = c;
		
		taille++;
		
		// "realloc" si la ligne est plus grande que l'espace alloué.
		if(taille > defaut) {
			defaut += 32;
			
			mot = realloc(mot, sizeof(char) * defaut);
			memcheck(mot);
		}
	}
	
	if(taille > defaut) {
		taille++;
		
		mot = realloc(mot, sizeof(char) * taille);
		memcheck(mot);
	}
	
	// Pour terminer la string.
	mot[taille] = '\0';

	return mot;
}

/**
 * Initialisation d'une pile.
 */
void pile_init(pile* p) {
	p->data = malloc(10 * sizeof(*p->data));
	
	if(p->data == NULL) {
		printf("Mémoire épuisée, impossible de continuer le traitement.\n");
		exit(1);
	}
	
	p->size = 0;
	p->max_size = 10;
}

/**
 * Aperçu du haut de la pile.
 * Retourne NULL si la pile est vide.
 */
num* pile_peek(pile* p) {
	if(p->size == 0) {
		return NULL;
	}
	
	return p->data[p->size - 1];
}

/**
 * Ajout d'un élément au sommet d'une pile.
 * Si la pile est pleine, réinitialise la pile à la taille double.
 */
void pile_push(pile* p, num* n) {
	if(p->size < p->max_size) {
		p->data[p->size++] = n;
	} else {
		p->max_size *= 2;
		
		p->data = realloc(p->data, p->max_size * sizeof(*p->data));
		
		if(p->data == NULL) {
			printf("Mémoire épuisée, impossible de continuer le traitement.\n");
			exit(1);
		}
		
		pile_push(p, n);
	}
}

/**
 * Enlève et retourne le sommet de la pile.
 */
num* pile_pop(pile* p) {
	if(p->size == 0) {
		return NULL;
	} else {
        if(p->size < p->max_size / 2) {
            p->max_size /= 2;
            p->data = realloc(p->data, p->max_size * sizeof(*p->data));
            
            if(p->data == NULL) {
                printf("Mémoire épuisée, impossible de continuer le traitement.\n");
                exit(1);
            }
        }
        
		num* top = pile_peek(p);
		p->size--;
		
		return top;
	}
}

/**
 * Retourne la taille de la pile.
 */
int pile_count(pile* p) {
	return p->size;
}
