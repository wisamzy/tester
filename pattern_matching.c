#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pattern_matching.h"


unsigned int id=0;

/**
 * @brief creat a new pm_state struct and init it and returns pointer
 * 
 * @return pm_state_t* 
 */
pm_state_t* newState() {

    pm_state_t* _state =(pm_state_t*) malloc(sizeof(pm_state_t));
    if(_state == NULL)
        return NULL;

    _state->id=0;
    _state->depth=0;
    _state->output=(dbllist_t*)malloc(sizeof(dbllist_t));
    dbllist_init(_state->output);
    _state->_transitions=NULL;
    _state->fail=NULL;
    return _state;

}
/**
 * @brief takes two lists and con them together
 * 
 * @param s 
 * @param fs 
 * @return int 
 */
int uni(dbllist_t* s, dbllist_t* fs){
    if(s == NULL || fs == NULL)
        return -1;
    dbllist_node_t* n = dbllist_head(fs);
    while(n != NULL) {
        if(dbllist_append(s, dbllist_data(n))==-1)
            return -1;
        n = dbllist_next(n);
    }
    return 0;
}

int pm_init(pm_t *pat) {
    pat->newstate =1;
    pat->zerostate =newState();
    if(pat->zerostate==NULL || pat == NULL || pat->zerostate->output == NULL )
        return -1;
    return 0;
}
int pm_addstring(pm_t *pm, unsigned char *string, _size_t n) {
    if (pm == NULL || pm->zerostate == NULL || string == NULL || n <= 0) {
        return -1;
    }

    pm_state_t* state = pm->zerostate;
    for (int i = 0; i < n; i++) {
        pm_state_t* next_state = pm_goto_get(state, string[i]);
        if (next_state == NULL) {
            next_state = newState();
            if (next_state == NULL) {
                return -1;
            }
            pm_goto_set(state, string[i], next_state);
            pm->newstate++;
        }
        state = next_state;
    }
    if (dbllist_append(state->output, (void*)(string)) == -1) {
        return -1;
    }

    return 0;
}
pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol) {
    
    if(state == NULL || state->_transitions == NULL){

        return NULL;
        
    }

    dbllist_node_t* cur;

    for(cur=dbllist_head(state->_transitions);cur!=NULL;cur=dbllist_next(cur)){
        
        pm_labeled_edge_t* curr_edg = (pm_labeled_edge_t*)(dbllist_data(cur));
        
        if(curr_edg->label==symbol){
            
            return curr_edg->state;

        }
    }
    return NULL;
}

int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state) {

    pm_labeled_edge_t *newEdge = (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));

    if(newEdge==NULL)
        return -1;
    newEdge->label=symbol;
    newEdge->state=to_state;

    if(from_state->_transitions == NULL){
        from_state->_transitions = (dbllist_t*) malloc(sizeof(dbllist_t));
        dbllist_init(from_state->_transitions);
    }

    if(dbllist_append(from_state->_transitions,newEdge)==-1)
        return -1;

    to_state->depth=from_state->depth+1;
    to_state->id = ++id;
    printf("Allocating state %d\n",to_state->id);
    printf("%d ->%c ->%d\n",from_state->id ,newEdge->label ,to_state->id);
    return 0;
}
int pm_makeFSM(pm_t *pm) {
    if (pm == NULL) {
        return -1;
    }

    dbllist_t* queue = malloc(sizeof(dbllist_t));
    if (queue == NULL) {
        return -1;
    }
    dbllist_init(queue);

    dbllist_node_t* _node = dbllist_head(pm->zerostate->_transitions);
    while (_node != NULL) {
        pm_labeled_edge_t* edge = (pm_labeled_edge_t*)(_node->data);
        edge->state->fail = pm->zerostate;
        if (dbllist_append(queue, edge->state) == -1) {
            dbllist_destroy(queue, DBLLIST_LEAVE_DATA);
            return -1;
        }
        _node = dbllist_next(_node);
    }

    dbllist_node_t* nq = dbllist_head(queue);

    while (nq != NULL) {
        pm_state_t* _state_ = (pm_state_t*)(dbllist_data(nq));
        if (_state_->_transitions == NULL) {
            nq = dbllist_next(nq);
            continue;
        }
        dbllist_node_t* tmp = dbllist_head(_state_->_transitions);
        while (tmp != NULL) {
            pm_labeled_edge_t* edge = (pm_labeled_edge_t*)(dbllist_data(tmp));
            unsigned char symbol = edge->label;
            pm_state_t* s = pm_goto_get(_state_, symbol);
            if (s == NULL) {
                tmp = dbllist_next(tmp);
                continue;
            }
            if (dbllist_append(queue, s) == -1) {
                dbllist_destroy(queue, DBLLIST_LEAVE_DATA);
                return -1;
            }
            pm_state_t* fstate = _state_->fail;
            while ((pm_goto_get(fstate, symbol)) == NULL && fstate != pm->zerostate) {
                fstate = fstate->fail;
            }
            s->fail = pm_goto_get(fstate, symbol);
            if (s->fail == NULL) {
                s->fail = pm->zerostate;
            }
            uni(s->output, s->fail->output);
            printf("Setting f(%d) = %d\n",s->id,s->fail->id);
            tmp = dbllist_next(tmp);
        }
        nq = dbllist_next(nq);
    }
    dbllist_destroy(queue, DBLLIST_LEAVE_DATA);
    free(queue);
    return 0;
}

dbllist_t* pm_fsm_search(pm_state_t *_state, unsigned char *string, _size_t n) {

    dbllist_t* match = malloc(sizeof(dbllist_t));
    dbllist_init(match);
    pm_state_t* tmp = _state;
    for (int i = 0; i < n; i++)
    {
        while ((pm_goto_get(tmp,string[i]))==NULL)
        {
            if(tmp == NULL)
                break;
            tmp=tmp->fail;
        }
        tmp=pm_goto_get(tmp,string[i]);
        if(tmp == NULL) {
            tmp = _state;
            continue;
        }
        if(dbllist_size(tmp->output) > 0) {
            if (tmp->output == NULL)
                break;
            for (dbllist_node_t *n_ = dbllist_head(tmp->output); n_ != NULL; n_ = dbllist_next(n_)) {
                pm_match_t *nMatch = malloc(sizeof(pm_match_t));
                nMatch->pattern = (char *) (dbllist_data(n_));
                nMatch->start_pos = i - (int) strlen(nMatch->pattern) + 1;
                nMatch->end_pos = i;
                nMatch->fstate = tmp;
                if (dbllist_append(match, (void *) (nMatch)) == -1)
                    return NULL;
                printf("pattern : %s , starts at: %d , at : %d , last state: %d\n", nMatch->pattern, nMatch->start_pos, nMatch->end_pos, nMatch->fstate->id);
            }
        }
    }
    return match;
}
/**
 * @brief free each state and it's output and it's transitions
 * 
 * @param _state 
 */
 void free_state(pm_state_t* _state) {
    dbllist_destroy(_state->output, DBLLIST_LEAVE_DATA);
    free(_state->output);
    dbllist_destroy(_state->_transitions, DBLLIST_FREE_DATA);
    free(_state->_transitions);
    free(_state);
}
/**
 * @brief a help function for the pm_destroy function to reach all the states and send it to the free state function
 * 
 * @param _state 
 */
void de_allocate(pm_state_t* _state)  {

    if(_state->_transitions ==  NULL){
        free_state(_state);
        return;
    }
    dbllist_node_t* _node = dbllist_head(_state->_transitions);
    while(_node!=NULL){
        de_allocate(((pm_labeled_edge_t*)(dbllist_data(_node)))->state);
        _node = dbllist_next(_node);
    }
    free_state(_state);
}

void pm_destroy(pm_t *pm) {
    if(pm ==NULL)
        return;
    de_allocate(pm->zerostate);
}
