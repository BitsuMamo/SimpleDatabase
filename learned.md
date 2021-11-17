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
## Persistency.
Implemeted saving inserted item to a file and a pager to allow cachine and reading.

The table has a pager and the pager has all the pages that a database has. The pager opens a connection and read data from the file.
The db_open() fucntions creates a pager with a filename. From the pager it calculates the number of rows saved. After it allocated memory for a Table and creates it with the number of rows.

The db_close() functions saves all data to the database file and free memory allocated. Uses the pager_flush() function to save.

The pager_flush() function write the data on the appropriate location.

Implemeted cursors. They track when our rows are. Aslo replaced row_slot() function with cursor value. It makes everything accesible by cursors.
Integrated the execute functions with cursors.

# Day 5
Implemeted a basic BTree.
Created constants for a common node and a leaf node.

Implelemted leaf_node_insert() method to insert a key and row at a given cursor location.
Reimplemented all cursor functions to work with node instead of arrays.

##Node functions
1. leaf_node_num_cells -> gets the number of cells found in a leaf node.
2. leaf_node_cell -> get the memory location of the a certain leaf node cell.
3. leaf_node_key -> get the key of the a certain leaf node.
4. leaf_node_value -> get memory location fo the value for a certain leaf node.
5. initialize_leaf_node -> Initializes a node to 0 Cells.

# Day 6
Implemeted duplicate key checking and searching.
Errors: Every new data after two become null with only the primary key exisiting. Might be tutorial problem.

table_find() function takes the table and key and gets a leaf node or an internal node(to be implemented)
leaf_node_find() function takes the table, page number and primary key and implemets a binary search to find where the key should be placed inside the tree. Return a position of a key, a postion we'll need to move or postion past the key.
get_node_type() return the node type of a certain node.
set_node_type() sets the node type of a certain node.
Using these during initialize_leaf_node we set the node type to leaf node.


# Testing

## I have learned the bare minimun rspec that has been used in this tutorial.
## I have understood the need for testing and its uses during development. I should testing for future projects too.

# Overall learned things

1. SQL and how it works in the back
2. C programming language
3. Unit Testing

# B-Trees
The B-Tree is a data structure that SQLite uses.
##Why a B-Tree?
1. Fast searching. Logrithmic.2. Fast Insert/Delete. Constant
3. Fast range traversal.

### A B-Tree is different from a binary tree. The B stands for balanced.
Unlike a binary tree a B-Tree can have more than two children. Each node can have m children. m is called the trees order.
To keep it balanced we say nodes need to have at least m/2 children(rounded up).

### Exceptions
1. If leaf nodes have 0 children,
2. The root node can have fewer than m children, but has to have at least 2.
3. If the root node the only node( The root is the leaf node ), it still has 0 children.

## Variations
The B-Tree explained above is used to store indexes. For tables there is a Variations called a B+Tree.

## B+Tree
Nodes with children are called internal nodes.

|Order m Tree|Internal Node|Leaf Node|
|------------|-------------|---------|
|Stores      |key and pointer to children|keys and values|
|Number of keys|up to m-1  |as many available|
|Number of pointers|number of keys+1|none|
|Number of values|none     |number of keys|
|key purpose|Routing       |paird with value|
|Stores Values|No          |Yes|
