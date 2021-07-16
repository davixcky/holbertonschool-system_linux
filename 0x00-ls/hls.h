#ifndef HLS_H
#define HLS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

/**
 * struct _node_permissions - Manage octal group 3 unix permissions
 *
 * @read: Octal value for reading permissions
 * @write: Octal value for writing permissions
 * @execute: Octal value for execution permissions
 **/
typedef struct _node_permissions
{
	char read;
	char write;
	char execute;
} node_permissions_t;

/**
 * struct file_node - Symbolize a unique file/dir unix
 *
 * @filename: Unique name that identify the node
 * @filename_upper: Unique name that identify the code in upper case
 * @size: Node size given in bytes
 * @node_type: It could be r/d/c/b/f/l/s
 * @owner_name: Name of the owner
 * @group_name: Name of the group
 * @last_modification: Char time of the last modification
 * @number_links: Numbers links
 * @user_permissions: User access permissions
 * @group_permissions: Group access permissions
 * @other_permissions: Other access permissions
 * @next: Next child in the list
 * @prev: Prev child in the list
 **/
typedef struct file_node
{
	char *filename;
	char *filename_upper;
	unsigned long size;
	char node_type;
	char *owner_name;
	char *group_name;
	char *last_modification;
	long number_links;
	node_permissions_t *user_permissions;
	node_permissions_t *group_permissions;
	node_permissions_t *other_permissions;
	struct file_node *next;
	struct file_node *prev;
} file_node_t;

/**
 * struct parent_node - Symbolyze a unique route, ex `ls test test2` a node
 * will be test and other test2
 *
 * @filename: Unique name that identify the parent
 * @dir_stream: Pointer to the main dir
 * @head_file: Head of the children nodes
 **/
typedef struct parent_node
{
	char *filename;
	DIR *dir_stream;
	file_node_t *head_file;
} parent_node_t;

/**
 * struct _general_info - Contains the global info of the hls command
 *
 * @head_parent: Head of the parent nodes
 * @argc: Number arguments passed
 * @argv: Arguments passed
 **/
typedef struct _general_info
{
	parent_node_t *head_parent;
	int argc;
	char **argv;
} general_t;

/**
 * struct _node_type - Typedef struct
 *
 * @key: Name of the mode
 * @type: Value symbol
 **/
typedef struct _node_type
{
	mode_t key;
	char type;
} node_type_t;

/* execute */
int execute_hls(general_t *info);
parent_node_t *get_path_nodes(general_t *info, char *path);
char get_node_type(mode_t mode);
void get_node_info(file_node_t *node, char *base_path);
char *build_path(char *base_path, char *filename);
void get_upper_filename(file_node_t *node);
void set_permissions(file_node_t *node, mode_t mode);
void free_directories(parent_node_t *parent);
file_node_t *create_file_node(struct dirent *dir, char *base_path);
void print_directory(parent_node_t *parent);
void sorted_insert(file_node_t **head, file_node_t *new_node);

/* text utils */
int _strlen(char *s);
int _strcmp(char *s1, char *s2);
int is_lowercase(char c);
char *string_toupper(char *s);
char *_strdup(char *str);
char *_itoa(int num, char *str, int base);
int _isdigit(int c);

/* error handling */
int error(general_t info, char *filename);

#endif
