
# <span style="text-decoration:underline;">FILE SYSTEM IMPLEMENTATION USING FUSE API</span>


**Abstract Overview:**

**Filesystem in Userspace (FUSE)** is a [software interface](https://en.wikipedia.org/wiki/Software_interface) for [Unix-like](https://en.wikipedia.org/wiki/Unix-like) computer [operating systems](https://en.wikipedia.org/wiki/Operating_system) that lets non-privileged users create their own [file systems](https://en.wikipedia.org/wiki/File_system) without editing [kernel](https://en.wikipedia.org/wiki/Kernel_(computing)) code. This is achieved by running file system code in [user space](https://en.wikipedia.org/wiki/User_space) while the FUSE module provides only a "bridge" to the actual kernel interfaces. FUSE is [free software](https://en.wikipedia.org/wiki/Free_software) originally released under the terms of the [GNU General Public License](https://en.wikipedia.org/wiki/GNU_General_Public_License) and the [GNU Lesser General Public License](https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License). 

To implement a new file system, a handler program linked to the supplied libfuse library needs to be written. The main purpose of this program is to specify how the file system is to respond to read/write/stat requests. The program is also used to [mount](https://en.wikipedia.org/wiki/Mount_(computing)) the new file system. At the time the file system is mounted, the handler is registered with the kernel. If a user now issues read/write/stat requests for this newly mounted file system, the kernel forwards these IO-requests to the handler and then sends the handler's response back to the user.

FUSE is particularly useful for writing [virtual file systems](https://en.wikipedia.org/wiki/Virtual_file_system). Unlike traditional file systems that essentially work with data on mass storage, virtual filesystems don't actually store data themselves. They act as a view or translation of an existing file system or storage device.In principle, any resource available to a FUSE implementation can be exported as a file system.

libfuse offers two APIs: a "high-level", synchronous API, and a "low-level" asynchronous API. In both cases, incoming requests from the kernel are passed to the main program using callbacks. When using the high-level API, the callbacks may work with file names and paths instead of inodes, and processing of a request finishes when the callback function returns. When using the low-level API, the callbacks must work with inodes and responses must be sent explicitly using a separate set of API functions.


# TABLE OF CONTENTS


<table>
  <tr>
   <td><strong>Serial No.</strong>
   </td>
   <td><strong>Topic</strong>
   </td>
  </tr>
  <tr>
   <td>1.
   </td>
   <td>Introduction
   </td>
  </tr>
  <tr>
   <td>2.
   </td>
   <td>Requirement Specification
   </td>
  </tr>
  <tr>
   <td>3.
   </td>
   <td>Design
   </td>
  </tr>
  <tr>
   <td>4. 
   </td>
   <td>Implementation
   </td>
  </tr>
  <tr>
   <td>5.
   </td>
   <td>Testing of Test Cases
   </td>
  </tr>
  <tr>
   <td>6.
   </td>
   <td>Conclusion
   </td>
  </tr>
</table>


**Introduction & Brief Overview**

In computing, a File System controls how data is stored and retrieved. Without a file system, information placed in a storage medium would be one large body of data with no way to tell where one piece of information stops and the next begins. The file system has two aspects:

1) Data structures : what type of on-disk structures are utilized by the file system to organize its data and metadata.

2) Access methods : how does it map the calls made by a process onto its data structures.

**Goals**

To create a simple, fast and efficient file system for storing and retrieving files and directories in any order and to avoid internal fragmentation as much as possible.

**Features**



1. Creating ordinary files of any size (unless smaller than file system size) in READ, WRITE, APPEND modes.
2. Creating directories with any level of nesting.
3. Copying ordinary files from one location in the file system to another.
4. TRUNCATING pre - existing files with new data.
5. Writing into files and reading the written information.

**Detailed Description**

The filesystem created by the team is a simple file system that involves 6 major blocks-superblock,inode list,data bitmap,sector metadata,root, data blocks. This file system supports three kinds of files (Like Linux File System) - Ordinary File, Directory File and Free File. On a high level, our file system involves blocks of block size 4096 bytes (4K bytes) and subdivided into 16 sectors of size 256 bytes. We have used the concept of sectors in order to reduce the wastage due to  internal fragmentation.

We have implemented create(), open(),read(),write(),unlink(),truncate(), mkdir(), rmdir() and readdir() system calls.

The moment the file system is mounted, the root directory is created. Inode number 0 is reserved for the root. Its parent is also set as root. 16 sectors are allotted to it.

Inode stores the metadata about a file/ directory. This metadata includes the inode number (low level name given to the file),file type, number of links, uid, gid, permissions associated with this file etc. To begin with, all the inode numbers are initialised to 0 and file type is initialised to FREE. The sector meta data comprise of an integer array that stores the starting sector number along with the total number of contiguous sectors allocated to this file. In order to create a new file, we first check the data bitmap in order to find an empty sector and an inode which is unassigned(which holds inode number 0).Each empty file is assigned one sector initially.  Once the file is created, it can be opened for reading and writing.  A file can be opened for writing in the append mode. When an existing file is recreated, it is truncated to size 0 and all its contents are overwritten. In the append mode, we calculate the number of sectors that are required to be allocated to this file. If the amount of data to be written exceeds the capacity of the sectors that are currently allocated to the file, we look at the bitmap again to find more free sectors. The sector number is updated in the sector metadata and the data bitmap is also updated to indicate that the sector is now occupied. Now, data is written to the sectors that are allocated to this file in a sequential fashion.  The last four bits of each sector stores the offset which is the number of bytes written onto that sector.

When a file is opened for reading, the sector number of the starting sector is retrieved along with the number of contiguous sectors and the data contained in these sectors is read in order.

When a file is removed, all its contents are removed from its data block. The entry corresponding to this file is removed from its parent directory, sector metadata is reset and its inode contents are re-initialised.

We have made our file system persistent by saving the filesystem state in a binary file. This binary file remains intact even when the system is rebooted.

**Commands for Mounting and Unmounting a FUSE- FIle System**



*   Mounting a FUSE File System

    ```
    $gcc filesystem.c `pkg-config fuse --cflags --libs` 
    $./a.out -f <path to mount point>
    ```


*   Unmounting a FUSE File System


```
$fusermount -u <path to mount point>
```


**REQUIREMENTS** :

Hardware Requirements : 



1. Download size: 1.0 MB
2. Estimated disk space required: 20 MB
3. Estimated build time: less than 0.1 SBU
4. Size of file system = 389120 bytes space on hard disk

	

Software Requirement : 



1. libfuse library
2. Gcc compiler

**DESIGN OF FILE SYSTEM**

We have used the array data structure for our file system. The array is divided into blocks. Each block is of size 4KB. We have divided each block into sectors of size 256 bytes. This is

done in order to reduce the wastage due to  internal fragmentation (i.e. instead of providing an entire block of memory to a file which might require very little space, we break the block into sectors, thus accommodating a maximum of 4KB / 256bytes = 16 files per block).

Starting from the 0th block the File System layout is described as follows:


<table>
  <tr>
   <td><strong><em>Superblock</em></strong>
   </td>
   <td><strong><em>Inode List</em></strong>
   </td>
   <td><strong><em>Data Bitmap</em></strong>
   </td>
   <td><strong><em>Sector-meta- data</em></strong>
   </td>
   <td><strong><em> Root</em></strong>
   </td>
   <td><strong><em>Data blocks</em></strong>
   </td>
  </tr>
</table>


The specifics of the File System are:


```
BLOCKSIZE 4096		//Each data block is 4KB
BLOCK_NO 95			//Total number of blocks
INODE_SIZE 80		//The size of an inode is 80 bytes
SECTOR_SIZE 84		//Size of sector meta data is 84 bytes
INODE_NO 45			//Total number of inodes
SUB_NO 59			//Total number of sub-directories in a dir
MAX_NAME_LEN 64		//The max number of characters in file/directory name
SECT_SIZE 256		//The size of a sector is 256 bytes
METADATA 20480		//This is the total size of the file system meta data(the first 5 blocks)
```


<span style="text-decoration:underline;">Structures Used:</span>

Super Block Structure


```
struct superblock {
    int magic;  //sequence of bytes to identify format of file
    int blocksize; //size of a block(4096)
    int  inode_table_size; //total no. of blocks for inode
    int  inode_count;  //no. of inodes per block
    int  data_block_count; //total no. of data blocks
    int root_dir; //root's inode number
    int max_file_length;//max file size=4096x2=8192 bytes
};
```


Inode Structure


```
struct myinode {
    ino_t st_id;            //inode id
    mode_t st_mode;         //permissions
    nlink_t st_nlink;       //number of hard links
    off_t st_size;          //size		
    uid_t st_uid;           //user id
    gid_t st_gid;           //group id
    int sector_pointer;   	 //which is the starting sector
    int sector_tuples;      //how many sector tuples 
    time_t st_mtim;         //modify time
    time_t st_atim;         //access time
    time_t st_ctim;         //creation time
    file_type type;  		 //0-free 1-file 2-directory

};
```


Directory Structure


```
struct mydirent {
    char name[MAX_NAME_LEN];          //name of dir
    char subs[SUB_NO][MAX_NAME_LEN];  //names of contents
    int sub_id[SUB_NO];               //inode ids of contents
    char extra[20];
};
```


Sector Metadata Structure


```
struct sector{
    int tuples[20];   	    //sector tuples with (start sector id,
    int extratuples;                   length of contiguous sectors)					         //extratuples points to a block to 
};						store more tuples
```


File Handler Structure


```
struct filehandle {
   struct myinode *node;			//Inode of the file
   int o_flags;					//Flags of the file
};
```


Fuse Operations Structure


```
static struct fuse_operations fs_oper = {
	.getattr = fs_getattr,
	.mkdir = fs_mkdir,
	.rmdir = fs_rmdir,
	.readdir = fs_readdir,
	.create = fs_creat,
  	.open = fs_open,
  	.read = fs_read,
  	.write = fs_write,
  	.mknod = fs_mknod,
  	.unlink = fs_unlink
};
```


**<span style="text-decoration:underline;">SYSTEM CALLS</span>**

**1) <code><span style="text-decoration:underline;">fs_creat</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_creat(const char *path, mode_t mode, struct fuse_file_info *fi)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
struct fuse_file_info * fi - contains file information such has file handle and other flags.
mode_t mode - The mode argument specifies the file permission bits to be used in creating the file

Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c. _**fs_creat implements the basic file create operation. First, we check to see if the inode of the file already exists. If yes, then the error number corresponding to the error ‘EEXIST’ is returned. If not, then we search for a free inode in the inode list and add this inode to the parent directory along with the name of the file. The number of links field in the inode is updated to one. Now, an empty sector is found by searching the data bitmap. Once found, the corresponding location in the data bitmap is updated to one to indicate that the sector is no longer free. The file handle points to the newly created inode and the flags are set.

**2) <code><span style="text-decoration:underline;">fs_open</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_open(const char *path, struct fuse_file_info *fi)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
struct fuse_file_info * fi - contains file information such has file handle and other flags.

Returns 0 on success, errno if call fails.
```


	

Defined in **_syscalls.c_**.This function is used to open at path given by const char * path. The inode of the file is retrieved by traversing the directories in the path (using the function getnodebypath (described below) and the filehandle is made to reference the inode and the flags are set.Now, further operations such as read, write etc. can be performed on this file.


```
3) fs_read 
```


	

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
char * buf - buffer string where read content is passed into
size_t size - size of the buffer in bytes
off_t offset - offset of the file 
struct fuse_file_info * fi - contains file information such has file handle and other flags.

Returns size of the buffer.
```


Defined in **_syscalls.c_** . fs_read implements basic file read operations. Reads every sector in an orderly fashion, considering the offset at the end of every sector. The sector metadata (_sector tuples of starting sector number, number of continuous sectors _) , is read from the `fuse_file_info` variable and written into the `char * buf` buffer. 

**<code>4) <span style="text-decoration:underline;">fs_write </span></code></strong>	

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
char * buf - buffer string where the content that has to be written is present
size_t size - size of the buffer in bytes
off_t offset - offset of the file 
struct fuse_file_info * fi - contains file information such has file handle and other flags.

Returns size of the buffer.
```


Defined in **_syscalls.c_** . fs_write implements basic file write operations. The file can be opened in only two ways in this simple file system, namely - write mode and append mode. In the write mode, the function uses the size of the `char * buf `buffer to assess the number of sectors that are required for the incoming content. This is then passed to another function - `get_req_sects` (explained in detail below) to allocate the required sectors (if present/free ). Now, the sector metadata is updated to add the newly allocated sectors. 

 The append mode, begins writing from the end of the previously written file. The last four bytes of every sector stores how many bytes are written onto it. Every write/append operation is a four step process. It is done by finding the offset of every block, calculating how much to write in every sector, writing to the sector and the finally updating the offset at every block.

**5) <code><span style="text-decoration:underline;">fs_unlink</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_unlink(const char *path)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file

Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c_** . This function is used to remove a file. The contents of the file are removed by making the data 0 in the sectors of the data block.The inode of the file is retrieved using the function getnodebypath (described below) and the basename is retrieved from the path in order to obtain the file name. Now, the entry corresponding to the file is removed from the directory. The inode fields are updated. Size of the file and link count  is made 0.

**6) <code><span style="text-decoration:underline;">fs_truncate</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_truncate(const char *path, off_t offset)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
off_t offset -new file size
Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c_** . This function is used to truncate a file to size given by `offset. `If the offset is zero, fs_truncate removes the data from the sectors associated with this file and makes the necessary changes to sector meta data. It also updates the inode with the new size. If the offset is non-zero,  an empty file comprising `offset `bytes is created and the inode is updated with this new size.

**7) <code><span style="text-decoration:underline;">fs_getattr</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_getattr(const char *path, struct stat *stbuf) 
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
struct stat * stbuf - refers to underlying kernel stat structure to which all the stat attributes are assigned

Returns 0 on success, errno if call fails.
```


	

Defined in **_syscalls.c_**. fs_getattr function is used to get all the attributes of the stat structure. It internally calls a `set_stat` function, which sets the required parameters to the internal underlying kernel stat structure.

**8) <code><span style="text-decoration:underline;">fs_mkdir</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>

static int fs_mkdir(const char *path, mode_t mode)

<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
mode_t mode - The mode argument specifies the file permission bits to be used in creating the file

Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c. _**fs_mkdir function is used to create a directory. First we look for a free inode for the new directory. Then we get the inode of its parent and add an entry in the parent’s data block to show that the child belongs to that directory. Parent’s inode is updated accordingly. 

We look for free sectors in the data bitmap and assign a block of 16 contiguous sectors to the new directory. We then initialize the inode of the new directory ( set size, type, sector tuples etc.)

**9) <code><span style="text-decoration:underline;">fs_rmdir</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>

`static int fs_mkdir(const char *path`)

<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file

Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c. _**fs_rmdir function shall remove a directory whose name is given by path. The directory will be removed only if it is empty. The inodes of the directory to be removed and its parent are found. The child’s entry is removed from the parent directory and parent’s nlink count is reduced by 1. The data bitmap values of the sectors occupied by the child is changed to 0 to indicate that they are now free. The sector tuple values are reinitialized to -1. The inode type is set to free and all other inode values are set to default values to indicate that the inode is now free.

**10) <code><span style="text-decoration:underline;">fs_readdir</span></code></strong>

<span style="text-decoration:underline;">SIGNATURE</span>


```
static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
char * buf - buffer string where read content is passed into
int fuse_fill_dir_t(void *buf, const char *name, const struct stat *stbuf, off_t off);		- used to copy the underlying directory's filenames; name is the string to be copied
off_t offset - offset of the file 
struct fuse_file_info * fi - contains file information such has file handle and other flags.

Returns 0 on success, errno if call fails.
```


Defined in **_syscalls.c. _**fs_readdir function is used to read the contents of a directory. First the inode of the directory is located using the path and its stat structure is taken using the inode which is given to the filler function to write the current directory name “.” to buffer. Similarly its parent “..” is written to buffer. Next we run through the data block of the directory to find the names and inodes of the contents of the directory and write them to the buffer. 

**Additional functions:**

**1) <span style="text-decoration:underline;">getnodebypath</span>**

<span style="text-decoration:underline;">SIGNATURE</span>


```
int getnodebypath(const char *path, struct myinode *parent, struct myinode *child)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
struct myinode *parent	-inode of parent 
struct myinode *child -inode of child

Returns 0 on success, errno if call fails.
```


This function is used to retrieve the inode of the child given the path and the inode of the parent. If `parent` is not a directory, the  error number corresponding to the error ‘ENOTDIR’ is returned. 

We traverse through the path and extract the name of the directory following the ‘/’ until the next ‘/’ in the path name is encountered. Now, we iterate over the files and sub-directories included in its parent directory to retrieve the inode of the current directory. If the entry corresponding to the directory whose name we extracted is not found in the parent directory, the  error number corresponding to the error ‘ENOENT’ is returned. This function is recursively called by passing the remainder of the path and current directory as the parent until the inode of the child is found.

**2) <span style="text-decoration:underline;">inode_entry</span>**

<span style="text-decoration:underline;">SIGNATURE</span>


```
int inode_entry (const char *path, mode_t mode)
```


<span style="text-decoration:underline;">DESCRIPTION</span>


```
const char * path  - path to the file
mode_t mode - The mode argument specifies the file permission bits to be used in creating the file

Returns 0 on success, errno if call fails.
```


This function finds a free inode and initializes it for the file/directory provided by the path. If it is a directory, then it allocates 16 sectors and if it is a file it allocates 1 sector. It adds the entry of the newly created file/directory in the parent directory. It initializes the inode values and sector tuple values accordingly.

