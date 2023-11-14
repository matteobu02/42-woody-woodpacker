NAME		=	woody_woodpacker

ASM			=	nasm
ASMFLAGS	=	-f

CXX			=	gcc
CXXFLAGS	=	-Wall -Wextra -Werror -Ofast -I $(INCLUDE) -I libft/includes/ -g #-fsanitize=address

SRCDIR		=	./src
C_SRCDIR	=	$(SRCDIR)/c/
ASM_SRCDIR	=	$(SRCDIR)/asm/
OBJDIR		=	./obj/
INCLUDE		=	./include/
PAYLOADDIR	=	./payloads/
LIBFT		=	./libft/libft.a

C_SRC		=	main.c	\
				elf64.c	\
				elf32.c	\
				utils.c	\

CRYPT		=	rc4_elf64.s

OBJ			=	${addprefix $(OBJDIR), $(C_SRC:%.c=%.o)}
OBJ			+=	${addprefix $(OBJDIR), $(CRYPT:%.s=%.o)}

ASM_SRC		=	handler_elf64.s		\
				parasite_elf64.s	\
				rc4_elf64.s			\
				handler_elf32.s		\
				parasite_elf32.s	\
				rc4_elf32.s			\

PAYLOADS	=	${addprefix $(PAYLOADDIR), $(ASM_SRC:%.s=%.bin)}


# ===== #


all:				$(NAME)

$(NAME):			$(LIBFT) $(OBJDIR) $(OBJ) $(PAYLOADDIR) $(PAYLOADS)
					$(CXX) $(CXXFLAGS) $(OBJ) -Llibft -lft -z noexecstack -o $(NAME)

clean:
					rm -rf $(NAME) woody

fclean:				clean
					rm -rf $(OBJDIR) $(PAYLOADDIR)
					make -C libft/ fclean

re:					fclean all

$(LIBFT):
					make -C libft/

$(OBJDIR):
					@mkdir -p $(OBJDIR)

$(OBJDIR)%.o:		$(C_SRCDIR)%.c
					$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)%.o:		$(ASM_SRCDIR)%.s
					$(ASM) $(ASMFLAGS) elf64 $< -o $@

$(PAYLOADDIR):
					@mkdir -p $(PAYLOADDIR)

$(PAYLOADDIR)%.bin:	$(ASM_SRCDIR)%.s
					$(ASM) $(ASMFLAGS) bin $< -o $@

.PHONY:				re clean fclean obj all
