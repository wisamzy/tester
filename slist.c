#include "slist.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief this function creat a new node.
 * 
 * @param data 
 * @return dbllist_node_t* 
 */
dbllist_node_t* newNode(void* data) {
    dbllist_node_t* n = malloc(sizeof(dbllist_node_t));
    if(n==NULL)
       return NULL;
       
    dbllist_data(n) = data;
    dbllist_next(n) = NULL;
    dbllist_prev(n) = NULL;
    return n;
} 
void dbllist_init(dbllist_t *list) {
    dbllist_head(list) =NULL;
    dbllist_tail(list) =NULL;
    dbllist_size(list) =0;
}
int dbllist_append(dbllist_t *list,void *data) {
    if(!list)
        return -1;
    dbllist_node_t* n  = newNode(data);
    if(!n) {
        return -1;
    }
    dbllist_prev(n)=dbllist_tail(list);
    dbllist_next(n)=NULL;
    if(dbllist_tail(list) != NULL) {
        dbllist_next(dbllist_tail(list))=n;
    }if(dbllist_head(list) == NULL)
        dbllist_head(list)=n;
    dbllist_tail(list)=n;
    dbllist_size(list)++;
    return 0;
}
int dbllist_prepend(dbllist_t *list,void *data) {
    dbllist_node_t* n=newNode(data);
    if(!n) {
        return -1;
    }
    if(dbllist_size(list) == 0) {
        dbllist_head(list) =n;
        dbllist_tail(list) =n;
    }
    else {
        dbllist_next(n) =dbllist_head(list);
        dbllist_prev(dbllist_head(list)) = n;
        dbllist_head(list)=n;
    }
        dbllist_size(list)++;
        return 0;
}
int dbllist_remove(dbllist_t *list, dbllist_node_t* n ,dbllist_destroy_t flag) {
    if(list == NULL || n == NULL)
        return -1;
    if(flag == DBLLIST_FREE_DATA) {
        free(dbllist_data(n));        
    }
    if(n == dbllist_head(list)){
        dbllist_head(list)=dbllist_next(dbllist_head(list));
        dbllist_prev(dbllist_head(list))=NULL;
        free(n);
    }
    else if(n == dbllist_tail(list)) {
        dbllist_tail(list)=dbllist_prev(dbllist_tail(list));
        dbllist_next(dbllist_tail(list))=NULL;
        free(n);
    }
    else {
        dbllist_prev(dbllist_next(n))=dbllist_prev(n);
        dbllist_next(dbllist_prev(n))=dbllist_next(n);
        free(n);
    }
    dbllist_size(list)--;
    return 0;
}
void dbllist_destroy(dbllist_t *list,dbllist_destroy_t flag) {

        if(list == NULL){
            return;
        }
        dbllist_node_t* curr = dbllist_head(list);

        for(int i = 0 ; i < dbllist_size(list) ; i++){

            dbllist_node_t* n = dbllist_next(curr);

            if(flag==DBLLIST_FREE_DATA){
                free(dbllist_data(curr));
            }
            free(curr);
            curr = n;
        }
}


