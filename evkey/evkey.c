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
static int key;
static int command;
static int timeout;


static int
key_check( int key )
{
  unsigned char keys[KEY_MAX / 8 + 1];
  int state;

  if ( key > KEY_MAX )
    return 1;

  memset(keys,0,sizeof(keys));
  ioctl(fd, EVIOCGKEY(sizeof(keys)), keys);

  state = ( keys[ key >> 3 ] & (1<<(key&7)) ) ? 1 : 0;

  printf("%d\n", state);

  return 0;
}


static int
sw_check( int key )
{
  unsigned char keys[SW_MAX / 8 + 1];
  int state;

  if ( key > SW_MAX )
    return 1;

  memset(keys,0,sizeof(keys));
  ioctl(fd, EVIOCGSW(sizeof(keys)), keys);

  state = ( keys[ key >> 3 ] & (1<<(key&7)) ) ? 1 : 0;

  printf("%d\n", state);

  return 0;
}


static int 
key_wait( int down )
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
  printf("usage: evkey <-u|-d|-k key|-s sw> <-t timeout> eventdevice\n");
}


int
main ( int argc, char *argv[] )
{
  int c;
  
  while ( (c = getopt(argc, argv, "dut:s:k:h")) != -1 )
  {
    switch (c)
    {
      case 'd':
      case 'u':
	command = c;
	break;

      case 's':
      case 'k':
	command = c;
	key = atoi(optarg);
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
    timeout = 100;
  }
  
  if (!command)
  {
    fprintf(stderr,"one of u,d,k,s options is needed\n");
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
    exit(-1);
  }
  
  switch (command)
  {
    case 'd':
      return key_wait(WAIT_DOWN);
      break;
    case 'u':
      return key_wait(WAIT_UP);
      break;
    case 'k':
      return key_check(key);
      break;
    case 's':
      return sw_check(key);
      break;
  }
  
  return 1;
}

