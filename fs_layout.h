//
//  fs_layout.h
//  
//
//  Created by Pragnya Suresh on 29/09/18.
//

/*superblock|inode list|data block mappings|root|data block list */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>


#define BLOCKSIZE 4096
#define BLOCK_NO 100
#define INODE_SIZE 80
#define INODE_NO 51
#define SUB_NO 59
#define MAX_NAME_LEN 64
#define MAX_MAP 3

#define AT (1 << 0)
#define CT (1 << 1)
#define MT (1 << 2)


struct superblock {
int magic;  //sequence of bytes to identify format of file
int blocksize; //size of a block(4096)
int  inode_table_size; //total no. of blocks for inode
int  inode_count;  //no. of inodes per block
int  data_block_count; //total no. of data blocks
int root_dir; //root's inode number
int max_file_length;//max file size=4096x3=12288 bytes
};

struct superblock *super;


typedef enum {
    ORDINARY,
    DIRECTORY,
    FREE
} file_type;


//Allocating max 3 blocks for a file
struct myinode {
ino_t st_id;            //inode id
mode_t st_mode;         //permissions and type of inode-file, directory or a block device.
nlink_t st_nlink;       //number of hard links
off_t st_size;          //size
uid_t st_uid;           //user id
blkcnt_t st_blocks;     //number of blocks rallocated to this file
time_t st_mtim;         //modify time
time_t st_atim;         //access time
time_t st_ctim;         //creation time
int direct_blk[MAX_MAP];      //no. of blocks directly mapped
file_type type;  //0-free    1-file      2-directory
    
};

struct myinode *root;

//Reducing limit to 60 files in a directory and 64 characters in the name
struct mydirent {
    char name[MAX_NAME_LEN];      //name of directory
    char subs[SUB_NO][MAX_NAME_LEN];  //names of contents
    int sub_id[SUB_NO];     //inode ids of contents
    char extra[20];  //padded bytes to keep block size 4096
};

struct filehandle {
    struct myinode *node;
    int o_flags;
};


void openfile();
int make_rootnode(ino_t st_id, file_type type, int blk, mode_t mode);
int make_rootdir(char* name);
int init_fs();
void *fs;



