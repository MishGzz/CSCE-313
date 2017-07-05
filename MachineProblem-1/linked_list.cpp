//Mishael Gonzalez
//MP1
//CSCE 313 - 100
//UIN: 725007329
//due June 6th
#include "linked_list.h"

node* head_ptr;
char* free_ptr;

int mem_size;
int block_size;
int num_blocks;
	
void Init(int M, int C) { // innitialize the list and use malloc to allocate memory (size M bytes)
	cout << "Initializing list" << endl << endl;
	mem_size = M;
	block_size = C;
	
	head_ptr = (node*)malloc(mem_size);
	free_ptr = (char*)head_ptr;


}

void Destroy() { //use free to return memory to the system after destroying the list
	cout << endl << "Destroying list" << endl;
	head_ptr = NULL; //tried using nullptr but did not work on lenss
	free_ptr = NULL;
	//free up the pointers
	mem_size = 0;
	block_size = 0;
	num_blocks = 0;

	free(head_ptr);
}

int Insert(int x, char* value_ptr, int value_len) { //use memcpy() to copy the value;
	cout << "Inserting node with key:" << x << endl;
	//num_blocks must be <= mem_size / block_size
	if (num_blocks >= mem_size / block_size) {
		//Error
		cout << "Not enough room for node with key: " << x << endl << endl;
		return -1;
	}
	else{
		//next_ptr is 8 bytes
		//key is 4 bytes
		//val_len is 4 bytes
		if (value_len > block_size - (sizeof(free_ptr) + sizeof(x) + sizeof(value_len))) {
			//Error
			cout << "There was an error inserting node with key: " << x << endl;
			cout << "node is too large." << endl << endl;
			return -1;
		}
		else{

			node* new_node = (node*)free_ptr;
			new_node->key = x;
			new_node->next_ptr = (node*)(free_ptr + block_size);
			new_node->val_len = value_len;
			memcpy((sizeof(node) + free_ptr), value_ptr, value_len);//use memcpy() to copy 
			free_ptr = free_ptr + block_size;							//the value
			num_blocks++;
			
			cout << "Node with key " << x << " has been inserted." << endl << endl;
			return x;
		}
	}
	
}

void Delete(int x) {
	cout << "Deleteing node with key:" << x << endl;
	//keep track of the current node and the previous node
	node* curr = head_ptr;
	node* prev = NULL;

	bool node_deleted = false;

	for (int i = 1; i <= num_blocks; i++) {
		if (curr->key == x && !node_deleted) {
			if (prev != NULL) {
				prev->next_ptr = curr->next_ptr;
			}
			else {
				head_ptr = curr->next_ptr;
			}
			num_blocks -= 1; // node has now been deleted
			node_deleted = true;
		}
		prev = curr;
		curr = curr->next_ptr;
	}

	if (node_deleted) {
		cout << "Key " << x << " has been deleted." << endl << endl;
	}
	else {
		//key not found
		cout << "Key not found." << endl << endl;
	}

}

char* Lookup(int x) {
	cout << "Looking up key: " << x << endl;
	node* curr = head_ptr;

	for (int i = 1; i <= num_blocks; i++) {
		if (curr->key == x) {//node has been found
			char* position = (char*)curr + sizeof(node*);
			return position;
		}
		curr = curr->next_ptr;
	}
	cout << "Node with key: " << x << " not found" << endl << endl;
	return NULL; //node not found
}

void PrintList() {
	cout << "Printing list" << endl;
	node* curr = head_ptr;
	for (int i = 1; i <= num_blocks; i++) {
		cout << "Node # " << i << endl;
		cout << "Key: " << curr->key << endl;
		cout << "Value Length: " << curr->val_len << endl << endl;

		curr = curr->next_ptr;
	}

}