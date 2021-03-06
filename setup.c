#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

#include "unsw.h"

int clear_current_files();
int download_dependencies();
char *get_zid();
char *get_zpass();
int check_zid(char *zid);
int check_info(char *zid, char *pass);
int create_login_info_file(char *zid, char *pass);
int compile_and_clean();
int append_path();
int move();

int main(int argc, char *argv[]) {
	
    clear_current_files();
    download_dependencies();

    char *zid = get_zid();
    char *pass = get_zpass();

    // check pass + zid

    create_login_info_file(zid, pass);
    compile_and_clean();
    append_path();
    move();
}

int download_dependencies() {
	// apt-get install sshpass
	// apt-get install sshfs
	// apt-get install make

	system("sudo apt-get install sshpass");
	system("sudo apt-get install sshfs");
	system("sudo apt-get install make");

	return 0;
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

// currently not working :(
int check_zid(char *zid) {
	return TRUE;
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

    fclose(input_stream); 

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
    return 0;
}

int append_path() {

	printf("Appeneding path to .profile\n");

	char path_line[] = "\nexport PATH=$PATH:/opt/connect/";
	
	system("echo 'export PATH=$PATH:/opt/connect/' >> ~/.profile");

	return 1;
}

int move() {
	
	char cwd[PATH_MAX];
	memset(cwd, '\0', PATH_MAX);
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }
	
	// Checks that the current folder is /connect. This is done so that the following 
    // the following commands arent run in a random folder. Could be bad otherwise.
	//char check[100] = {'\0'};
	//for (int i = strlen(cwd); cwd[i] != '/'; i--) {
	//	check[i] = cwd[i];
	//}
	
	//if (strcmp(check, "connect") != 0) {
	//	return 1;
	//}
	
    strcat(cwd, "/");

	struct stat st = {0};
    
    // create /opt/connect but if it exists already, then clear out old files
	if (stat("/opt/connect", &st) != -1) {
		printf("Removing old files\n");
		int remove_status = system("sudo rm -rf /opt/connect");
	}
	
	pid_t pid;
    extern char** environ;

	// Move files into correct location
	//int move_status = system("sudo mv * /opt/connect/");

	char* sudo_mv_argv[] = { "/bin/sudo", "mv", cwd, "/opt/", NULL };

    if (posix_spawn(&pid, "/bin/sudo", NULL, NULL, sudo_mv_argv, environ) != 0) {
        perror("spawn");
        exit(1);
        return 1;
    }

    // wait for spawned processes to finish
    int exit_status;
    if (waitpid(pid, &exit_status, 0) == -1) {
        perror("waitpid");
        exit(1);
        return 1;
    }
	
	return 0;
}


int clear_current_files() {
    system("sudo rm -rf /opt/connect");
    return 0;
}
