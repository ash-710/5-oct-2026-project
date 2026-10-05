#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Get a string from the user
char *input() {	
	// cursor represent current position when going over text; size represents total size of string
	size_t cursor = 0;
	size_t size = 16;

	char *str = malloc(size);
	if (str == NULL) {return NULL;}

	// add all characters until newline to the string, increasing chars as needed, append \0, and return
	int c;
	while ((c = getchar()) != EOF && c != '\n') {
		if (cursor+1 == size) {
			size += 16;

			char *tmp = realloc(str, size);
			if (tmp == NULL) {
				free(str); 
				return NULL;
			}
			str = tmp;
		}
		str[cursor] = c;
		cursor++;
	}
	// Mark empty EOF as en error to fix Ctrl+D
	if (cursor == 0 && c == EOF) {
		free(str);
		return NULL;
	}
	str[cursor]='\0';
	return str;
}


// Main process; Defined as a function to improve readability
// Returns 0 if process completes; 1 if user wants to exit
int process(void **list) {
	void **original_arr = list;
	printf("\nCommand: ");

	// Check if command has been entered without errors
	char *command = input();
	if (command == NULL) {
		if (feof(stdin) || ferror(stdin)) {
			// Prevent infinite loop due to EOF
			free(command);
			return 1;
		}
		printf("Error occurred.\n");
		return 0;
	}
	int cmd_len = strlen(command);
	if (cmd_len == 0) {
		free(command);
		return 0;
	}


	if (strcmp(command,"EXIT") == 0) {
		free(command);
		return 1;
	}


	// cmd1, cmd2, and cmd3 are obtained by splitting command along spaces
	char *cmd1 = malloc(cmd_len+1);
	char *cmd2 = malloc(cmd_len+1);
	char *cmd3 = malloc(cmd_len+1);
	if (cmd3 == NULL || cmd2 == NULL || cmd1 == NULL) {
		free(command);
		free(cmd1);
		free(cmd2);
		free(cmd3);
		printf("Error occurred.\n");
		return 0;
	}

	// Iteration variable; Command lengths
	int j = 1;
	int k1 = 0;
	int k2 = 0;
	int k3 = 0;

	for (int i=0; i<cmd_len; i++) {
		if (command[i] != ' ') {
			if (j==1) {
				cmd1[k1] = command[i];
				k1++;
			}
			if (j==2) {
				cmd2[k2] = command[i];
				k2++;
			}
			if (j==3) {
				cmd3[k3] = command[i];
				k3++;
			}
		}
		if (command[i] == ' ') {
			j++;

			// No command can have more than three pieces
			if (j>3) {
				printf("Incorrect command.\n");
				free(command);
				free(cmd1);
				free(cmd2);
				free(cmd3);
				return 0;
			}
		}
	}
	// Ensure that strings end with \0
	cmd1[k1]='\0';
	cmd2[k2]='\0';
	cmd3[k3]='\0';


	// Handling GET <KEY> and EXISTS <KEY>
	if (strcmp(cmd1, "GET") == 0 || strcmp(cmd1, "EXISTS") == 0) {
		// Ensure that the input contains the correct arguments 
		if (k3 != 0 || k2 == 0) {
			printf("Incorrect command.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// If list contains no key, database is empty
		if (list[0] == NULL) {
			printf("Empty database.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Iterate over all nodes of list
		while (list != NULL && list[2] != NULL) {
			if (list[0] != NULL && strcmp(list[0], cmd2) == 0) {
				if (strcmp(cmd1, "GET") == 0) {
					printf("%s\n",list[1]);
				} else {
					printf("Exists.\n");
				}
				free(command);
				free(cmd1);
				free(cmd2);
				free(cmd3);
				return 0;
			}
			if (list != NULL) {
				list = list[2];
			}
		}
		if (list[0] != NULL && strcmp(list[0], cmd2) == 0) {
			if (strcmp(cmd1, "GET") == 0) {
				printf("%s\n",list[1]);
			} else {
				printf("Exists.\n");
			}
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		printf("Value not found.\n");
		free(command);
		free(cmd1);
		free(cmd2);
		free(cmd3);
		return 0;
	}


	// Handling SET <KEY> <VALUE>
	if (strcmp(cmd1, "SET") == 0) {
		// Ensure that the input contains the key; empty value is permitted
		if (k2 == 0) {
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Check if database is empty
		if (list[0] == NULL) {
			list[0] = cmd2;
			list[1] = cmd3;
			free(command);
			free(cmd1);
			return 0;
		}

		// Check if list already contains key
		while (list[2] != NULL) {
			if (list[0] != NULL && strcmp(list[0], cmd2) == 0) {
				printf("Erasing %s.\n", list[1]);
				free(list[1]);
				list[1] = cmd3;
				free(command);
				free(cmd1);
				free(cmd2);
				return 0;
			}

			list = list[2];
		}

		// Check if last node of list contains key
		if (list[0] != NULL && strcmp(list[0], cmd2) == 0) {
			printf("Erasing %s.\n", list[1]);
			free(list[1]);
			list[1] = cmd3;
			free(command);
			free(cmd1);
			free(cmd2);
			return 0;
		}

		// Create a new node
		list[2] = malloc(3 * sizeof(void *));
		if (list[2] == NULL) {
			printf("Error occurred.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}
		
		// Populate node; do not free cmd2 to prevent use after free
		list = list[2];
		list[0] = cmd2;
		list[1] = cmd3;
		list[2] = NULL;
		free(command);
		free(cmd1);
		return 0;
	}


	// Handling DEL <KEY>
	if (strcmp(cmd1, "DEL") == 0) {
		// Ensure that the input contains the correct arguments
		if (k2 == 0 || k3 != 0) {
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Check if database is empty
		if (list[0] == NULL) {
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}
		
		// Iterate over all nodes and check if a node has the corresponding key
		void **parent = list;
		do {
			if (list[0] != NULL && strcmp(list[0], cmd2) == 0) {
				void **next = list[2];
				free(list[0]);
				free(list[1]);
				if (next != NULL) {
					list[0] = next[0];
					list[1] = next[1];
					list[2] = next[2];
				} else {
					// Set them to NULL in case parent == list and next node doesn't exist
					list[0] = NULL;
					list[1] = NULL;

					parent[2] = NULL;

					if (parent != list) {
						free(list);
						list = NULL;
					}
				}
				free(next);
			}
			parent = list;
			if (list != NULL) {
				list = list[2];
			}
		} while (list != NULL);

		free(command);
		free(cmd1);
		free(cmd2);
		free(cmd3);
		return 0;
	}

	// Handling SAVE <FILE>
	if (strcmp(cmd1, "SAVE") == 0) {
		// Ensure that arguments are as specified
		if (k2 == 0 || k3 != 0) {
			printf("Incorrect command.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Open file
		FILE *ptr = NULL;
		ptr = fopen(cmd2, "w");
		if (ptr == NULL) {
			printf("Error occurred.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}
		
		// Check if database exists
		if (list[0] == NULL) {
			printf("Database does not exist.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			fclose(ptr);
			return 0;
		}

		// Iterate over list to save to the file
		do {
			fputs(list[0], ptr);
			fputs(" ", ptr);
			fputs(list[1], ptr);
			fputs("\n", ptr);
			list = list[2];
		} while (list != NULL);

		printf("Saved database.\n");
		free(command);
		free(cmd1);
		free(cmd2);
		free(cmd3);
		fclose(ptr);
		return 0;
	}


	// Handling LOAD <FILE>
	if (strcmp(cmd1, "LOAD") == 0) {
		// Ensure arguments are as specified
		if (k2 == 0 || k3 != 0) {
			printf("Incorrect command.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Open file
		FILE *ptr = NULL;
		ptr = fopen(cmd2, "r");
		if (ptr == NULL) {
			printf("Error occurred.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			return 0;
		}

		// Clear DB; Ensure that original array doesn't get erased
		void **tmp = NULL;
		void **arr = original_arr;
		tmp = arr[2];
		free(arr[0]);
		free(arr[1]);
		arr = tmp;
		while (arr != NULL) {
			tmp = arr[2];
			free(arr[0]);
			free(arr[1]);
			free(arr);
			arr=tmp;
		}
		original_arr[0] = NULL;
		original_arr[1] = NULL;
		original_arr[2] = NULL;

		// Read file
		size_t size=16;
		size_t cur=0;
		char *str=malloc(size);
		if (str == NULL) {
			printf("Error occurred.\n");
			free(command);
			free(cmd1);
			free(cmd2);
			free(cmd3);
			fclose(ptr);
			return 0;
		}
		int c;
		list = original_arr;
		
		// Loop over all characters unless EOF is encountered; if EOF is encountered, dump existing contents
		while ((c = fgetc(ptr)) != EOF || (cur!=0 && feof(ptr)) {
			// Extend allocated memory if more is needed
			if (cur == size - 1) {
				size += 16;
				char *tmp_ptr = str;
				str = realloc(str, size);
				if (str == NULL) {
					printf("Error occurred.\n");
					void **tmp = NULL;
					void **arr = original_arr;
					tmp = arr[2];
					free(arr[0]);
					free(arr[1]);
					arr=tmp;
					while (arr != NULL) {
						tmp = arr[2];
						free(arr[0]);
						free(arr[1]);
						free(arr);
						arr=tmp;
					}
					free(str);
					free(tmp_ptr);
					free(command);
					free(cmd1);
					free(cmd2);
					free(cmd3);
					original_arr[0] = NULL;
					original_arr[1] = NULL;
					original_arr[2] = NULL;
					fclose(ptr);
					return 0;
				}
			}

			// If character is not a newline or EOF, add it to str and shift cur
			if (c != '\n' && c != EOF) {
				str[cur] = c;
				cur++;
			}
			// If newline or EOF is detected, go over line and add it to db
			if (c == '\n' || c == EOF) {
				// Variables for iteration and string length
				int j = 1;
				int k4 = 0;
				int k5 = 0;
				char *key = malloc(size);
				char *value = malloc(size);
				if (key == NULL || value == NULL) {
					printf("Error occurred.\n");
					void **tmp = NULL;
					void **arr = original_arr;
					tmp = arr[2];
					free(arr[0]);
					free(arr[1]);
					arr=tmp;
					while (arr != NULL) {
						tmp = arr[2];
						free(arr[0]);
						free(arr[1]);
						free(arr);
						arr=tmp;
					}
					free(key);
					free(value);
					free(str);
					free(command);
					free(cmd1);
					free(cmd2);
					free(cmd3);
					original_arr[0] = NULL;
					original_arr[1] = NULL;
					original_arr[2] = NULL;
					fclose(ptr);
					return 0;
				}

				// Iterate over each element recorded in string so far
				for (int i = 0; i < cur; i++) {
					// Spaces separate entires in a line
					if (str[i] == ' ') {
						j++;
						// Check if a line has 3 entries
						if (j>2) {
							printf("Error occurred.\n");
							void **tmp = NULL;
							void **arr = original_arr;
							tmp = arr[2];
							free(arr[0]);
							free(arr[1]);
							arr=tmp;
							while (arr != NULL) {
								tmp = arr[2];
								free(arr[0]);
								free(arr[1]);
								free(arr);
								arr=tmp;
							}
							free(key);
							free(value);
							free(str);
							free(command);
							free(cmd1);
							free(cmd2);
							free(cmd3);
							original_arr[0] = NULL;
							original_arr[1] = NULL;
							original_arr[2] = NULL;
							fclose(ptr);
							return 0;
						}
					}

					// If j is 1, update key; else, update value
					if (j == 1 && str[i] != ' ') {
						key[k4] = str[i];
						k4++;
					}
					if (j == 2 && str[i] != ' ') {
						value[k5] = str[i];
						k5++;
					}
				}

				// Add \0 to end of key and value
				key[k4] = '\0';
				value[k5] = '\0';

				// Reset cur when reading file
				cur = 0;
				
				// Check if key is a duplicate
				void **dup_check = original_arr;
				int dup = 0;
				while (dup_check != NULL) {
					if (dup_check[0] != NULL && strcmp(dup_check[0], key) == 0) {
						dup++;
						printf("Duplicate key detected.\n");
					}
					dup_check = dup_check[2];
				}
				

				// Update DB if k4 is not 0 and key is not a duplicate
				if (list[0] == NULL && k4 != 0 && dup == 0) {
					list[0] = key;
					list[1] = value;
				} else if (k4 != 0 && dup == 0) {
					list[2] = malloc(3*sizeof(void *));
					if (list[2] == NULL) {
						printf("Error occurred.\nClearing database.\n");
						void **tmp = NULL;
						void **arr = original_arr;
						tmp = arr[2];
						free(arr[0]);
						free(arr[1]);
						arr = tmp;
						while (arr != NULL) {
							tmp = arr[2];
							free(arr[0]);
							free(arr[1]);
							free(arr);
							arr=tmp;
						}
						free(command);
						free(cmd1);
						free(cmd2);
						free(cmd3);
						free(key);
						free(value);
						free(str);
						original_arr[0] = NULL;
						original_arr[1] = NULL;
						original_arr[2] = NULL;
						fclose(ptr);
						return 0;
					}
					list = list[2];
					list[0] = key;
					list[1] = value;
					list[2] = NULL;
				} else {
					free(key);
					free(value);
				}
			}
		 }

		// Close file
		fclose(ptr);
		free(command);
		free(cmd1);
		free(cmd2);
		free(cmd3);
		free(str);
		return 0;
	}

	printf("Incorrect command.\n");
	free(command);
	free(cmd1);
	free(cmd2);
	free(cmd3);
	return 0;
}


int main(void) {
	printf("DATABASE");

	// Initialize array
	void **arr = malloc(3 * sizeof(void *));
	if (arr == NULL) {
		printf("Error occurred.\n");
		return 1;
	}
	arr[0] = NULL;
	arr[1] = NULL;
	arr[2] = NULL;
	
	// exit is set to gracefully exit the loop
	int exit = 0;

	while (!exit) {
		exit = process(arr);
	}


	// Free memory and return
	void **tmp = NULL;
	while (arr != NULL) {
		tmp = arr[2];
		free(arr[0]);
		free(arr[1]);
		free(arr);
		arr=tmp;
	}
	return 0;
}
