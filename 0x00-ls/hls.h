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
typedef struct __attribute__((__packed__))
{
	char read;
	char write;
	char execute;
} node_permissions_t;

/**
 * struct file_node - Symbolize a unique file/dir unix
 *
 * @filename: Unique name that identify the node
 * @size: Node size given in bytes
 * @node_type: It could be r/d/c/b/f/l/s
 * @user: User access permissions
 * @group: Group access permissions
 * @other: Other access permissions
 * @parent: Parent directory if recursion is enabled
 * @next: Next child in the list
 * @children_list: Head of the file_node if recursively was enabled
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
	struct file_node *previous;
} file_node_t;

/**
 * struct parent_node - Symbolyze a unique route, ex `ls test test2` a node
 * will be test and other test2
 *
 * @filename: Unique name that identify the parent
 * @head_file: Head of the children nodes
 * @next: Next parent node if exists
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
 * @status_code: Status code of the command
 * @argc: Number arguments passed
 * @argv: Arguments passed
 **/
typedef struct __attribute__((__packed__))
{
	parent_node_t *head_parent;
	int argc;
	char **argv;
} general_t;

/**
 * struct _format - Typedef struct
 *
 * @type: Format
 * @f: The function associated
 **/
typedef struct _node_type
{
	mode_t key;
	char type;
} node_type_t;

/* execute.c */
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

/* text utils */
int _strlen(char *s);
int _strcmp(char *s1, char *s2);
int is_lowercase(char c);
char *string_toupper(char *s);
char *_strdup(char *str);

/* error handling */
int error(general_t info, char *filename);

#endif
