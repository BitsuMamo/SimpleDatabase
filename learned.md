# Day 1
## SQL
A query goes through the front end with the following steps
    1. Tokenizer
    2. Parser
    3. Code Generator

Input (SQL query) -> Output (sql virtual machine bytecode)

The backend consists of
    1. Virutual Machine: takes the output(bytecode) then performs operations on one or more tables and/or indexes stored in a B-tree.
    2. B-tree: a collection of nodes each one page long. It can retrieve and save pages on the disk using the pager.
    3. Pager: reads and writes pages at appropriate offsets. It also keeps cache or recent pages, and determines when to save them.
    4. OS Interface: Different depending on OS.

The fronend is considered as the SQL complier. It parses a given string to bytecode that can be interpreted by the backend. The bytecode is executed by the virtual machine.

During this day I created a simpel REPL to get input from the user and show out put to the user. The REPL accepted a meta command to exit the loop.


# Day 2
## Separating meta command and SQL query proccessing.
Meta commands are non sql command that start with a dot(.). To process this separately I created a function call do_meta_command() that parses meta command. The meta command are checked by checking the first letter of the input.
## Exceptions
Since exceptions are not allowed in C, we need to use Emuns. Our functions return enum values. We process these value to check what out function return and give appropriate response.
Exceptions are not allowed in C so we need to use Enums to give us reults back to errors.

I also added comments explaning most of the fucntions created.

## The Code so far
When the code starts we create a table (currently only a user table). Inside a infite loop we ask for the an input.
The input is checked if it is a meta command or an SQL command. Meta commands are commands that start with '.'.
If it is a meta command we get into a switch statment that checks all the meta command form an enum.
Other wise if it is a SQL statment we prepare the command using the prepare statment method. The switch statment checks the enum for errors or sucesses.
Lastly we pass the statment and the table to the execute_staement method to be processes it returns an enum with and error or success code.


# Day 3
## Testing
Inmplemeted basic testing user rspec. Created 5 test: input/output, maximum lenght string, over maximum lenght string, table full ,and negative id.
For the test and SQL added more enums and handled them appropirately.

Created new function for insert statmentes. Used strok() rather than scanf() to avoid memory overflow problems. Also added one byte to strings to compenstate for the last mandatory byte.

# Day 4
## Persistens


# Testing

## I have learned the bare minimun rspec that has been used in this tutorial.
## I have understood the need for testing and its uses during development. I should testing for future projects too.

# Overall learned things

1. SQL and how it works in the back
2. C programming language
3. Unit Testing
