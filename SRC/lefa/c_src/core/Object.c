/*	object.c
	
	Lithe Engine Object 
	
	SildTeam

*/


#include"core/object.h"


int32_t UObject_IsA(UObject* obj, TypeID target_type) 
{
    int i = 0;
    if(!obj)
	{
		return 0;
	}
	else
	{
		for(i = 0; i < obj->hierarchy_depth; i++) 
		{
			if(obj->hierarchy[i] == target_type) 
			{
				return 1; /* Verdadeiro: Pertence a essa classe ou ancestral */
			}
		}
	}
    
    return 0; /* Falso */
}














