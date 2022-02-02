// John Henderson (z5368143)

#include "unsw.h"
#include "login_info.h"
#include <stdio.h>

// Allows the user to clone a folder from their unsw remote home directory to
// their local machine. Uses rsync to download the directory to the users local.
int clone_folder(char* folder_name) {

    pid_t pid;
    extern char** environ;

    // Length of the totoal command, all init to '\0'
    char command[67 + strlen(folder_name)];
    memset(command, '\0', 67 + strlen(folder_name));

    strcat(command, ZID);
    strcat(command, "@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/");
    strcat(command, ZID);
    strcat(command, "/");
    strcat(command, folder_name);

    // rsync -avW -e ssh z5368143@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/z5368143/COMP1531 ./tmp
    char* rsync_argv[] = { "/bin/sshpass", "-p", ZPASS, "rsync", "-avW", "-q", "--delete", "-e", "ssh", command, "./", NULL };

    if (posix_spawn(&pid, "/bin/sshpass", NULL, NULL, rsync_argv, environ) != 0) {
        perror("spawn");
        exit(1);
        printf("An error has occured when trying to clone <%s>\n", folder_name);
        printf("This most likely coccured because of a typo or the file may not exist\n");
        return 1;
    }

    // wait for spawned processes to finish
    int exit_status;
    if (waitpid(pid, &exit_status, 0) == -1) {
        perror("waitpid");
        exit(1);
        return 1;
    }

    printf("successfully cloned <%s>\n", folder_name);

    return 0;
}

// Reflects the changes the user has made on their local directory to the remote
// directory.
int push(char* folder_name) {

    // Checks if the user has given a command line argument or not. If the
    // folder name is null, then it gets the folder name from current path
    char cwd[PATH_MAX];
    if (folder_name == NULL) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return 1;
        }
        
        // Gets current working directory and then takes the last directory
        int not_finished = TRUE;
        for (int i = strlen(cwd) - 1; i >= 0 && not_finished; i--) {
            if (cwd[i] == '/') {
                folder_name = &cwd[i];
                not_finished = FALSE;
            }
        }
    }

    printf("Found folder name <%s>\n", folder_name);
    pid_t pid;
    extern char** environ;

    // Length of the total command, all init to '\0'
    char command[67 + strlen(folder_name)];
    memset(command, '\0', 67 + strlen(folder_name));

    strcat(command, ZID);
    strcat(command, "@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/");
    strcat(command, ZID);
    strcat(command, "/");
    strcat(command, folder_name);
    
    // sync current folder
    //  rsync -avW -e ssh z5368143@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/ZID/FOLDER .
    char* rsync_argv[] = { "/bin/sshpass", "-p", ZPASS, "rsync", "-a", "--delete", ".", command, NULL };

    // spawn "/bin/date" as a separate process
    if (posix_spawn(&pid, "/bin/sshpass", NULL, NULL, rsync_argv, environ) != 0) {
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

    printf("successfully pushed <%s>\n", folder_name);
    return 0;
}

// Reflects the changes the user has made on their remote unsw directory to their
// local directory.
int pull(char* folder_name) {

    // if folder name is null, then it extracts the currect folder name from thier paths
    char cwd[PATH_MAX];
    if (folder_name == NULL) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return 1;
        }

        int not_finished = TRUE;
        for (int i = strlen(cwd) - 1; i >= 0 && not_finished; i--) {
            if (cwd[i] == '/') {
                folder_name = &cwd[i + 1];
                not_finished = FALSE;
            }
        }
    }

    printf("Found folder name <%s>\n", folder_name);

    pid_t pid;
    extern char** environ;

    // Creates correct remote path
    // Length of the totoal command, all init to '\0'
    char command[67 + strlen(folder_name)];
    memset(command, '\0', 67 + strlen(folder_name));

    strcat(command, ZID);
    strcat(command, "@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/");
    strcat(command, ZID);
    strcat(command, "/");
    strcat(command, folder_name);
    strcat(command, "/");

    //  rsync -avW -e ssh z5368143@login.cse.unsw.edu.au:/tmp_amd/reed/export/reed/4/ZID/FOLDER .
    char* rsync_argv[] = { "/bin/sshpass", "-p", ZPASS, "rsync", "-a", "--delete", command, ".", NULL };

    // spawn "/bin/date" as a separate process
    if (posix_spawn(&pid, "/bin/sshpass", NULL, NULL, rsync_argv, environ) != 0) {
        perror("spawn");
        exit(1);
    }

    // wait for spawned processes to finish
    int exit_status;
    if (waitpid(pid, &exit_status, 0) == -1) {
        perror("waitpid");
        exit(1);
        return 1;
    }

    printf("successfully pulled <%s>\n", folder_name);
    return 0;
}

// Looks in the current directory and finds all directories with the format COMP*
// It then links and returns the names in a string seporated by a ';'.
char* get_comp_courses() {
    char* all_comp_courses = malloc(1000);

    // Open the current directory
    DIR* d;
    struct dirent* dir;
    if ((d = opendir(".")) == 0) {
        printf("error: could not load current directory\n");
        return NULL;
    }

    // Loop through and find all comp courses
    while ((dir = readdir(d)) != NULL) {
        if (strncmp(dir->d_name, "COMP", 4) == 0) {
            // printf("Found: %s\n", dir->d_name);
            strcat(all_comp_courses, ";");
            strcat(all_comp_courses, dir->d_name);
        }
    }
    closedir(d);
    return all_comp_courses;
}

// Loops through all courses in your directory and generates a command to link them to
// the respective directory in VLAB
int link_to_unsw(char* file_name) {
    printf("Linking to UNSW\n");

    if (file_name == NULL) {
        printf("Folders found in directory that can be linked:\n");
        char* all_comp_courses = get_comp_courses();
        // Loops through all the courses and runs a command to link them up
        char* token = strtok(all_comp_courses, ";");
        while (token) {
            int exit_status = system(generate_link_command(token, token));

            if (exit_status) {
                printf("error: There was an issue in connecting %s\n", token);
            }
            else {
                printf("successfully linked %s\n", token);
            }

            token = strtok(NULL, ";");
        }
    }
    else { // This only occures if you want to link just one file given
        struct stat st = { 0 };
        if (stat(file_name, &st) == -1) {
            printf("Creating <%s>\n", file_name);
            mkdir(file_name, 0700);
        }
        else {
            printf("File found in current directory\n");
        }

        int exit_status = system(generate_link_command(file_name, file_name));
        if (exit_status) {
            printf("error: There was an issue in connecting %s\n", file_name);
        }
        else {
            printf("successfully linked %s\n", file_name);
        }
    }
    return 0;
}

// Unlinks all COMP course folders in the current directory from VLAB
int unlink_from_unsw(char* file_name, int delete) {
    printf("Unlinking files from UNSW\n");

    char* all_comp_courses = get_comp_courses();

    // Loops through all the courses and runs a command to unlink them up
    char* token = strtok(all_comp_courses, ";");
    // Done inside this look as you do not want to delete or unlink files that are not,
    // part of cse
    if (file_name == NULL) {
        while (token) {

            int exit_status = system(generate_unlink_command(token));

            if (exit_status) {
                printf("error: There was an issue in unconnecting %s\n", token);
            }
            else {
                printf("successfully unlinked %s\n", token);
                char command[100] = { '\0' };
                if (delete) {
                    strcat(command, "rm -rf ");
                    strcat(command, token);
                    system(command);
                    printf("Deleted <%s>\n", token);
                }
            }
            token = strtok(NULL, ";");
        }
    }
    else {
        int exit_status = system(generate_unlink_command(file_name));

        if (exit_status) {
            printf("error: There was an issue in unconnecting %s\n", file_name);
        }
        else { // only if it can successfully unlink the file will it delete it.
            printf("successfully unlinked %s\n", file_name);
            char command[100] = { '\0' };
            if (delete) {
                strcat(command, "rm -rf ");
                strcat(command, file_name);
                system(command);
                printf("Deleted <%s>\n", file_name);
            }
        }
    }

    return 0;
}

int send_command(int argc, char* argv[]) {
    char command[100] = { '\0' };

    // Working on trying to find the correct directory.
    struct passwd* pw = getpwuid(getuid());
    char* homedir = pw->pw_dir;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }

    int length_of_home_dir = strlen(homedir);
    char* path = &cwd[length_of_home_dir + 1];

    printf("Executing in -> %s\n", path);

    // Putting the command together
    for (int i = 2; i < argc; i++) {
        strcat(command, argv[i]);
        strcat(command, " ");
    }

    char ssh_command[1000] = { '\0' };
    strcat(ssh_command, "sshpass -p ");
    strcat(ssh_command, ZPASS);
    strcat(ssh_command, " ssh ");
    strcat(ssh_command, ZID);
    strcat(ssh_command, "@login.cse.unsw.edu.au ");
    strcat(ssh_command, "\"cd ");
    strcat(ssh_command, path);
    strcat(ssh_command, "; ");
    strcat(ssh_command, command);
    strcat(ssh_command, ";\"");
    int exit_status = system(ssh_command);

    return 0;
}

char* generate_unlink_command(char* file_name)
{
    char* command = malloc(200);
    memset(command, '\0', 200);

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    strcat(command, "fusermount -u ");
    strcat(command, cwd);
    strcat(command, "/");
    strcat(command, file_name);
    // printf("Running %s\n", command);

    return command;
}

char* generate_link_command(char* file_name, char* path)
{
    char* command = malloc(200);
    memset(command, '\0', 200);

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    strcat(command, "echo '");
    strcat(command, ZPASS);
    strcat(command, "' | sshfs ");
    strcat(command, ZID);
    strcat(command, "@login.cse.unsw.edu.au:/import/reed/4/");
    strcat(command, ZID);
    strcat(command, "/");
    strcat(command, file_name);
    strcat(command, " ");
    strcat(command, cwd);
    strcat(command, "/");
    strcat(command, path);
    strcat(command, " -o password_stdin");
    //printf("Running: %s\n", command);
    
    // echo 'ZPASS' | sshfs z5555555@login.cse.edu.au:import/reed/4/z5555555/file_name cwd/path -o password_stdin
    //snprintf(command, 100 + sizeof(ZPASS), "%s%s%s%s%s%s%s%s%s%s%s%s%s", "echo '", ZPASS,
    //        "' | sshfs", ZID, "@login.cse.unsw.edu.au:/import/reed/4/", ZID, "/", 
    //        file_name, " ", cwd, "/", path, " -o password_stdin");
    
    return command;
}

int activate_ssh()
{

    printf("Remeber to use \"exit\" to exit ssh\n");

    // z5555555@login.cse.unsw.edu.au = 30 characters + 1 null character
    char command[33] = { '\0' };
    snprintf(command, 33, "%s%s", ZID, "@login.cse.unsw.edu.au");
    //strcat(command, ZID);
    //strcat(command, "@login.cse.unsw.edu.au");


    pid_t pid;
    extern char** environ;
    char* ssh_argv[] = { "/bin/sshpass", "-p", ZPASS, "ssh", command, NULL };

    // spawn "/bin/date" as a separate process
    if (posix_spawn(&pid, "/bin/sshpass", NULL, NULL, ssh_argv, environ) != 0) {
        perror("spawn");
        exit(1);
        printf("There was an error in connecting to ssh\n");
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
