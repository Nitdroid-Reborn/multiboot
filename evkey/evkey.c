/*

  Quick hack to get key codes and key state from kernel input subsystem

  Copyright (c) 2006 Frantisek Dufka <dufkaf@seznam.cz>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>

#include <linux/input.h>


#define WAIT_DOWN  1
#define WAIT_UP    0

static int fd;
static int command;
static int timeout;


static int 
keywait( int down )
{
  struct pollfd inpollfd;
  struct input_event ev;
  int err,rb;
  
  memset(&inpollfd,0,sizeof(inpollfd));
  
  inpollfd.fd = fd;
  inpollfd.events = POLLIN;
  
  while (1)
  {
    err = poll(&inpollfd,1,timeout);
    if ( err<1 )
    {
      return 1;
    }
    
    rb = read(fd,&ev,sizeof(struct input_event));
    if ( rb<sizeof(struct input_event) )
    {
      return 1;
    }
    else 
    {
      if ( ev.type == EV_KEY && ev.value == down )
      {
	printf("%d\n", ev.code);
	return 0;
      }
    }
  }

  return 1;
}


static void 
usage()
{
  printf("usage: evkey <-u|-d> <-t timeout> eventdevice\n");
}


int
main ( int argc, char *argv[] )
{
  int morethanone=0;
  int c;
  
  while ( (c = getopt(argc, argv, "dut:h")) != -1 )
  {
    switch (c)
    {
      case 'd':
      case 'u':
	if (!command) 
	  command = c;
	else
	  morethanone = c;
	break;
	
      case 't':
	timeout = atoi(optarg);
	break;

      case 'h':
      case '?':
	usage();
	break;
	
      default:
	printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }
  
  if (!timeout) 
  {
    timeout=100;
  }
  
  if (!command)
  {
    fprintf(stderr,"one of u,d options is needed\n");
    exit(-1);
  }
  
  if (morethanone)
  {
    fprintf(stderr,"only one of u,d options is allowed\n");
    exit(-1);
  }
  
  if (optind >= argc)
  {
    fprintf(stderr,"missing input device name\n");
    exit(-1);
  }
  
  if ( (fd = open (argv[optind], O_RDONLY)) < 0 )
  {
    perror("evdev open");
  }
  
  switch (command)
  {
    case 'd':
      return keywait(WAIT_DOWN);
      break;
    case 'u':
      return keywait(WAIT_UP);
      break;
  }
  
  return 1;
}

