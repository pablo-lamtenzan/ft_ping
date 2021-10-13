# TO DO: Of course i need a better makefile to precced with this project

NAME=ft_ping
CFLAGS= -Wall -Wextra -Werror -lm #-g3 -fsanitize=address

SRCDIR = srcs

PARSE= $(addprefix parse/, parse_opts.c)
MAIN= $(addprefix main/, main.c)
LEGACY= $(addprefix legacy/, in_cksum.c in_tvsub.c)
CORE= get_hostaddr.c pinger.c resolve_dest_addr.c sig_handlers.c print_packet.c
DEBUG= debug.c 
ERROR_HANDLING= #error.c

SRCS= $(MAIN) $(ERROR_HANDLING) $(PARSE) $(LEGACY) $(CORE) $(DEBUG)
			
OBJDIR=bin
OBJS= $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

all: $(NAME)
	@echo Done.

$(NAME) : $(OBJDIR) $(OBJS)
	gcc -o $(NAME) $(CFLAGS) $(OBJS)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o : srcs/%.c
	mkdir -p $(shell dirname $@)
	gcc -c -o $@  -I./includes $<

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all
