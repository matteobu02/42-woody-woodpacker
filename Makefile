NAME		=	woody_woodpacker
CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror -I $(INCLUDE) -I libft/includes/ -g #-fsanitize=address
SRCDIR		=	./src/
OBJDIR		=	./obj/
INCLUDE		=	./include/

SRC			=	main.c		\
				elf64.c		\
				crypto.c	\
				utils.c		\

OBJ			=	${addprefix $(OBJDIR), $(SRC:%.c=%.o)}


# ===== #


all:			$(NAME)

$(NAME):		$(OBJDIR) $(OBJ)
				make -C libft/
				$(CXX) $(CXXFLAGS) $(OBJ) -Llibft -lft -o $(NAME)

clean:
				rm -rf $(OBJDIR)

fclean:			clean
				rm -rf $(NAME)
				make -C libft/ fclean

re:				fclean all

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean obj all
