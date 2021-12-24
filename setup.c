#include <stdio.h>
#include <string.h>

#include "unsw.h"

int check_zid(char *zid);
int check_info(char *zid, char *pass);
int create_login_info_file(char *zid, char *pass);
int compile_and_clean();

int main() {

	printf("Please enter your ZID: ");
	// +1 for the NULL character at the end
	char zid[ZID_LENGTH + 1] = {'\0'};
	for (int i = 0; i < ZID_LENGTH; i++) {
		scanf("%c", &zid[i]);
	}
	
	if (check_zid(zid) == FALSE) {
		printf("ZID format is incorrect\n");
		return 1;
	}

	printf("Please enter your ZPASS: ");  
	// +1 for the NULL character at the end
	char pass[PASS_LENGTH + 1] = {'\0'};
	scanf("%s", pass);

	if (check_info(zid, pass) == FALSE) {
		printf("Incorrect login info\n");
		printf("Aborting\n");
		return 1;
	}

	create_login_info_file(zid, pass);

}

int check_zid(char *zid) {
	return TRUE;
	if (strlen(zid) != ZID_LENGTH) {
		return FALSE;
	}

	if (zid[0] != 'z') {
		return FALSE;
	}
	
	// Check the other 7 characters are numbers

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
	strcat(new_file, "\n#define PASS \"");
	strcat(new_file, pass);
	strcat(new_file, "\"");

	for (int i = 0; new_file[i] != '\0'; i++) {
		fputc(new_file[i], input_stream);
	}

    fclose(input_stream);  // optional as close occurs

    return 0;
}

int compile_and_clean() {

}
