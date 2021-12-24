// apt-get install sshpass
// apt-get install sshfs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define TRUE    1
#define FALSE   0

#define ZID_LENGTH	8
#define PASS_LENGTH	100

int clone_folder(char *folder_name);
int push(char *folder_name);
int pull(char *folder_name);
int link_to_unsw(char *file_name);
int unlink_from_unsw(char *file_name, int delete);
int send_command(int argc, char *argv[]);
char *get_comp_courses();
char *generate_link_command(char *file_name, char *path);
char *generate_unlink_command(char *file_name);
