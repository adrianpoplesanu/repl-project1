#ifndef __OBJECTS_H
#define __OBJECTS_H

enum Ad_Object_Type {
	NULL_OBJ,
	INT_OBJ,
	BOOL_OBJ,
	STRING_OBJ,
	RETURN_VALUE_OBJ
};

class Ad_Object {
	Ad_Object_Type type;
	unsigned int num_octets;
	unsigned int ref_count;
};

void Ad_INCREF(Ad_Object*);
void Ad_DECREF(Ad_Object*);

#endif