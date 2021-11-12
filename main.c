// Website is https://cstack.github.io/db_tutorial/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

// Macros
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define TABLE_MAX_PAGES 100
typedef enum{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef enum{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

// Add one more byte to accomodate null string
typedef struct{
    u_int32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct{
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef struct{
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct{
    u_int32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

// Constants
const u_int32_t ID_SIZE = size_of_attribute(Row, id);
const u_int32_t USERNAME_SIZE = size_of_attribute(Row, username);
const u_int32_t EMAIL_SIZE = size_of_attribute(Row, email);
const u_int32_t ID_OFFSET = 0;
const u_int32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const u_int32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const u_int32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Pages size is 4KB since a page is that size on most computer architectures.
const u_int32_t PAGE_SIZE = 4096;
const u_int32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const u_int32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

/*
 * Allocates all memory need for the InputBuffer struct.
 * Return the memory for the allocated space.
 * */
InputBuffer* new_input_buffer(){
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

/*
 * It takes InputBuffer.
 * Frees memeory allocated to InputBuffer.
 * */
void close_input_buffer(InputBuffer* input_buffer){
    // Need to free space for both the buffer allocated throught the get line method
    // and the struct which we allocate when we call new_input_buffer method
    free(input_buffer->buffer);
    free(input_buffer);

}
/*
 * Create a new table by allocating the porper size for a table
 * It also initialized all the page to NULL
 * */
Table* new_table(){
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;
    for(u_int32_t i = 0; i < TABLE_MAX_PAGES; i++){
        table->pages[i] = NULL;
    }
    return table;
}

/*
 * Accepts a table and free all memory allocated to pages as well as the table
 * */
void free_table(Table* table){
    for(int i= 0; table->pages[i]; i++){
        free(table->pages[i]);
    }
    free(table);
}

void print_prompt(){
    printf("db > ");
}

/*
 * Takes an InputBuffer struct and inserts the input data.
 * Using the getline method it also save the input lenght.
 * We need the input length since it could be less than the size of the input.
  */
void read_input(InputBuffer* input_buffer){
    // While starting since buffer is Null getline will allocate the memory needed.
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

/*
 * It take the Row as a source and void as destination as memory.
 * Copys the data from the row to the page memory found at the destincation memory.
 * Basic operation is writing to file
 * */
void serialize_row(Row* source, void* destination){
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/*
 * It take the Row as a destination and void as source as memory.
 * Copys data from the page memory to the row to be read.
 * Basic operation is reading from file
 * */
void deserialize_row(void* source, Row* destination){
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

/*
 * It takes the a Table and row_num variable.
 * Calculates the page_num that it is going to be written on.
 * From the caluculated page number we get the page form the table if it is not NULL otherwise
 * we allocate memory for the pages
 * It rerturn the memroy address of the page added witht the byte_offset to get an open slot.
 * */
void* row_slot(Table* table, u_int32_t row_num){
    u_int32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    if (page == NULL){
        // Allocating memory when trying accessing a page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }

    u_int32_t row_offset = row_num % ROWS_PER_PAGE;
    u_int32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void print_row(Row* row){
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

/*
 * It takes an inputbuffer and does all the MetaCommands passed.
 * */
MetaCommandResult do_meta_command(InputBuffer* input_buffer){
    // Currently only works for '.exit' command
    if(strcmp(input_buffer->buffer, ".exit") == 0){
        exit(EXIT_SUCCESS);
    }else{
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement){
    statement->type = STATEMENT_INSERT;

    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL){
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if(id < 0){
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE){
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE){
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

// Prepare the result based on the input and return an enum for prepare
// Also sets the statement type
/*
 * It takes an InputBuffer and a Statement.
 * Checks for different SQL statements and assigns the statement->type to the correct enum value.
 * Currently we check and do the this for the user table hard coded. The select print all data.
 * Returns an enum value with an error or success.
 * */
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
    if(strncmp(input_buffer->buffer, "insert", 6) == 0){
        return prepare_insert(input_buffer, statement);
    }

    if(strcmp(input_buffer->buffer, "select") == 0){
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

/*
 * It takes a Statement and Table.
 * Uses the Statement->row_to_insert data to insert data to table.
 * Uses serialize and row_slot to insert data
 * Returns enum value of success or error
 * */
ExecuteResult execute_insert(Statement* statement, Table* table){
    if (table->num_rows >= TABLE_MAX_ROWS){
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

/*
 * It takes a Statement and Table.
 * Uses deserialize_row and row_slot to print data
 * Returns enum value of success or error
 * */
ExecuteResult execute_select(Statement* statement, Table* table){
    Row row;
    for(u_int32_t i = 0; i < table->num_rows; i++){
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }

    return EXECUTE_SUCCESS;
}

/*
 * Takes the Statement and Table.
 * Based on the Statement->type it will run the execute_insert or execute_select functions.
 * Returns enum value of success or error
 * */
ExecuteResult execute_statement(Statement* statement, Table* table){
    switch (statement->type){
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
            break;
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
            break;
    }
}


int main(int argc, char* argv[]){
    Table* table = new_table();
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
            case(PREPARE_STRING_TOO_LONG):
                printf("String is too long.\n");
                continue;
            case(PREPARE_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case(PREPARE_SYNTAX_ERROR):
                printf("Syntax Error. Could not parse statment.");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
               printf("Uncrecognized keyword at start '%s'.\n", input_buffer->buffer);
               continue;
        }

        switch(execute_statement(&statement, table)){
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table Full.\n");
        }
    }
}
