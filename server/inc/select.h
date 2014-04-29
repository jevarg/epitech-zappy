/*
** FILE for FILE in /home/moriss_h/projet
**
** Made by hugues morisset
** Login   <moriss_h@epitech.net>
**
** Started on  Mon Oct  8 09:34:29 2012 hugues morisset
** Last update Mon Oct  8 16:20:21 2012 hugues morisset
*/

#ifndef SELECT_H_INCLUDED
# define SELECT_H_INCLUDED

# include <stdlib.h>
# include <stdio.h>
# include <sys/select.h>
# include <fcntl.h>
# include <errno.h>

# include "liste.h"

# define FDREAD 1
# define FDWRITE 2
# define ISREADABLE(x) (((x)->etype & FDREAD) == FDREAD)
# define ISWRITEABLE(x) (((x)->etype & FDWRITE) == FDWRITE)
# define CHECKREAD(x) ((x)->checktype |= FDREAD)
# define CHECKWRITE(x) ((x)->checktype |= FDWRITE)

typedef struct	s_selfd
{
  int		fd;
  int		etype;
  int		checktype;
  void		*data;
  void		(*callback)(struct s_selfd *this, void *data);
}		t_selfd;

void	do_select(t_list *fds, void *global_arg);
t_selfd	*create_fd(int fd, void *data, void (*call)());

#endif /* !SELECT_H_INCLUDED */