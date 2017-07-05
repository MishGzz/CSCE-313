#pragma once
//Mishael Gonzalez
//MP1
//CSCE 313 - 100
//UIN: 725007329
//due June 6th
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstring>

using namespace std;
class linked_list;

//node
class node {
public:
	node* next_ptr;
	int key;
	int val_len;
};

void Init(int M, int C);// obtains M bytes of memory by calling the standard library
//function malloc() and initializes the linked list.

void Destroy();
// destroys the linked list and returns memory to the system by calling free()

int Insert(int x, char* value_ptr, int value_len);
//inserts a key-val pair where the key is an int , x and the val is some data pointed to
//by the pointer value_ptr of length value_len. Use memcpy() to copy the value

void Delete(int x);
//Deletes the first item from the beginning with the key x

char* Lookup(int x);
//Finds the given key in a list and returns a pointer to the first occurrence of that key

void PrintList();
//prints all the items key-value pairs sequentially starting from the head of the list. 
//Print only the key and the value-length. dont print the actual value as it could contain
//binary data