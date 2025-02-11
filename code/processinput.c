#include "common.h"

void process_input(char *input) {
    char original_input[INPUT_MAX_SIZE];
    strcpy(original_input, input);

    char *command;
    char *rest = input;

    trim_whitespace(input);

    while ((command = strtok_r(rest, ";", &rest))) {
        char *sub_command;
        char *sub_rest = command;
        char *first_part = strtok(command, "&");
        char *second_part = strtok(NULL, "&");
        trim_whitespace(first_part);
        if(second_part != NULL){
            trim_whitespace(second_part);
        }
        // printf("first part:%s\n", first_part);
        // printf("second part:%s\n", second_part);
        bool is_background = false;
        for(int i=0;i<strlen(original_input);i++){
            if(original_input[i]=='&'){
                execute_command(first_part, true);
                if(second_part != NULL){
                    execute_command(second_part, false);
                }
                is_background = true;
            }
        }
        if(!is_background){
            execute_command(first_part, false);
        }
    }
    return;
}