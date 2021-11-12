// Website is https://cstack.github.io/db_tutorial/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct{
    StatementType type;
} Statement;

InputBuffer* new_input_buffer(){
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt(){
    printf("db> ");
}

// Uses getline method with InputBuffer struct to read input from command line
// and save the input and the length
void read_input(InputBuffer* input_buffer){
    // While starting since buffer is Null get line will allocate the memory needed.
    ssize_t bytes_read =
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if(bytes_read <= 0){
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // -1 to remove trailing new line
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}


// Since memory is allocated we need to free it after it has been used
// and to avoid memory leeks
void close_input_buffer(InputBuffer* input_buffer){
    // Need to free space for both the buffer allocated throught the get line method
    // and the struct which we allocate when we call new_input_buffer method
    free(input_buffer->buffer);
    free(input_buffer);

}


// A fuction to do meta commands passed by the user
MetaCommandResult do_meta_command(InputBuffer* input_buffer){
    // Currently only works for '.exit' command
    if(strcmp(input_buffer->buffer, ".exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

// Prepare the result based on the input and return an enum for prepare
// Also sets the statement type
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
    if(strncmp(input_buffer->buffer, "insert", 6) == 0){
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if(strcmp(input_buffer->buffer, "select") == 0){
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}


void execute_statement(Statement* statement){
    switch (statement->type){
        case (STATEMENT_INSERT):
            printf("Insert\n");
            break;
        case (STATEMENT_SELECT):
            printf("Select\n");
            break;
    }
}


int main(int argc, char* argv[]){
    InputBuffer* input_buffer = new_input_buffer();
    while(true){
        print_prompt();
        read_input(input_buffer);

        if(input_buffer->buffer[0] == '.'){
            switch(do_meta_command(input_buffer)){
                case(META_COMMAND_SUCCESS):
                    continue;
                case(META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s' \n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch(prepare_statement(input_buffer, &statement)){
            case(PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
               printf("Uncrecognized keyword at start '%s'.\n", input_buffer->buffer);
               continue;
        }

        execute_statement(&statement);
        printf("Executed.\n");
    }
}
