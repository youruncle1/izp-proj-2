/** @file newsetcal.c
* @brief Application for working with sets and relations.
* Authors:
* @FrodoCZE login: xdolez0c
* @Midiros login: xlegne00
* @youruncle1 login: xpolia05

Last updated 3.12.2021.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define NUM_OF_KEYWORDS 21
#define KEYWORD_MAX_LEN 14

typedef struct
{
    char element[31];
} Universum_elements;

typedef struct
{
    Universum_elements *elements;
    int universum_cardinality;
    int capacity;
} Universum;

typedef struct
{
    int *set_items;
    int capacity;
    int cardinality;
    int line_index;
} Set_line;

typedef struct
{
    Set_line *l;
    int line_capacity;
    int line_cardinality;
} Sets;

typedef struct
{
    int first;
    int second;
} Pairs;

typedef struct
{
    Pairs *p;
    int cardinality;
    int capacity;
    int line_index;
} Relation_line;

typedef struct
{
    Relation_line *l;
    int line_capacity;
    int line_cardinality;
} Relations;

typedef struct
{
    Universum *u;
    Sets *s;
    Relations *r;

} Main;

void *allocate_or_resize(void *ptr, unsigned int size);
int init_universum(Main *m);
int init_set(Main *m);
int init_relation(Main *m);
void main_dtor(Main *m, int depth);
Main *main_ctor();
int uni_add_element(Main *m, char *element, int idx);
void print_set(Main *m, int cardinality, int *set);
int set_line_add(Main *m, int line_index);
int set_add_element(Main *m, int element_index, int idx);
int is_set(Main *m);
int find_function_keyword(char *str,bool find_function);

/**
 * @brief reallocates/allocates pointer pointing to an adress
 *
 * @param ptr pointer to address which needs to be reallocated
 * @param size size in int which memory will be resized
 * @return pointer to allocated memory or NULL if failed to allocate
 */
void *allocate_or_resize(void *ptr, unsigned int size)
{

    void *new_arr = realloc(ptr, size);
    if (new_arr == NULL)
    {
        free(ptr);
        return NULL;
    }

    return new_arr;
}

/**
 * @brief
 *
 * @param m
 * @return int
 */
int init_universum(Main *m)
{
    m->u->capacity = 1;
    m->u->universum_cardinality = 0;
    m->u->elements = malloc(sizeof(Universum_elements));
    if (m->u->elements == NULL)
    {
        return 0;
    }

    return 1;
}

/**
 * @brief
 *
 * @param m
 * @return int
 */
int init_set(Main *m)
{
    m->s->l = malloc(sizeof(Set_line));
    if (m->s->l == NULL)
        return 0;
    m->s->l[0].line_index = 0;
    m->s->l[0].capacity = 0;
    m->s->l[0].cardinality = 0;
    m->s->line_capacity = 1;
    m->s->line_cardinality = 0;
    

    return 1;
}

/**
 * @brief
 *
 * @param m
 * @return int
 */
int init_relation(Main *m)
{
    m->r->l = malloc(sizeof(Relation_line));
    if (m->r->l == NULL)
        return 0;
    m->r->l[0].line_index = 0;
    m->r->l[0].capacity = 0;
    m->r->l[0].cardinality = 0;
    m->r->line_cardinality = 0;
    m->r->line_capacity = 1;
    return 1;
}

/**
 * @brief Function which deconstructs a main structure.
 *
 * @param m
 * @param depth
 */
void main_dtor(Main *m, int depth)
{
    if(m->r->l->p != NULL)
    if (depth == 3)
    {
        if(m->r->l[0].p != NULL){
            free(m->r->l[0].p);
        }
        if(m->s->l[0].set_items != NULL){
            free(m->s->l[0].set_items);
        }
        for (int i = m->s->line_cardinality -1; i > 0; i--)
        {
            free(m->s->l[i].set_items);
        }
        for (int j = m->r->line_cardinality -1; j > 0; j--)
        {
            free(m->r->l[j].p);
        }
    
    }
    if (depth >= 2)
    {
        free(m->r->l);
        free(m->s->l);
        free(m->u->elements);
    }
    if (depth >= 1)
    {
        free(m->u);
        free(m->s);
        free(m->r);
    }
    if (depth >= 0)
    {
        free(m);
    }
}

/**
 * @brief Function which constructs a main structure.
 *
 * @return Main*
 */
Main *main_ctor()
{

    Main *m = malloc(sizeof(Main));
    if (m == NULL)
        return NULL;
    m->u = malloc(sizeof(Universum));
    m->s = malloc(sizeof(Sets));
    m->r = malloc(sizeof(Relations));
    if (m->u == NULL || m->s == NULL || m->r == NULL)
    {
        main_dtor(m, 1);
        return NULL;
    }
    if (init_universum(m) && init_set(m) && init_relation(m))
    {   
        m->s->l->set_items  = malloc(sizeof(Set_line));
        m->r->l->p = malloc(sizeof(Pairs));
        if (m->s->l->set_items == NULL || m->r->l->p == NULL)
        {
            main_dtor(m, 3);
            return NULL;
        }
        return m;
    }
    else
    {
        main_dtor(m, 2);
    }
    return NULL;
}

/**
 * @brief Function which checks whether the input is valid or if it is a empty universum/set/relation.
 *
 * @param file 
 * @return returns an int representing U/R/S/C or representing empty U/R/S
 */
int type_check(FILE *file){
    char character = getc(file);
    // printf("typecheck char: %d\n", character);
    if (character == 'U'){
        if((character = getc(file)) == '\n' || character == EOF){
            return 7; //empty uni
        }
        if (character == ' '){
            return 2;
        }
    }
    if (character == 'S'){
        if ((character = getc(file)) == '\n' || character == EOF){
            return 8; //empty set
        }
        else if (character == ' '){
            return 3;
        }
    }
    if (character == 'R'){
        if ((character = getc(file)) == '\n' || character == EOF){
            return 9; //empty relation
        }
        else if (character == ' '){
            return 4;
        }
    }
    if (character == 'C'){
        if (getc(file) == ' '){
            return 5;
        }
    }
    if (character == '\n'){
        return -1;
    }
    if (character == EOF){
        return 0;
    }
    if (character == 10){
        return 0;
    }
    return -1;
}

/**
 * @brief Function which adds an element to the universum.
 * Also checks whether the universum cardinality hasnt exceeded the capacity, if yes it calls for a resizing of the universum.
 *
 * @param m represents the main structure.
 * @param element represents which element is to be added.
 * @param idx 
 * @return true if the element was added, false if not.
 */
int uni_add_element(Main *m, char *element, int idx)
{
    if (m->u->universum_cardinality + 1 > m->u->capacity)
    {

        m->u->capacity = m->u->capacity * 2 + 1;
        m->u->elements = allocate_or_resize(m->u->elements, m->u->capacity * sizeof(Universum_elements));
        if ((m->u->elements) == NULL)
            return false;
    }
    if(idx == -1){
        m->u->universum_cardinality = 0;
    }
    else{
        strcpy(m->u->elements[idx].element, element);
        (m->u->universum_cardinality)++;
    }
    return TRUE;
}

/**
 * @brief Function which checks what command is supposed to be used.
 *
 * @param str the string which represents the name of command that the user entered.
 * @param find_function         .
 * @return index value of the command.
 */
int find_function_keyword(char *str,bool find_function){

    int keyword_count = 21;
    char functions[NUM_OF_KEYWORDS][KEYWORD_MAX_LEN] = {
    // 1 input 0-9
    {"empty"},
    {"card"},
    {"complement"},
    {"reflexive"},
    {"symmetric"},
    {"antisymmetric"},
    {"transitive"},
    {"function"},
    {"domain"},
    {"codomain"},
    // 2 inputs 10-15
    {"union"},
    {"intersect"},
    {"minus"},
    {"subseteq"},
    {"subset"},
    {"equals"},
    // 3 inputs 16-19
    {"injective"},
    {"surjective"},
    {"bijective"},
    {"true"},
    {"false"},
    };
    if(find_function)
        keyword_count = 19;

    for (int i = 0; i < keyword_count; i++){
        if (!(strcmp(str, functions[i])))
            return i;
    }
    return -1;
}

/**
 * @brief Function which checks whether a word is composed of letters only.
 * @param m points to main structure.
 * @return True if its made up only of letters, else false.
 * @see isalpha
 */

int is_alpha_only(char *str){
    for (int i = 0; str[i] != '\0'; i++){
        if(!isalpha((int)str[i]))
            return 0;
    }
    return 1;
}

/**
 * @brief
 *
 * @param m
 * @param element
 * @param idx
 * @return int
 */
int load_universum(FILE *file, Main *m, int line_index)
{
    char element[31];
    char character;
    int idx = 0;
    int element_len = 0;

    character = getc(file);

    while (character != '\n' && character != EOF)
    {
        if (element_len > 29)
        {
            fprintf(stderr, "ERROR: Element defined in universe longer than 30 characters!\n");
            return -1;
        }

        if (character != ' ')
        {
            element[element_len++] = character;
        }
        character = getc(file);
        if (character == ' ' || character == '\n')
        {
            element[element_len] = '\0';
            if(!is_alpha_only(element)){
                fprintf(stderr, "ERROR: Universum element contains non-alpha character!\n");
                return -1;
            }
            if(find_function_keyword(element, 0) != -1){
                fprintf(stderr, "ERROR: Forbidden word defined in universum!\n");
                return -1;
            }
            uni_add_element(m, element, idx);
            if (m->u->elements[idx].element == NULL)
            {
                fprintf(stderr, "ERROR: Memory allocation failure...\n");
                return -1;
            }
            idx++;
            element_len = 0;
            strcpy(element, "");
        }
    }
    set_line_add(m, line_index);
    for(int i = 0; i < m->u->universum_cardinality; i++)
        set_add_element(m, i, i);
    (m->s->line_cardinality)++;
    return 1;
}
/**
 * @brief Function for checking whether an universum is valid. It checks for duplicity of elements within the universum.
 * @param m points to main structure.
 * @return True if it is a valid universum, false if it isn't.
 */

int is_universum_valid(Main *m){
    int universum_cardinality = m->u->universum_cardinality;
    for (int i = 0; i < universum_cardinality; i++){
        for(int j = i+1; j < universum_cardinality; j++){
            if (strcmp(m->u->elements[i].element,m->u->elements[j].element) == 0){
                fprintf(stderr, "ERROR: Universum has duplicate elements\n");
                return -1;
            }
        }
    }
    return 1;
}

/**
 * @brief Checks whether an element is in the universum if so at which index.
 *
 * @param m points to main structure.
 * @param str represents the string that is to be seeked in the universum.
 * @return int value at which index the word was wound, in case it wasnt found returns an invalid value.
 */
int is_in_universum(Main *m, char *str)
{
    for (int j = 0; j < (m->u->universum_cardinality); j++)
    {
        if (!(strcmp(str, m->u->elements[j].element)))
        {
            return j;
        }
    }
    return -1;
}

/**
 * @brief Function for adding elements to sets.
 * Function adds elements to sets it also checks whether the set capacity is near its maximum capacity.
 * If the capacity is soon to be exceeded it  reallocates the size of the memory for the set.
 * @see allocate_or_resize
 * @param m points to main structure.
 * @param element_index represents the index in the universum at which the element we want to add is.
 * @param idx represents the index in the set at which we want to add the new element.
 */
int set_add_element(Main *m, int element_index, int idx)
{
    int line_cardinality = m->s->line_cardinality;
    if (m->s->l[line_cardinality].cardinality + 1 > m->s->l[line_cardinality].capacity){
        m->s->l[line_cardinality].capacity = m->s->l[line_cardinality].capacity * 2 + 1;
        m->s->l[line_cardinality].set_items = allocate_or_resize(m->s->l[line_cardinality].set_items, m->s->l[line_cardinality].capacity * sizeof(int));
        if ((m->s->l[line_cardinality].set_items) == NULL)
        {
            fprintf(stderr, "Memory for set could not be allocated\n");
            return false;
        }
    }
    if (element_index == -1){
        m->s->l[line_cardinality].cardinality = 0;
    }
    else{
        m->s->l[line_cardinality].set_items[idx] = element_index;

        (m->s->l[line_cardinality].cardinality)++;
    }
    return -1;
}
/**
 * @brief Function for checking whether a set is valid. It checks for duplicity within the set.
 * @param m points to main structure.
 * @return True if it is a valid set, false if it isn't.
 */

int is_set(Main *m){
    int line_cardinality = m->s->line_cardinality;
    int set_cardinality = m->s->l[line_cardinality].cardinality;
    for(int i = 0; i < set_cardinality; i++){
        for(int j = i+1; j < set_cardinality; j++){
            if(m->s->l[line_cardinality].set_items[i] == m->s->l[line_cardinality].set_items[j]){
                fprintf(stderr, "ERROR: Set element duplicity!");
                return -1;
            }
        }
    }
    return true;
}

/**
 * @brief Function for checking whether a relation is valid. It checks for duplicity of pairs within the relation.
 * @param m points to main structure.
 * @return True if it is a valid relation, false if it isn't.
 */
int is_relation(Main *m){
    int line_cardinality = m->r->line_cardinality;
    int rel_cardinality = m->r->l[line_cardinality].cardinality;
    for(int i = 0; i < rel_cardinality; i++){
        for (int j = i+1; j < rel_cardinality; j++)
            if((m->r->l[line_cardinality].p[i].first == m->r->l[line_cardinality].p[j].first) && (m->r->l[line_cardinality].p[i].second == m->r->l[line_cardinality].p[j].second)){
                fprintf(stderr, "ERROR: Relation element duplicity!\n");
                return -1;
            }
        }
    return true;    
}

/**
 * @brief Function which converts elements to index numbers from the universum for ease of work within the programm. 
 *
 * @param file represents the file from which the data is to be retrieved from.
 * @param m points to main structure.
 * @see is_in_universum
 */
int set_to_index(FILE *file, Main *m)
{
    char character = getc(file);
    char element[31];
    int element_len = 0;
    int idx = 0;
    int set_index = 0;

    while (character != '\n' && character != EOF)
    {
        if (character != ' ')
        {
            element[element_len++] = character;
        }
        character = getc(file);
        if (character == ' ' || character == '\n' || character == EOF)
        {
            element[element_len] = '\0';
            set_index = is_in_universum(m, element);
            if (set_index != -1)
            {
                set_add_element(m, set_index, idx);
                if (m->s->l->set_items == NULL)
                {
                    fprintf(stderr, "ERROR: Memory allocation failure...\n");
                    return -1;
                }
                idx++;
                element_len = 0;
                strcpy(element, "");
            }
            else
            {
                fprintf(stderr, "ERROR: Set element is not in universum\n");
                return -1;
            }
        }
    }
    return 1;
}

/**
 * @brief Function for adding elements to relation.
 * Function adds elements to relation it also checks whether the relation capacity is near its maximum capacity.
 * If the capacity is soon to be exceeded it reallocates the size of the memory for the relation.
 * @see allocate_or_resize
 * @param m represents the main structure.
 * @param element_index represents the index in the universum at which the element we want to add is.
 * @param isfirst If the value is true the element is added to first element of the pair.If the value is false the element is added to the second element of the pair.
 * @param idx represents the index of the pair in the relation at which we want to add the new element.
 */
int relation_add_element(Main *m, int element_index, bool isfirst, int idx)
{

    int ln_car = m->r->line_cardinality;

    if (m->r->l[ln_car].cardinality + 1 > m->r->l[ln_car].capacity)
    {
        m->r->l[ln_car].capacity = m->r->l->capacity * 2 + 1;
        m->r->l[ln_car].p = allocate_or_resize(m->r->l[ln_car].p, m->r->l[ln_car].capacity * sizeof(m->r->l[ln_car].p));
        if ((m->r->l[ln_car].p) == NULL)
        {
            return false;
        }
    }
    if (isfirst)
    {
        m->r->l[ln_car].p[idx].first = element_index;
    }
    if (!isfirst)
    {
        m->r->l[ln_car].p[idx].second = element_index;
        (m->r->l[ln_car].cardinality)++;
    }

    return -1;
}

/**
 * @brief Function which converts elements to index numbers from the universum for ease of work within the programm.
 *
 * @param file represents the file from which the data is to be retrieved from.
 * @param m represents the main structure.
 * @see is_in_universum
 * @see relation_add_element
 */
int relation_to_index(FILE *file, Main *m)
{

    bool first_word_loaded = false;
    bool second_word_loaded = false;
    char temp[31];
    int index = 0;
    char character;
    int idx = 0;
    character = 'd';

    while (character != '\n' && character != EOF){
        character = getc(file);
        if(character == ')'){
            fprintf(stderr,"ERROR: Wrong relation syntax\n");
            return -1;
        }
        if (character == '('){
            while (character != ' '){
                character = getc(file);
                if(character == '(' || character == ')'){
                    fprintf(stderr,"ERROR: Wrong relation syntax\n");
                    return -1;
                }
                if (character != ' ')
                    temp[index++] = character;
            }
            temp[index] = '\0';
            index = 0;
            if ((is_in_universum(m, temp)) != -1){
                relation_add_element(m, is_in_universum(m, temp), 1, idx);
            }
            else{
                fprintf(stderr,"ERROR: Relation element not in universum\n");
                return -1;
            }
            strcpy(temp, "");
            first_word_loaded = true;
            while (character != ')') {
                character = getc(file);
                if(character == ' ' || character == '('){
                    fprintf(stderr,"ERROR: Wrong relation syntax\n");
                    return -1;
                }
                if (character != ')')
                    temp[index++] = character;
            }
            temp[index] = '\0';
            index = 0;
            if ((is_in_universum(m, temp)) != -1){
                relation_add_element(m, is_in_universum(m, temp), 0, idx);
            }
            else{
                fprintf(stderr,"ERROR: Relation element not in universum\n");
                return -1;
            }
            strcpy(temp, "");
            second_word_loaded = true;
            if (second_word_loaded && first_word_loaded){
                idx++;
                first_word_loaded = false;
                second_word_loaded = false;
            }
            else{
                fprintf(stderr,"ERROR: Wrong relation syntax\n");
                return -1;
            }
        }
    }

    return true;
}
/**
 * @brief Function finds at what index within the main set structure the set is located on.
 *
 * @param m represents the main structure.
 * @param line_index.
 * @return index value of the set in the main set structure
 */
int set_find_index(Main *m, int line_index)
{
    for (int i = 0; i < m->s->line_cardinality; i++)
    {
        if (m->s->l[i].line_index == line_index)
            return i;
    }
    return -1;
}
/**
 * @brief Function finds at what index within the main relation structure the relation is located on.
 *
 * @param m represents the main structure.
 * @param line_index represents the index of the relation we want to check.
 * @return index value of the relation in the main relation structure
 */

int rel_find_index(Main *m, int line_index)
{
    for (int i = 0; i < m->r->line_cardinality; i++)
    {
        if (m->r->l[i].line_index == line_index)
            return i;
    }
    return -1;
}

/**
 * @brief Function checks whether a set is empty or not.
 *
 * @param m represents the main structure.
 * @param line_index represents the index of the set we want to check.
 */
void is_empty(Main *m, int line_index)
{
    printf("%s\n", m->s->l[line_index].cardinality > 0 ? "false" : "true");
}
/**
 * @brief Function which prints out the intersection between set A and set B ---- A\B.
 *
 * @param m represents the main structure.
 * @param set_line_index_a represents the line index of the set A.
 * @param set_line_index_b represents the line index of the set B.
 * @see print_set()
 */
void intersect(Main *m, int set_line_index_a, int set_line_index_b)
{
    int i, h;
    int intersect_cardinality = 0;
    int *intersect = malloc(sizeof(int) * m->s->l[set_line_index_a].cardinality);
    for (i = 0; i < m->s->l[set_line_index_a].cardinality; ++i)
    {
        int a_index = m->s->l[set_line_index_a].set_items[i];
        for (h = 0; h < m->s->l[set_line_index_b].cardinality; h++)
        {

            int b_index = m->s->l[set_line_index_b].set_items[h];
            if (a_index == b_index)
            {
                intersect[intersect_cardinality] = a_index;
                intersect_cardinality++;
                continue;
            }
        }
    }
    print_set(m, intersect_cardinality, intersect);
    free(intersect);
}

void do_complement(Main *m, int line_index)
{
    bool found;
    printf("S ");
    for (int i = 0; i < m->u->universum_cardinality; i++)
    {
        found = false;
        for (int j = 0; j < m->s->l[line_index].cardinality; j++)
        {
            if (m->s->l[line_index].set_items[j] == i)
                found = true;
        }
        if (!found)
            printf("%s ", m->u->elements[i].element);
    }
}

/**
 * @brief  
 * 
 *
 *
 * @param m
 * @param set_index_a
 * @param set_index_b
 */
void do_union(Main *m, int set_line_index_a, int set_line_index_b)
{
    bool in_union;
    int cardinality_a = m->s->l[set_line_index_a].cardinality;
    int cardinality_b = m->s->l[set_line_index_b].cardinality;
    int *union_set = malloc((cardinality_a) + (cardinality_b) * sizeof(int));
    int i;
    int union_cardinality = 0;

    for (i = 0; i < cardinality_a; i++)
    {
        union_set[i] = m->s->l[set_line_index_a].set_items[i];
        union_cardinality++;
    }
    for (int j = 0; j < cardinality_b; j++)
    {
        in_union = false;
        for (int k = 0; k < union_cardinality; k++)
        {
            if (m->s->l[set_line_index_b].set_items[j] == union_set[k])
            {
                in_union = true;
            }
        }
        if (!in_union)
        {
            union_set[i] = m->s->l[set_line_index_b].set_items[j];
            union_cardinality++;
            i++;
        }
    }
    print_set(m, union_cardinality, union_set); // outputs the result
    free(union_set);
}
/**
 * @brief Function which prints out the difference between set A and set B ---- A\B.
 *
 * @param m represents the main structure.
 * @param line_index_a represents the line index of the set A.
 * @param line_index_b represents the line index of the set B.
 * @see print_set()
 */

void minus(Main *m, int line_index_a, int line_index_b)
{
    int cardinality_a = m->s->l[line_index_a].cardinality;
    int cardinality_b = m->s->l[line_index_b].cardinality;
    bool element_found;
    int *difference_set = malloc((cardinality_a + cardinality_b) * sizeof(int));
    int diff_idx = 0;

    for (int i = 0; i < cardinality_a; i++)
    {
        element_found = false;
        for (int j = 0; j < cardinality_b; j++)
        {
            if (m->s->l[line_index_a].set_items[i] == m->s->l[line_index_b].set_items[j])
            {
                element_found = true;
                break;
            }
        }
        if (!element_found)
        {
            difference_set[diff_idx] = m->s->l[line_index_a].set_items[i];
            diff_idx++;
        }
    }
    print_set(m, diff_idx, difference_set);
    free(difference_set);
}

/**
 * @brief Function which compares two sets and returns whether set A is a subset of set B.
 *
 * @param m represents the main structure.
 * @param line_index_a represents the line index of the set A.
 * @param line_index_b represents the line index of the set B.
 */
bool subseteq(Main *m, int line_index_a, int line_index_b)
{
    int cardinality_a = m->s->l[line_index_a].cardinality;
    int cardinality_b = m->s->l[line_index_b].cardinality;
    bool element_found;
    for (int i = 0; i < cardinality_a; i++)
    {
        element_found = false;
        for (int j = 0; j < cardinality_b; j++)
        {
            if (m->s->l[line_index_a].set_items[i] == m->s->l[line_index_b].set_items[j])
            {
                element_found = true;
                break;
            }
        }
        if (!element_found)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Function which compares two sets and returns whether set A is a proper subset of set B.
 *
 * @param m represents the main structure.
 * @param line_index_a represents the line index of the set A.
 * @param line_index_b represents the line index of the set B.
 * @see subseteq()
 */
 
int subset(Main *m, int line_index_a, int line_index_b)
{

    int cardinality_a = m->s->l[line_index_a].cardinality;
    int cardinality_b = m->s->l[line_index_b].cardinality;

    if (subseteq(m, line_index_a, line_index_b) && (cardinality_a < cardinality_b))
    {
        printf("true\n");
        return true;
    }

    printf("false\n");
    return false;
}
/**
 * @brief Function which compares two sets and returns whether they are equal or not..
 *
 * @param m represents the main structure.
 * @param line_index_a represents the line index of the set A.
 * @param line_index_b represents the line index of the set B.
 */

void equals(Main *m, int line_index_a, int line_index_b)
{
    // mame podmnoziny X,Y -> ak X je podmnozinou Y  a Y je podmnozinou X => X=Y
    if (subseteq(m, line_index_a, line_index_b) && subseteq(m, line_index_b, line_index_a))
        printf("true\n");
    else
        printf("false\n");
}

//*************************************************************//
//************************ R E L A C E ************************//
//*************************************************************//

/**
 * @brief Function which checks whether a relation is symmetric. Returns a True or False value.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */
void symmetric(Main *m, int line_index){
    int rel_cardinality = m->r->l[line_index].cardinality;
    int x, y, x2, y2;
    bool symmetric;
    //R (a a) (b b) (b c) (c b)
    for (int i = 0; i < rel_cardinality; i++){
        symmetric = false;
        for(int j = 0; j < rel_cardinality; j++){
            x = m->r->l[line_index].p[i].first;
            y = m->r->l[line_index].p[i].second;
            x2 = m->r->l[line_index].p[j].first;
            y2 = m->r->l[line_index].p[j].second;
            if (x == y2 && y == x2){
                symmetric = true;
            }
        }
        if(!symmetric){
            printf("false\n");
            break;
        }
    }
    if(symmetric){
        printf("true\n");
    }
}

/**
 * @brief Function which checks whether a relation is a function. Returns a True or False value.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */
bool is_function(Main *m, int line_index_a)
{
    int rel_cardinality = m->r->l[line_index_a].cardinality;
    for (int i = 0; i < rel_cardinality; i++){
        for (int j = i+1; j < rel_cardinality; j++){
            if(m->r->l[line_index_a].p[i].first == m->r->l[line_index_a].p[j].first)
                return false;
        }
    }
    return true;
}

/**
 * @brief Function which checks whether a relation is reflexive. Returns a True or False value.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */

void is_reflexive(Main *m, int line_index_a){
    int universum_cardinality = m->u->universum_cardinality;
    int rel_cardinality = m->r->l[line_index_a].cardinality;
    int reflex_count = 0;

    for (int i = 0; i < rel_cardinality; i++){
        if(m->r->l[line_index_a].p[i].first == m->r->l[line_index_a].p[i].second){
            reflex_count++;
        }
    }
    if(reflex_count != universum_cardinality){
        printf("false\n");
    }
    else{
        printf("true\n");
    }
}

/**
 * @brief Function which checks whether a relation is asymmetric. Returns a True or False value.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */

void is_asymmetric(Main *m, int line_index){

    int relation_cardinality = m->r->l[line_index].cardinality;
    bool asymetric = true;
    int x,y,x2,y2;
    for(int i = 0; i < relation_cardinality; i++){

        for(int j = 0; j < relation_cardinality; j++){
            x = m->r->l[line_index].p[i].first; // c
            y = m->r->l[line_index].p[i].second; // a
            x2 = m->r->l[line_index].p[j].first; // a
            y2 = m->r->l[line_index].p[j].second; // a
            asymetric = true;
            if ((x == y2 && y == x2) && x != y){
                asymetric = false;
                break;
            }
        }
        if(!asymetric){
            printf("false\n");
            break;
        }
    }
    if(asymetric){
        printf("true\n");
    }
}
/**
 * @brief Function which checks whether a relation is a transitive. Returns a True or False value.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */

bool transitive(Main *m, int line_index_a){
    int i,j,k;
    int relation_cardinality = m->r->l[line_index_a].cardinality;
    bool transitive;
    int first_x, first_y, second_x, second_y, third_x, third_y;
    for(i = 0; i < relation_cardinality; i++){
        first_x = m->r->l[line_index_a].p[i].first;
        first_y = m->r->l[line_index_a].p[i].second;
        for(j = 0; j< relation_cardinality; j++){
            second_x = m->r->l[line_index_a].p[j].first;
            second_y = m->r->l[line_index_a].p[j].second;
            transitive = false;
            if(first_y == second_x){
                for(k = 0; k < relation_cardinality; k++){
                     third_x = m->r->l[line_index_a].p[k].first;
                     third_y = m->r->l[line_index_a].p[k].second;
                     if(first_x == third_x && second_y == third_y){
                         transitive = true;
                     }
                }
                if(!transitive){
                    printf("false\n");
                    return false;
                }
            }
        }
    }
    printf("true\n");
    return true;
}
/**
 * @brief Function which which prints out the range/codomain of a relation.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */
void codomain(Main *m, int line_index_a){
    int i, j;
    int current_element;
    int index = 0;
    int cardinality = m->r->l[line_index_a].cardinality;
    int *obor_hodnot = malloc(sizeof(int) * cardinality);
    bool shoda;

    for (i = 0; i < cardinality; i++){
        shoda = false;
        current_element = m->r->l[line_index_a].p[i].second;
        for (j = 0; j < index; j++){

            if (obor_hodnot[j] == current_element){
                shoda = true;
                break;
            }
        }
        if (shoda == false){
            obor_hodnot[index] = current_element;
            index++;
            continue;
        }
    }
    print_set(m , index, obor_hodnot);
    free(obor_hodnot);
}
/**
 * @brief Function which which prints out the domain of a relation.
 *
 * @param m represents the main structure
 * @param line_index represents the line index of the relation being worked on.
 */
void domain(Main *m, int line_index_a){
    int i, j;
    int current_element;
    int index = 0;
    int cardinality = m->r->l[line_index_a].cardinality;
    int *rel_domain = malloc(sizeof(int) * cardinality);
    bool shoda;

    for (i = 0; i < cardinality; i++){
        shoda = false;
        current_element = m->r->l[line_index_a].p[i].first;
        for (j = 0; j < index; j++){

            if (rel_domain[j] == current_element){
                shoda = true;
                break;
            }
        }
        if (shoda == false){
            rel_domain[index] = current_element;
            index++;
            continue;
        }
    }
    print_set(m, index, rel_domain);

    free(rel_domain);
}
/**
 * @brief Function which checks relation.
 *
 * @param m represents the main structure
 * @param rel_line_index represents the line index of the relation being worked on.
 * @param set_A_line_index
 * @param set_B_line_index
 *
 */
int injective(Main *m, int rel_line_index, int set_A_line_index, int set_B_line_index){

    int rel_cardinality = m->r->l[rel_line_index].cardinality;
    int set_A_cardinality = m->s->l[set_A_line_index].cardinality;
    int set_B_cardinality = m->s->l[set_B_line_index].cardinality;
    bool set_A_check = true;
    int count = 0;

    if(!is_function(m, rel_line_index)){
        return false;
    }


    if((set_A_cardinality == 0 || set_B_cardinality == 0) && rel_cardinality != 0){
        return false;
    }
    for(int i = 0; i< rel_cardinality; i++){
        for( int j = 0; j<set_B_cardinality; j++){
            if(m->s->l[set_B_line_index].set_items[i] == m->r->l[rel_line_index].p[j].first){
                return false;
            }
        }
    }
    for(int i = 0; i< rel_cardinality; i++){
        for( int j = 0; j<set_A_cardinality; j++){
            if(m->s->l[set_A_line_index].set_items[i] == m->r->l[rel_line_index].p[j].second){
                return false;
            }
        }
    }
    for(int i = 0; i < set_A_cardinality; i++){
        set_A_check = false;
        for (int j = 0; j < rel_cardinality; j++)
            if(m->s->l[set_A_line_index].set_items[i] == m->r->l[rel_line_index].p[j].first){
                set_A_check = true;
            }
        if(!set_A_check){
            return false;
        }

    }
    for(int i = 0; i < set_B_cardinality; i++){
        count = 0;
        for(int j = 0; j < rel_cardinality; j++){
            if(m->s->l[set_B_line_index].set_items[i] == m->r->l[rel_line_index].p[j].second)
                count++;
        }
        if (count > 1){
            return false;
        }

    }
    return true;
}

/**
 * @brief Function which checks relation.
 *
 * @param m represents the main structure
 * @param rel_line_index represents the line index of the relation being worked on.
 * @param set_A_line_index
 * @param set_B_line_index
 *
 */
/*
U a b c x y z w 1
S a b c 2 <= A
S x y z 3 <= B
S x y 4
S x y z w 5
S 6
R (x y) (a x) (b z) (c y) 7
R (a x) (b a) (c z) 8 
R (a x) (b y) 9 
R (a x) (a y) (b y) (c z) 10
R 11
R (a x) (b y) (c x) 12
R (a x) (b y) (c z) 13 <=
C surjective 7 2 3 false f
C surjective 8 2 3 false f
C surjective 9 2 3 false f
C surjective 10 2 3 false t
C surjective 11 2 3 false f
C surjective 11 6 6 true t
C surjective 13 2 6 false f
C surjective 13 6 3 false f
C surjective 13 6 6 false t
C surjective 12 2 3 fales f
C surjective 12 2 4 true f
C surjective 13 2 3 true f
C surjective 13 2 5 false f
*/
int surjective(Main *m, int rel_line_index, int set_A_line_index, int set_B_line_index){
    int rel_cardinality = m->r->l[rel_line_index].cardinality;
    int set_B_cardinality = m->s->l[set_B_line_index].cardinality;
    int set_A_cardinality = m->s->l[set_A_line_index].cardinality;
    bool found_B_element = true;

    if(!is_function(m, rel_line_index)){
        return false;
    }

    if((set_A_cardinality == 0 || set_B_cardinality == 0) && rel_cardinality != 0){
        return false;
    }

    for(int i = 0; i< rel_cardinality; i++){
        for( int j = 0; j<set_A_cardinality; j++){
            if(m->s->l[set_A_line_index].set_items[j] == m->r->l[rel_line_index].p[i].second){
                return false;
            }
        }
    }
    // first element in pair does not contain any B elements.
    for(int i = 0; i< rel_cardinality; i++){
        for( int j = 0; j<set_B_cardinality; j++){
            if(m->s->l[set_B_line_index].set_items[j] == m->r->l[rel_line_index].p[i].first){
                return false;
            }
        }
    }

    // checks if all second elements are in B
    for(int i = 0; i < set_B_cardinality; i++){
        found_B_element = false;
        for(int j = 0; j < rel_cardinality; j++){
            if(m->s->l[set_B_line_index].set_items[i] == m->r->l[rel_line_index].p[j].second){
                found_B_element = true;
            }
        }
        if(!found_B_element){
            return false;
        }
    }
    return true;
}

int bijective(Main *m, int rel_line_index, int set_A_line_index, int set_B_line_index){
    if(injective(m,rel_line_index,set_A_line_index,set_B_line_index) && surjective(m,rel_line_index,set_A_line_index,set_B_line_index)){
        printf("true\n");
        return true;
    }
    printf("false\n");
    return false;
}


/**
 * @brief 
 * 
 * @param m 
 * @param func_index 
 * @param par1 
 * @param par2 
 * @param par3 
 * @return int 
 */
int function_call(Main *m, int func_index, int par1, int par2, int par3){
    int rel_1 = rel_find_index(m, par1);
    int set_1 = set_find_index(m,par1);
    int set_2 = set_find_index(m,par2);
    int set_3 = set_find_index(m,par3);
    if (func_index <= 2 && set_1 == -1){
        fprintf(stderr, "ERROR: wrong command argument!\n");
        return -1;
    }
    if ((func_index >= 3 && func_index <= 9) && rel_1 == -1){
        fprintf(stderr, "ERROR: wrong command argument!\n");
        return -1;
    }
    if((func_index >= 10 && func_index <= 15) && (set_1 == -1 || set_2 == -1)){
        fprintf(stderr, "ERROR: wrong command argument!\n");
        return -1;
    }
    if((func_index >= 16 && func_index <= 18) && (rel_1 == -1 || set_2 == -1 || set_3 == -1)){
        fprintf(stderr, "ERROR: wrong command argument!\n");
        return -1;
    }
    switch(func_index) {
        case 0 :
            if (set_2 || set_3 != 0)
            is_empty(m,set_1);
            break;
        case 1 :
            printf("%d\n",m->s->l[set_1].cardinality);
            break;
        case 2 :
            if(set_1 == -1){
                return -1;
            }
            do_complement(m,set_1);
            break;
        case 3 :
            is_reflexive(m,rel_1);
            break;
        case 4 :
            symmetric(m,rel_1);
            break;
        case 5 :
            is_asymmetric(m,rel_1);
            break;
        case 6 :
            transitive(m,rel_1);
            break;
        case 7 :
            if(is_function(m,rel_1)){
                printf("true\n");
            }
            else{
                printf("false\n");
            }
            break;
        case 8 :
            domain(m,rel_1);
            break;
        case 9 :
            codomain(m,rel_1);
            break;
        case 10 :
            do_union(m,set_1,set_2);
            break;
        case 11 :
            intersect(m,set_1,set_2);
            break;
        case 12 :
            minus(m,set_1,set_2);
            break;
        case 13 :
            if(subseteq(m,set_1,set_2))
                printf("true\n");
            else
                printf("false\n");
            break;
        case 14 :
            subset(m,set_1,set_2);
            break;
        case 15 :
            equals(m,set_1,set_2);
            break;
        case 16 :
            if(injective(m, rel_1, set_2, set_3)){
                printf("true\n");
            }
            else{
                printf("false\n");
            }
            break;
        case 17 :
            if(surjective(m,rel_1, set_2, set_3)){
                printf("true\n");
            }
            else{
                printf("false\n");
            }
            break;
        case 18 :
            bijective(m,rel_1,set_2,set_3);
            break;
        default :
            printf("bad func name\n");
    }
    return true;
}

/**
 * @brief 
 * 
 * @param file 
 * @param savednum 
 * @param character 
 * @return int 
 */
int parse_num(FILE *file, int *savednum, int *character){
    char *ptr;
    int index = 0;
    char number[5];
    strcpy(number,"");
    while(*character != '\n' && *character != EOF){
            number[index++] = *character;
            *character = getc(file);
            //loads number
            if(*character == ' ' || *character == '\n' || *character == EOF){
                number[index] = '\0';
                *savednum = strtol(number,&ptr,10);
                return 1;
            }
    }
    return 0;
}

int function_parser(FILE *file, Main *m){
    char temp[31];
    int index = 0;
    int character = fgetc(file);
    temp[index] = character;
    int func_num = 0;
    int firstnum = 0, secondnum = 0, thirdnum = 0;


    while (character != ' ' && character != '\n'){
        temp[index++] = character;
        character = fgetc(file);
        if (character == ' ')
        {
            temp[index] = '\0';
            func_num = find_function_keyword(temp,1);
            if (func_num == -1){
                fprintf(stderr,"ERROR: Wrong function keyword\n");
                return -1;
                }
        }
            if(character == '\n'){
                fprintf(stderr,"ERROR: no function input given.\n");
                return -1;
            }
    }

    int num_of_parsed = 0;
    while(character != '\n' && character != EOF){
        character = fgetc(file);
        num_of_parsed +=parse_num(file,&firstnum,&character);
        num_of_parsed +=parse_num(file,&secondnum,&character);
        num_of_parsed +=parse_num(file,&thirdnum,&character);
    }
    if((func_num <= 9 && num_of_parsed != 1) || (func_num > 9 && func_num <= 15 && num_of_parsed != 2) || (func_num > 15 && num_of_parsed != 3)){
        fprintf(stderr,"ERROR: Function has a wrong number of parameters\n");
        return -1;
    }

    if(function_call(m, func_num, firstnum, secondnum, thirdnum) == -1)
        return -1;
    return 0;
}

/**
 * @brief Function which prints out the entire universum.
 *
 * @param m points to struct main
 */
void print_universum(Main *m)
{
    printf("U");
    for (int i = 0; i < m->u->universum_cardinality; i++)
    {
        printf(" ");
        printf("%s", m->u->elements[i].element);
        if (i == m->u->universum_cardinality - 1)
            break;
    }
    printf("\n");
}

void print_set_by_line_cardinality(Main *m){

    int uni_index = 0;
    int line_cardinality = m->s->line_cardinality;

    printf("S");
    for (int i = 0; i < m->s->l[line_cardinality].cardinality; i++)
    {   
        printf(" ");
        uni_index = m->s->l[line_cardinality].set_items[i];
        printf("%s", m->u->elements[uni_index].element);
        if (i == m->s->l[line_cardinality].cardinality - 1)
            break;
    }
    printf("\n");
}
/**
 * @brief Function which prints out the set.
 *
 * @param m represents the main structure.
 * @param cardinality represents how many elements does the set have.
 * @param set points to set which is supposed to be printed out.
 */
void print_set(Main *m, int cardinality, int *set){
    printf("S");
    for (int i = 0; i < cardinality; i++){
        printf(" ");
        printf("%s", m->u->elements[set[i]].element);
        if(i == cardinality-1)
            break;
    }
    printf("\n");
}

/**
 * @brief Function which prints out the relation
 *
 * @param m points to main structure. 
 */

void print_relation(Main *m){
    int rel_index = 0;
    int line_cardinality = m->r->line_cardinality;
    if(m->r->l[line_cardinality].cardinality == 0)
        printf("R");
    else
        printf("R ");
    for (int i = 0; i < m->r->l[line_cardinality].cardinality; i++){
        rel_index = m->r->l[line_cardinality].p[i].first; // first pair

        printf("(%s ", m->u->elements[rel_index].element);

        rel_index = m->r->l[line_cardinality].p[i].second; //second pair

        printf("%s)", m->u->elements[rel_index].element);
        if (i == m->r->l[line_cardinality].cardinality - 1)
            break;
        printf(" ");
    }
    printf("\n");
}
/**
 * @brief initializes relation in struct Main or reallocates memory for it if needed
 *
 * @param m represents the main structure.
 * @param line_index represents line at which relation can be found in file
 */

int relation_line_add(Main *m, int line_index){
    int line_cardinality = m->r->line_cardinality;

    if (m->r->line_cardinality + 1 > m->r->line_capacity)
    {
        m->r->line_capacity = m->r->line_capacity * 2 + 1;
        m->r->l = allocate_or_resize(m->r->l, m->r->line_capacity * sizeof(Relation_line));
        if (m->r->l == NULL){
            fprintf(stderr, "Memory could not be allocated\n");
            return -1;
        }
    }
    if(line_cardinality == 0){
        free(m->r->l[line_cardinality].p);
    }
    m->r->l[line_cardinality].line_index = line_index;
    m->r->l[line_cardinality].p = NULL;
    m->r->l[line_cardinality].capacity = 0;
    m->r->l[line_cardinality].cardinality = 0;

    return true;
}

/**
 * @brief Checks for line capacity and if needed reallocs the memory for number of lines.
 * 
 * @param m points to struct Main
 * @param line_index line index at which set can be found in file
 * @return int 
 */
int set_line_add(Main *m, int line_index){
    int line_cardinality = m->s->line_cardinality;
    if (m->s->line_cardinality + 1 > m->s->line_capacity){
        m->s->line_capacity = m->s->line_capacity * 2 + 1;
        m->s->l = allocate_or_resize(m->s->l, m->s->line_capacity * sizeof(Set_line));
        if (m->s->l == NULL){
            fprintf(stderr, "Memory could not be allocated\n");
            return -1;
        }
    }
    if(line_cardinality == 0){
        free(m->s->l[line_cardinality].set_items);
    }
    m->s->l[line_cardinality].set_items = NULL;
    m->s->l[line_cardinality].line_index = line_index;
    m->s->l[line_cardinality].capacity = 0;
    m->s->l[line_cardinality].cardinality = 0;
    return true;
}

int main(int argc, char *argv[]){
    FILE *file;
    int return_value;
    bool error = false;
    bool uni_loaded = false;
    bool set_found = false;
    bool relation_found = false;
    if (argc > 2 || argc == 1){
        fprintf(stderr, "ERROR: Only one input is allowed\n");
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    int line_index = 1;
    file = fopen(filename, "r");

    if (file == NULL){
        fprintf(stderr, "ERROR: This file name is invalid\n");
        return -1;
    }

    Main *m = main_ctor();
    if (m == NULL){
        fclose(file);
        main_dtor(m,3);
        return EXIT_FAILURE;
    }

    while (1){
        if(line_index > 1000){
            fprintf(stderr,"ERROR: too many lines!");
            error = 1;
            break;
        }
        return_value = type_check(file);
        if (return_value == 2){
            if(load_universum(file, m, line_index) == -1){
                error = 1;
                break;
            }
            if(is_universum_valid(m) == -1){
                error = 1;
                break;
            }
            uni_loaded = true;
            print_universum(m);
        }
        if(return_value == 7){
            uni_add_element(m, "", -1);
            print_universum(m);
            set_line_add(m, line_index);
            set_add_element(m, -1, 0);
            (m->s->line_cardinality)++;
            uni_loaded = true;

        }
        if (return_value == 3){
            set_line_add(m, line_index);
            if(set_to_index(file, m) == -1){
                error = 1;
                break;
            }

            if(is_set(m) == -1){
                error = 1;
                break;
            }
            print_set_by_line_cardinality(m);
            set_found = true;
            (m->s->line_cardinality)++;
        }
        if (return_value == 8){
            set_line_add(m,line_index);
            set_add_element(m, -1, 0);
            print_set_by_line_cardinality(m);
            (m->s->line_cardinality)++;
            set_found = true;
        }
        if (return_value == 4){
            relation_line_add(m, line_index);
            if((relation_to_index(file, m)) == -1){
                error = 1;
                break;
            }
            if(is_relation(m) == -1){
                error = 1;
                break;
            }
            print_relation(m);
            relation_found = true;
            (m->r->line_cardinality)++;
        }
        if(return_value == 9){
            relation_line_add(m,line_index);
            relation_add_element(m,-1,true,0);
            print_relation(m);
            relation_found = true;
            (m->r->line_cardinality)++;
        }
        if (return_value == 5){
            if((function_parser(file, m)) == -1){
                error = 1;
                break;
            }
            break;
        }
        if (!return_value){
            break;
        }
        if(return_value == -1){
            fprintf(stderr, "ERROR: Syntax error.\n");
            error = 1;
            break;
        }
        if(return_value != 5)
            line_index++;
        if(!uni_loaded){
            fprintf(stderr, "ERROR: Universum not defined!\n");
            error = 1;
            break;
        }
    }
    if(!set_found && !relation_found){
        fprintf(stderr,"ERROR: missing sets/relations");
        main_dtor(m, 3);
        fclose(file);
        return -1;
    }
    while(!error){
        return_value = type_check(file);
        if (!return_value){
            break;
        }
        if(return_value != 5){
            fprintf(stderr,"ERROR: expected COMMAND\n");
            error = 1;
            break;
        }
        if((function_parser(file, m)) == -1){
                error = 1;
                break;
        }
    }
    main_dtor(m, 3);
    fclose(file);
    if(error){
        return EXIT_FAILURE;
    }
    return 0;
    main_dtor(m, 3);
    fclose(file);
    if(error){
        return EXIT_FAILURE;
    }
    return 0;
}