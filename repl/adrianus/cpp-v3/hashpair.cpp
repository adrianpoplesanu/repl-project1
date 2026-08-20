#include "hashpair.h"

HashPair::HashPair() {
	//key = NULL;
	//value = NULL;
}

HashPair::HashPair(Ad_Object* k, Ad_Object* v) {
	key = k;
	value = v;
}

HashPair::~HashPair() {
	// TODO: implement this
}

Ad_Object* HashPair::GetKey() {
	return key;
}

Ad_Object* HashPair::GetValue() {
	return value;
}