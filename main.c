#include "unsw.h"
int activate_ssh();

int main(int argc, char *argv[]) {

    if (argc == 1) {
        printf("TODO - add help");
        return 1;
    }
    
    // LINK COMMAND
    if (strcmp(argv[1], "-l") == 0) {
        if (argc == 2) {
            link_to_unsw(NULL);
        } else if (argc == 3) {
            link_to_unsw(argv[2]);
        }
    
    // UNLINK COMMAND
    } else if (strcmp(argv[1], "-u") == 0) {
        if (argc == 2) {
            unlink_from_unsw(NULL, TRUE);
        } else if (argc == 3) {
            if (strcmp(argv[2], "-k") == 0) {
                unlink_from_unsw(NULL, FALSE);
            } else {
                unlink_from_unsw(argv[2], TRUE);
            }
        } else if (argc == 4) {
             if (strcmp(argv[2], "-k") == 0) {
                unlink_from_unsw(argv[3], FALSE);
            }
        }

    // CLONING A FILE
    } else if (strcmp(argv[1], "clone") == 0) {
        
        printf("Fetching file <%s>\n", argv[2]);
        printf("This may take a moment\n");
        clone_folder(argv[2]);
    
    // PUSHING A FILE
    } else if (strcmp(argv[1], "push") == 0) {

        printf("pushing current changes \n");
        printf("This may take a moment\n");
        
        // Allows the user to either soecify what they want to push, or just push from the directory    
        if (argc == 2) {
            push(NULL);
        } else if (argc == 3) {
            push(argv[2]);
        }
    
    // PULLING A FILE
    } else if (strcmp(argv[1], "pull") == 0) {

        printf("pulling from remote\n");
        printf("This may take a moment\n");

        if (argc == 2) {
            pull(NULL);
        } else if (argc == 3) {
            pull(argv[2]);
        }
    
    // RUNNING A COMMAND VIA EXTERNAL
    } else if (strcmp(argv[1], "-r") == 0) {
      
        if (argc < 3 ) {
            printf("error: too few arguments\n");
        }
        send_command(argc, argv);
    
    // SSH INTO UNSW
    } else if (strcmp(argv[1], "-ssh") == 0) {
        activate_ssh();

    } else {
        printf("error: unknown flag\n");
    }

    return 0;
}
