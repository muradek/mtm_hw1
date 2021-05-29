//
// Created by Yousef  Abuakell on 29/05/2021.
//

//
//  map.c
//  hw12021
//
//  Created by Yousef  Abuakell on 02/05/2021.
//

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define EMPTY_POINTER -1



typedef struct node{
    MapDataElement DataElement;
    MapKeyElement KeyElement;
    struct node* next_node;
}*Node;


struct Map_t {
    copyMapDataElements CopyDataElement;
    copyMapKeyElements CopyKeyElement;
    freeMapDataElements FreeDataElement;
    freeMapKeyElements FreeKeyElement;
    compareMapKeyElements CompareKeyElements;
    int maximum_size;
    int current_index;
    Node iterator;
    Node first_node;
};
Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if( !copyDataElement || !copyKeyElement || !freeKeyElement || !freeDataElement || !compareKeyElements){
        return  NULL;
    }
    Map map = malloc(sizeof(*map));
    if(map == NULL){
        return NULL;
    }



    map->current_index = 0;
    map->CopyDataElement = copyDataElement;
    map->CopyKeyElement = copyKeyElement;
    map->FreeKeyElement = freeKeyElement;
    map->FreeDataElement = freeDataElement;
    map->CompareKeyElements = compareKeyElements;
    map-> maximum_size = 0;
    map->iterator = NULL;
    map->first_node = NULL;
    return map;
}
static Node nodeCreate()
{
    Node created_node= malloc(sizeof(*created_node));
    if (!created_node)
    {
        return NULL;
    }
    created_node->KeyElement=NULL;
    created_node->DataElement=NULL;
    created_node->next_node=NULL;

    return created_node;
}
void mapDestroy(Map map){
    if(map == NULL){
        return ;
    }
    mapClear(map);
    free(map);
}
Map mapCopy(Map map){
    if(!map)
        return NULL;
    Map map_copy=mapCreate(map->CopyDataElement,map->CopyKeyElement,map->FreeDataElement,map->FreeKeyElement,map->CompareKeyElements);
    if(!map_copy){
        return NULL;
    }
    Node current_node = map->first_node;
    while(current_node != NULL)
    {
        mapPut(map_copy, map->CopyKeyElement(current_node->KeyElement), map->CopyDataElement(current_node->DataElement));
        current_node=current_node->next_node;
    }
    map->iterator = NULL;
    return map_copy;
}

int mapGetSize(Map map){
    if(map==NULL){
        return EMPTY_POINTER;
    }
    Node current = map->first_node;
    if(map->first_node==NULL){
        return 0;
    }
    int i = 0;
    while(current){
        i++;
        current=current->next_node;
    }
    return (i);

}
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){// check if it
    if(map == NULL|| keyElement == NULL|| dataElement==NULL){
        return MAP_NULL_ARGUMENT;
    }
    Node current_node = map->first_node;
    if(mapContains(map, keyElement)){// if the key is already in the map
        while(current_node){
            if(map->CompareKeyElements(keyElement,current_node->KeyElement)==0){
                map->FreeDataElement(current_node->DataElement);
                current_node->DataElement = map->CopyDataElement(dataElement);; // we should use copyDataelement
                return MAP_SUCCESS;
            }
            current_node=current_node->next_node;
        }
    }
    Node new_node = nodeCreate(); // allocating the node and its elements.
    MapDataElement newDataElement = map->CopyDataElement(dataElement);
    MapKeyElement newKeyElement = map->CopyKeyElement(keyElement);
    if( newDataElement == NULL || newKeyElement == NULL || new_node==NULL){
        free(newDataElement);
        free(newKeyElement);
        free(new_node);
        return MAP_OUT_OF_MEMORY;
    }
    new_node->DataElement =newDataElement;
    new_node->KeyElement = newKeyElement;
    // if the map is empty
    if(map->first_node == NULL){
        map->first_node = new_node;

        return MAP_SUCCESS;
    }

    // if we want to insert a key as the head
    if (map->CompareKeyElements(keyElement,map->first_node->KeyElement)<0)
    {
        new_node->next_node= map->first_node;
        map->first_node=new_node;

        return MAP_SUCCESS;
    }
    // if we want to insert the key in the end of the list.
    Node node_iterator = map->first_node;
    while(node_iterator->next_node!=NULL){
        node_iterator=node_iterator->next_node;
    }
    if(map->CompareKeyElements(keyElement,node_iterator->KeyElement)>0){
        node_iterator->next_node=new_node;
        return MAP_SUCCESS;
    }
    // if its in the middle
    node_iterator=map->first_node;
    while(map->CompareKeyElements(keyElement,node_iterator->next_node->KeyElement)>0){
        node_iterator=node_iterator->next_node;
    }
    new_node->next_node=node_iterator->next_node;
    node_iterator->next_node=new_node;
    map->current_index++;
    map->iterator = NULL;
    return MAP_SUCCESS;
}
bool mapContains(Map map, MapKeyElement element){
    if(map == NULL || element == NULL){
        return false;
    }
    Node current_node = map->first_node;
    while(current_node){
        if(map->CompareKeyElements(current_node->KeyElement,element)== 0){
            map->iterator=NULL;
            return true;
        }
        current_node=current_node->next_node;
    }
    map->iterator=NULL;
    return false;
}
MapResult mapClear(Map map){
    if(map == NULL){
        return MAP_NULL_ARGUMENT;
    }
    if(map->first_node == NULL){//if the map is already clean
        return MAP_SUCCESS;
    }
    Node current_node = map->first_node;
    while(current_node){
        Node toDelete= current_node;
        current_node= current_node->next_node;
        map->FreeDataElement(toDelete->DataElement);
        map->FreeKeyElement(toDelete->KeyElement);
        free(toDelete);
    }
    map->first_node=current_node;
    return MAP_SUCCESS;
}
MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(map == NULL || keyElement == NULL){
        return MAP_NULL_ARGUMENT;
    }
    Node current_node = map->first_node; // if the key we want to remove is the first in the linked list
    if(map->CompareKeyElements(keyElement,map->first_node->KeyElement)==0){
        map->FreeKeyElement(current_node->KeyElement);
        map->FreeDataElement(current_node->DataElement);
        map->first_node=current_node->next_node;
        free(current_node);
        return MAP_SUCCESS;
    }
    // if the key is in the middle
    while(current_node->next_node){
        if(map->CompareKeyElements(keyElement,current_node->next_node->KeyElement)==0 && current_node->next_node->next_node){
            Node to_remove = current_node->next_node;
            current_node->next_node = current_node->next_node->next_node;
            to_remove->next_node =NULL;
            map->FreeKeyElement(current_node->next_node->KeyElement);
            map->FreeDataElement(current_node->next_node->DataElement);
            return MAP_SUCCESS;
        }
        current_node=current_node->next_node;
    }
    //if the key is the last one in the linked list

        if(map->CompareKeyElements(keyElement,current_node->KeyElement)==0){
            map->FreeKeyElement(current_node->KeyElement);
            map->FreeDataElement(current_node->DataElement);
            free(current_node);
            return MAP_SUCCESS;
        }
    map->iterator=NULL;

    return MAP_ITEM_DOES_NOT_EXIST;
}
MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(map == NULL || keyElement == NULL){
        return NULL;
    }
    Node current_node = map->first_node;
    while(current_node){
        if(map->CompareKeyElements(current_node->KeyElement,keyElement)==0){
            return current_node->DataElement;
        }
        current_node=current_node->next_node;
    }
    map->iterator=NULL;
    return NULL;
}
MapKeyElement mapGetFirst(Map map){
    if (map==NULL || map->first_node==NULL)
    {
        return NULL;
    }
    map->iterator = map->first_node;
    return map->CopyKeyElement(map->iterator->KeyElement);
}
MapKeyElement mapGetNext(Map map){
    if (map==NULL || map->iterator==NULL || map->iterator->next_node==NULL)
    {
        return NULL;
    }
    map->iterator=map->iterator->next_node;
    return map->CopyKeyElement(map->iterator->KeyElement);
}



