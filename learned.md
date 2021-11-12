# SQL
# A query goes through the front end with the following steps
    1. Tokenizer
    2. Parser
    3. Code Generator

## Input (SQL query) -> Output (sql virtual machine bytecode)

# The backend consists of
    1. Virutual Machine: takes the output(bytecode) then performs operations on one or more tables and/or indexes stored in a B-tree.
    2. B-tree: a collection of nodes each one page long. It can retrieve and save pages on the disk using the pager.
    3. Pager: reads and writes pages at appropriate offsets. It also keeps cache or recent pages, and determines when to save them.
    4. OS Interface: Different depending on OS.

## The fronend is considered as the SQL complier. It parses a given string to bytecode that can be interpreted by the backend. The bytecode is executed by the virtual machine.







# Non SQL commands are called meta commands they start with a '.'(dot).
# Exceptions are not allowed in C so we need to use Enums to give us reults back to errors.
# What fuctions do are explained in side code using comments.
# The Code
## When the code starts we create a table (currently only a user table). Inside a infite loop we ask for the an input.
## The input is checked if it is a meta command or an SQL command. Meta commands are commands that start with '.'.
## If it is a meta command we get into a switch statment that checks all the meta command form an enum.
## Other wise if it is a SQL statment we prepare the command using the prepare statment method. The switch statment checks the enum for errors or sucesses.
## Lastly we pass the statment and the table to the execute_staement method to be processes it returns an enum with and error or success code.
