#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#include "unsw.h"

char *get_zid();
char *get_zpass();
int check_zid(char *zid);
int check_info(char *zid, char *pass);
int create_login_info_file(char *zid, char *pass);
int compile_and_clean();
int move();

int main() {

	char *zid = get_zid();
	char *pass = get_zpass();
	
	// check pass + zid

	create_login_info_file(zid, pass);
	compile_and_clean();
}

// Returns a string from the user containing their ZID
char *get_zid() {
	
	printf("Please enter your ZID: ");
	
	// +1 for the NULL character at the end
	char *zid = malloc(ZID_LENGTH + 1);
	memset(zid, '\0', ZID_LENGTH + 1);

	for (int i = 0; i < ZID_LENGTH; i++) {
		scanf("%c", &zid[i]);
	}
	
	if (check_zid(zid) == FALSE) {
		printf("ZID format is incorrect\n");
		return NULL;
	}

	return zid;
}

// Returns a string from the user containing their ZPASS
// Must be a max of 100 characters
char *get_zpass() {
	printf("Please enter your ZPASS: ");  
	
	// +1 for the NULL character at the end
	char *pass = malloc(PASS_LENGTH);
	memset(pass, '\0', PASS_LENGTH);
	
	scanf("%s", pass);
	
	return pass;
}

int check_zid(char *zid) {
	if (strlen(zid) != ZID_LENGTH) {
		return FALSE;
	}

	if (zid[0] != 'z') {
		return FALSE;
	}
	
	// Check the other 7 characters are numbers
	for (int i = 0; i < ZID_LENGTH - 1; i++) {
		if (isdigit(zid[i]) == FALSE) {
			return FALSE;
		}
	}

	return TRUE;
}

int check_info(char *zid, char *pass) {
	return TRUE;	
}

int create_login_info_file(char *zid, char *pass) {
    
	FILE *input_stream = fopen("login_info.h", "w");
    if (input_stream == NULL) {
        perror("login_info.h");  // prints why the open failed
        return 1;
    }

	//#define ZID "z5555555"
	//#define ZPASS   ""
	
	char new_file[100] = {'\0'};
	
	strcat(new_file, "#define ZID \"");
	strcat(new_file, zid);
	strcat(new_file, "\"");
	strcat(new_file, "\n#define ZPASS \"");
	strcat(new_file, pass);
	strcat(new_file, "\"");

	for (int i = 0; new_file[i] != '\0'; i++) {
		fputc(new_file[i], input_stream);
	}

    fclose(input_stream);  // optional as close occurs

    return 0;
}

int compile_and_clean() {
	
	int return_make = system("make unsw");
	
	if (return_make != 0) {
		printf("There was an error compiling the program.\n");
		printf("Please wait and then run ./setup.c again\n");
	}
	
	if (remove("login_info.h") != 0) {
        printf("There was an error in removing login_info.h\n");
		printf("Please remove it manually to avoid security risks.\n");
		return 1;
	}
	
	printf("Successfully compiled and removed sensitive files\n");
	system("rm login_info.h");
	printf("Removing non-compiled files.\n");

}

int move() {
	struct stat st = {0};

	if (stat("/opt/connect", &st) == -1) {
		int mkdir_status = mkdir("/opt/connect", 0700);
	} else {
		int remove_status = system("sudo rm -rf /opt/connect/*");
	}
	
	int move_status = system("sudo mv -a * /opt/connect/");

	return 0;
}
