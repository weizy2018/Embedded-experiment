#include <sys/types.h>                                                  
#include <sys/stat.h>                                                      
#include <fcntl.h>                                                       
#include <termios.h>                                                    
#include <stdio.h>  
#ifndef  __TTY_H__
#define __TTY_H__
int tty_init();
int tty_end();
int tty_read(char *buf,int nbytes);
int tty_write(char *buf,int nbytes);
int tty_writecmd(char *buf,int nbytes);
void tty_fflush();
#endif

