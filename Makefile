NAME		=	woody_woodpacker

ASM			=	nasm
ASMFLAGS	=	-f

CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror -Ofast -I $(INCLUDE) -I libft/includes/ -g #-fsanitize=address

SRCDIR		=	./src/
OBJDIR		=	./obj/
INCLUDE		=	./include/
ENCRYPTION	=	./encryption/

SRC			=	main.c	\
				woody.c	\
				utils.c	\

SRC_CRYPT	=	rc4.s	\

OBJ			=	${addprefix $(OBJDIR), $(SRC:%.c=%.o)}
OBJ			+=	${addprefix $(OBJDIR), $(SRC_CRYPT:%.s=%.o)}

PAYLOAD_SO	=	./payloads/parasite_so.s
PAYLOAD_EXE	=	./payloads/parasite_exec.s
PAYLOAD_RC4	=	./payloads/decryptor.s


# ===== #


all:			$(NAME)

$(NAME):		$(OBJDIR) $(OBJ) $(PAYLOAD_SO) $(PAYLOAD_EXE) $(PAYLOAD_RC4)
				make -C libft/

				$(ASM) $(ASMFLAGS) bin $(PAYLOAD_SO)
				$(ASM) $(ASMFLAGS) bin $(PAYLOAD_EXE)
				$(ASM) $(ASMFLAGS) bin $(PAYLOAD_RC4)

				$(CXX) $(CXXFLAGS) $(OBJ) -Llibft -lft -z noexecstack -o $(NAME)

clean:
				rm -rf $(OBJDIR) woody

fclean:			clean
				rm -rf $(NAME) ./payloads/parasite_so ./payloads/parasite_exec ./payloads/decryptor
				make -C libft/ fclean

re:				fclean all

$(OBJDIR)%.o:	$(SRCDIR)%.c
				$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:	$(ENCRYPTION)%.s
				$(ASM) $(ASMFLAGS) elf64 $< -o $@

$(OBJDIR):
				@mkdir -p $(OBJDIR)

.PHONY:			re clean fclean obj all
