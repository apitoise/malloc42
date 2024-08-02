ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

CC			=	gcc
CC_FLAGS	=	-Wall -Wextra -Werror -fPIC
RM			=	rm -rf

DIR_HEADERS		=	./hdrs/

DIR_SRCS		=	./srcs/

DIR_OBJS		=	./compiled_srcs/

SUB_DIRS		=	.

SUB_DIR_OBJS	=	$(SUB_DIRS:%=$(DIR_OBJS)%)

SRCS			=	malloc.c \
					free.c \
					realloc.c \
					show_alloc_mem.c \
					zone.c \
					block.c \
					utils.c

OBJS		=	$(SRCS:%.c=$(DIR_OBJS)%.o)

NAME		=	libft_malloc.so

DYNAMIC_LIB =	libft_malloc_$(HOSTTYPE).so

all:			$(NAME)

$(NAME):		$(DYNAMIC_LIB)
				@ln -sf $(DYNAMIC_LIB) $(NAME)

$(DYNAMIC_LIB):	$(OBJS)
				@$(CC) $(CC_FLAGS) $(OBJS) -shared -o $(DYNAMIC_LIB)

$(OBJS):		| $(DIR_OBJS)

$(DIR_OBJS)%.o: $(DIR_SRCS)%.c
				@$(CC) $(CC_FLAGS) -I $(DIR_HEADERS) -c $< -o $@

$(DIR_OBJS):	$(SUB_DIR_OBJS)

$(SUB_DIR_OBJS):
				@mkdir -p $(SUB_DIR_OBJS)

clean:
				@$(RM) $(DIR_OBJS)

fclean:			clean
				@$(RM) $(NAME)
				@$(RM) $(DYNAMIC_LIB)

norm:
				@$(NORMINETTE) $(DIR_SRCS)
				@$(NORMINETTE) $(DIR_HEADERS)

re:				fclean all

.PHONY:			all clean fclean re norm
