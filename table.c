#include "table.h"
#include "memory.h"
#include <string.h>

#define TABLE_MAX_LOAD 0.75f

void initTable(Table *table) {
	table->entries = NULL;
	table->count = 0;
	table->size = 0;
}

void freeTable(Table *table) {
	FREE_ARRAY(Entry, table->entries, table->size);
	initTable(table);
}

static Entry *findEntry(Entry *entries, int size, ObjString *key) {
	int i = key->hash % size;
	Entry *tompStone = NULL;
	while(entries[i].key != NULL || (entries[i].key == NULL && IS_BOOL(entries[i].value))) {
		if(entries[i].key == key) return entries + i;
		if(IS_BOOL(entries[i].value) && entries[i].key == NULL) {
			tompStone = entries+i;
		}
		i = (i + 1) % size;
	}
	return tompStone != NULL ? tompStone : entries + i; // NULL KEY or tompstone if found
}

void tableAddAll(Table *from, Table *to) {
	for(int i=0;i<from->size;i++) {
		if(from->entries[i].key != NULL) {
			tableSet(to, from->entries[i].key, from->entries[i].value);
		}
	}
}

bool tableGet(Table *table, ObjString *key, Value *value) {
	if(table->count == 0) return false;
	Entry *entry = findEntry(table->entries, table->size, key);
	if(entry->key == NULL) return false;
	*value = entry->value;
	return true;
}

bool tableDelete(Table *table, ObjString *key) {
	if(table->count == 0) return false;
	Entry *entry = findEntry(table->entries, table->size, key);
	if(entry->key == NULL) return false;
	entry->key = NULL;
	entry->value = BOOL_VAL(true);
	return true;
}


static void adjustSize(Table *table, int newSize) {
	Table newTable;
	initTable(&newTable);
	newTable.size = newSize;
	newTable.entries = ALLOCATE(Entry, newSize);
	for(int i=0;i<newSize;i++) {
		newTable.entries[i].key = NULL;
		newTable.entries[i].value = NIL_VAL;
	}
	for(int i=0;i<table->size;i++) {
		Entry *entry = &table->entries[i];
		if(entry->key != NULL) {
			Entry *newEntry = findEntry(table->entries, newSize, entry->key);
			newEntry->key = entry->key;
			newEntry->value = entry->value;
			newTable.count++;
		}
	}
	freeTable(table);
	table->size = newTable.size;
	table->entries = newTable.entries;
	table->count = newTable.count;
}

ObjString *tableFindString(Table *strings, char *chars, int length, uint32_t hash) {
	if(strings->count == 0) return NULL;
	uint32_t i = hash % strings->size;
	while(1) {
		Entry *entry = &strings->entries[i];
		if(entry->key == NULL) {
			if(IS_NIL(entry->value)) return NULL;
		} else if(entry->key->length == length && entry->key->hash == hash) {
			if(!memcmp(entry->key->chars, chars, length)) return entry->key;
		}
		i = (i+1) % strings->size;
	}
}


bool tableSet(Table *table, ObjString *key, Value value) {
	if(table->count >= (int)((float)table->size * TABLE_MAX_LOAD)) {
		int size = GROW_CAPACITY(table->size);
		adjustSize(table, size);
	}
	Entry *entry = findEntry(table->entries, table->size, key);
	bool isNewKey = entry->key == NULL;
	if(isNewKey && IS_NIL(entry->value)) table->count++;
	entry->key = key;
	entry->value = value;
	return isNewKey;
}
