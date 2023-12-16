#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*
come risolvere le collisioni:

open addressing -> tutto nella tabella -> troviamo un altro spazio -> 
    linear probing (controlliamo gli spazi successivi/intorno) -> se lo spazio finisce fa schifo

external chaining -> linked list serieeeeeeeee
*/

/*
struct HashFn
{
	std::size_t operator() (const Tile &p) const
	{
		std::size_t h1 = std::hash<int>()(p.x);
		std::size_t h2 = std::hash<int>()(p.y);

		return h1 ^ (h2 << 1);
	}
};
*/

#define MAX_NAME 256 //carattere massimo della persona
#define TABLE_SIZE 10 //grandezza array

typedef struct person{
    char name[MAX_NAME]; //nome della persona
    int age;
    struct person * next;
} person;

person * hash_table[TABLE_SIZE];

unsigned int hash(char * name){
    int lenght = strnlen(name, MAX_NAME);
    unsigned int hash_value = 0;
    for (int i = 0; i < lenght; i++){
        hash_value += name[i];
        hash_value = (hash_value * name[i]) % TABLE_SIZE;
    }
    return hash_value;
}

void init_hash_table(){
    for (int i=0; i<TABLE_SIZE; i++){
        hash_table[i]=NULL;
    }
    //tile_set is empty
}

void print_table(){
    printf("Start\n");
    for (int i=0; i<TABLE_SIZE; i++){
        if (hash_table[i] == NULL){
            printf("\t%i\t---\n",i);
        } else {
            printf("\t%i\t ",i);
            person *tmp = hash_table[i];
            while (tmp != NULL){
                printf("%s - ", tmp->name);
                tmp = tmp -> next;
            }
            printf("\n");
        }
    }
    printf("End\n");
}

bool tile_set_insert(person * p){
    if (p==NULL) return false;
    int index = hash(p->name);
    p -> next = hash_table[index];
    hash_table[index]=p;
    return true;
}

person * tile_set_lookup (char * name){
    int index = hash(name);
    person * tmp = hash_table[index];
    while (tmp != NULL && strncmp(tmp->name,name,MAX_NAME) != 0)
    {
        tmp = tmp->next;
    }
    return tmp;
}

person * tile_set_delete(char * name){
    int index = hash(name);
    person * tmp = hash_table[index];
    person * prev = NULL;
    while (tmp != NULL && strncmp(tmp->name,name,MAX_NAME) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL) return NULL; //se non hai trovato niente
    if (prev == NULL) {
        //rimuovo la testa
        hash_table[index] = tmp->next;
    } else {
        prev->next = tmp->next;
    }
    return tmp;
}

int main(){

    init_hash_table();
    print_table();

    person gianni = {.name="gianni", .age=25};
    person mario = {.name="mario", .age=32};


    tile_set_insert(&gianni);
    tile_set_insert(&mario);
    
    print_table();

    person * tmp = tile_set_lookup("mario");

    if (tmp == NULL){
        printf("not found");
    } else {
        printf ("found %s.\n", tmp->name);
    }

    tile_set_delete("mario");

    tmp = tile_set_lookup("mario");

    if (tmp == NULL){
        printf("not found");
    } else {
        printf ("found %s.\n", tmp->name);
    }

    // printf ("Dario => %u\n", hash("Dario"));
    return 0;
}