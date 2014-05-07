toyfs
=====

toy filesystem in c++;
Cris Capdevila;
Alexander Saites


What is it?
-----------
It's a toy file system, implemented in c++. It imitates the Unix inode file
system with a fake disk (a flat file) and offers several commands.

How do I compile it?
--------------------
With Clang 3.0+ and llvm installed, simply call "make". You can also use 'make
debug', in which case executing the program will allow it to test some simple 
test cases. If you do so, you can pipe the output to another file and diff it 
with 'CorrectOutput.txt':

    > make clean
    > make debug
    > ./main fsFile > output
    > diff output CorrectOutput
    
You'll find that the inode numbers are likely different on your system.    

How do I run it?
----------------
Although our filesystem is not persistent, you can choose the name of the
working file to ensure it does not clash with any other files in your working
directory.

    Run the program: ./main workingFileName

Since we read commands on stdin and output to stdout and stderr, you can 
redirect input and output as you would any other unix program:
    
    With redirection: ./main filename < inputfile > outputfile

What commands can I use?
------------------------
Our ToyFS supports many commands:

    open filename mode:
        Open a file and return a file descriptor
            filename: the name of the file; if openning for reading, the 
                      file must exist
                mode: one of w, r, rw:
                      w: open for writing
                      r: open for reading
                      rw: open for reading and writing
    
    close fd:
        Closes an open file descriptor fd

    read fd bytes
        Reads bytes from fd, starting from the current position. If bytes
        exceeds the number of bytes in fd, an error is reported. The file
        must be open for reading.

    write fd "some string"
        Writes "some string" to the file pointed to by fd, starting from the
        current location. The file must be open for writing.

    seek fd pos
        Seeks to pos in the file pointed to by fd.

    link src dest
        Creates a new file, dest, that points to the same inode as src. Src and
        dest must be in different directories.

    unlink file
        Removes the link to a given inode ("deletes" this file from a 
        directory). If the inode is no longer pointed to by any file, it is
        deleted, and it's memory is placed back in the free list.

    stat file1 [file2, file3, ...]
        Returns some information about a file or directory.

    cat file
        Prints the contents of a file.

    import src dest
        Imports a text file from the host operating system, giving it the name
        dest.

    export src dest
        Exports the text file src to the host operating system, giving it the
        name dest.

    cp src dest
        Create a new file dest with the same contents as src.

    mkdir dir1 [dir2, dir3, ...]
        Makes a directory by the name given, starting from the current location
        (unless an absolute location is given). Our file system supports "."
        for the current location and ".." for the parent directory.

    rmdir dir1 [dir2, ...]
        Removes the directory by the name given, provided that directory is
        empty.

    cd directory
        Changes the current working directory to the name given

    pwd
        Prints the current working directory path, starting from root.

    ls
        List the contents of the current working directory.

    tree
        List the contents of the current directory and every subdirectory.


Design Decisions
----------------
Unsurprisingly, the most important decision we made was the actual setup of our
file system. For ease of implmentation, we focused more on replicating basic
Unix ideas and FS structure. We are able to do much of this in memory, given
that we have such a small disk. This, coupled with the lack of need for a 
persistent file system lead to our decision to hold the free node list and 
inodes in memory, simplifying the overall implementation. Further, although
we chose not to, persistence could be implemented relatively easily by 
serializing our freelist and outputing it to "disk". 

Like the Unix file structure, all files and directories are, at their core, the
same basic class: the DirEntry. A flag in DirEntry determines its type, which
for our system is just a file or directory. They both have a name, but a 
directory just consists of a list of pointers to other directories, whereas a
file holds a pointer called an inode which references actual blocks on the 
disk. By breaking the pointers to blocks away from DirEntries, multiple files
can share the same inode, allowing linking. DirEntries also hold some other
useful information, including a pointer to its parent and its total size (if
its a file).

As stated, DirEntries represent files and point to inodes. An inode keeps track
of its size, the number of blocks it's using, and (most importantly), direct
and indirect pointers to blocks on the disk. Blocks contain the actual data
that makes up a file, and serves as the basic unit of storage in our file
system. We chose blocks of size 1024 bytes, which really just seemed like a 
decent block size. To determine a "better" block size, we would need to collect
accurate usage statistics similar to those we examined in class.

Inodes get blocks from the free list, a list of free nodes that represent 
blocks on the disk that are currently not pointed to by any inode. When a file
grows in size, new blocks are allocated to it from the free list. These blocks
are first pointed to by direct pointers, and then by indirect pointers as the
file grows, balancing performance for a compact representation. If a file is
deleted (by virtue of no more DirEntries hold a pointer to its inode), the 
blocks the file was using are marked as free, and returned to the free list.
Doing so allows other files to use this space if needed.

We focused other portions of our file system on ease-of-writing, including 
handing off portions of code to "helper" functions the implement "basic"
versions of reading, writing, and opening files. Doing so allows cp, cat, 
and other commands to take advantage of their shared implementation. Overall
is a simple file system that emulates many features of the Unix file system
design.
