/*	object.h
	
	Lithe Engine Object 
	
	SildTeam

*/

#include<stdio.h>
#include<stdint.h>

#ifndef MAX_HIERARCHY_DEPTH
	#define MAX_HIERARCHY_DEPTH 4
#endif /* MAX_HIERARCHY_DEPTH */

typedef unsigned long TypeID;


#define TYPE_UOBJECT       0x10000000
typedef struct UObject_ 
{
    TypeID type_id;                         /* ID do tipo exato desta instância */
    TypeID hierarchy[MAX_HIERARCHY_DEPTH];  /* Herança completa */
    unsigned char hierarchy_depth;          /* Nível na árvore */
    unsigned long flags;
    unsigned long instance_id;
} UObject;



