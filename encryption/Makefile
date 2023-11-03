NAME		=	test
CXX			=	gcc
CXXFLAGS	=	-m32 -Wall -Wextra -Werror -g
ASM			=	nasm -felf32 -g
OBJDIR		=	./obj/

ASM_SRC		=	rc4_32.s
C_SRC		=	test.c

OBJ			=	${addprefix $(OBJDIR), $(C_SRC:%.c=%.o)}
OBJ			+=	${addprefix $(OBJDIR), $(ASM_SRC:%.s=%.o)}


# ===== #


all:			$(NAME)

$(NAME):		$(OBJDIR) $(OBJ)
				$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
				rm -rf $(OBJDIR)

fclean:			clean
				rm -rf $(NAME)

re:				fclean all

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:	$(SRCDIR)%.s
				$(ASM) $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean obj all

