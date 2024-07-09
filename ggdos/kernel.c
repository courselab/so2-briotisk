/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Gabriel  H. B. <gabriel_h_brioto@usp.br>
 *    SPDX-FileCopyrightText: 2024 Guilherme  C. M. <guilhermechiarotto@usp.br>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *    This file is part of SYSeg, available at https://gitlab.com/monaco/syseg.
 */

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"		/* For kwrite() etc.            */
#include "bios2.h"		/* For kread() etc.             */
#include "kernel.h"		/* Essential kernel functions.  */
#include "kaux.h"		/* Auxiliary kernel functions.  */

/* Kernel's entry function. */

void kmain(void)
{
  int i, j;
  
  register_syscall_handler();	/* Register syscall handler at int 0x21.*/

  splash();			/* Uncessary spash screen.              */

  shell();			/* Invoke the command-line interpreter. */
  
  halt();			/* On exit, halt.                       */
  
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
  int i;
  clear();
  kwrite ("GGDOS 1.0\n");

  while (go_on)
    {

      /* Read the user input. 
	 Commands are single-word ASCII tokens with no blanks. */
      do
	{
	  kwrite(PROMPT);
	  kread (buffer);
	}
      while (!buffer[0]);

      /* Check for matching built-in commands */
      
      i=0;
      while (cmds[i].funct)
	{
	  if (!strcmp(buffer, cmds[i].name))
	    {
	      cmds[i].funct();
	      break;
	    }
	  i++;
	}

      /* If the user input does not match any built-in command name, just
	 ignore and read the next command. If we were to execute external
	 programs, on the other hand, this is where we would search for a 
	 corresponding file with a matching name in the storage device, 
	 load it and transfer it the execution. Left as exercise. */
      
      if (!cmds[i].funct)
	kwrite ("Command not found\n");
    }
}


struct fs_header_t
{
  unsigned char  signature[FS_SIGLEN];    /* The file system signature.              */
  unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
  unsigned short number_of_boot_sectors;  /* Sectors reserved for boot code.         */
  unsigned short number_of_file_entries;  /* Maximum number of files in the disk.    */
  unsigned short max_file_size;		  /* Maximum size of a file in blocks.       */
  unsigned int unused_space;              /* Remaining space less than max_file_size.*/
} __attribute__((packed)) fs_header;      /* Disable alignment to preserve offsets.  */


/* Array with built-in command names and respective function pointers. 
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
  {
    {"help",    f_help},     /* Print a help message.       */
    {"quit",    f_quit},     /* Exit TyDOS.                 */
    {"exec",    f_exec},     /* Execute an example program. */
    {"list",    f_list},     /* Lists the disk files.     . */
    {0, 0}
  };


/* Build-in shell command: help. */

void f_help()
{
  kwrite ("...me, Obi-Wan, you're my only hope!\n\n");
  kwrite ("   But we can try also some commands:\n");
  kwrite ("      exec    (to execute an user program example\n");
  kwrite ("      quit    (to exit TyDOS)\n");
}

void f_quit()
{
  kwrite ("Program halted. Bye.");
  go_on = 0;
}

/*extern int main();*/
void f_exec()
{
  kwrite("Working on it...\n");
  //kwrite("type the filename: ");
	//kread (buffer);
//
  //kwrite ("executing you program...\n");			/* Call the user program's 'main' function. */
}

void f_list() 
{

  struct fs_header_t *fs_header = (struct fs_header_t *)0x7c00;

  unsigned short start_sect = fs_header->number_of_boot_sectors;
  unsigned short num_sect_read = fs_header->number_of_file_entries * 32 / 512; 

  extern unsigned char _MEM_POOL;
  	void *addr = (void *)&_MEM_POOL;


/*loads the disk*/
  __asm__ volatile(
      "pusha \n"
		  "mov boot_drive, %%dl \n"	/* Select the boot drive (from rt0.o). */
	    "mov $0x2, %%ah \n"		/* BIOS disk service: op. read sector. */
	    "mov %[sects_to_read], %%al \n"	/* Number of sectors to read.          */
	    "mov $0x0, %%ch \n"		/* Cylinder coordinate (starts at 0).  */
	    "mov %[fs_sect], %%cl \n"		/* Sector coordinate   (starts at 1).  */
	    "mov $0x0, %%dh \n"		/* Head coordinage     (starts at 0).  */
	    "mov %[fs_addr], %%bx \n"	/* Where to load the fs (rt0.o).   */
	    "int $0x13 \n"		/* Call BIOS disk service 0x13.        */
      "popa \n" ::
      [sects_to_read] "g"(num_sect_read),
      [fs_sect] "g"(start_sect),
      [fs_addr] "g"(addr)
    );

    for (int i = 0; i < fs_header->number_of_file_entries; i++) {
      char *fname = addr + i * 32;
      if (fname[0] && fname[0] != 32) {
        kwrite(fname);
        kwrite("\n");
      }
    }

}

