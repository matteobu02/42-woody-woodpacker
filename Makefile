NAME		=	woody_woodpacker

ASM			=	nasm
ASMFLAGS	=	-f

CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror -Ofast -I $(INCLUDE) -I libft/includes/ -g #-fsanitize=address

SRCDIR		=	./src/
OBJDIR		=	./obj/
INCLUDE		=	./include/
ENCRYPTION	=	./encryption/
LIBFT		=	./libft/libft.a

SRC			=	main.c	\
				woody.c	\
				utils.c	\

SRC_CRYPT	=	rc4.s	\

OBJ			=	${addprefix $(OBJDIR), $(SRC:%.c=%.o)}
OBJ			+=	${addprefix $(OBJDIR), $(SRC_CRYPT:%.s=%.o)}

HANDLER		=	./payloads/handler.s
PARASITE	=	./payloads/parasite.s
DECRYPTOR	=	./encryption/rc4.s


# ===== #


all:			$(NAME)

$(NAME):		$(LIBFT) $(OBJDIR) $(OBJ) $(HANDLER) $(PARASITE) $(DECRYPTOR)

				$(ASM) $(ASMFLAGS) bin $(HANDLER)
				$(ASM) $(ASMFLAGS) bin $(PARASITE)
				$(ASM) $(ASMFLAGS) bin $(DECRYPTOR) -o ./payloads/decryptor

				$(CXX) $(CXXFLAGS) $(OBJ) -Llibft -lft -z noexecstack -o $(NAME)

clean:
				rm -rf $(OBJDIR) woody

fclean:			clean
				rm -rf $(NAME) ./payloads/handler ./payloads/parasite ./payloads/decryptor
				make -C libft/ fclean

re:				fclean all

$(LIBFT):
				make -C libft/

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:	$(ENCRYPTION)%.s
				$(ASM) $(ASMFLAGS) elf64 $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean obj all
