NAME = ft_ping
SRC = $(addprefix $(DIR_SRC), ft_ping.c headers.c utils.c init.c run.c end.c parse_arg.c echo.c receive.c)
FLAGS = 
DEPFLAGS = -MMD -MP
DIR_OBJ = tmp/
DIR_SRC = src/
OBJ = $(addprefix $(DIR_OBJ), $(notdir $(SRC:.c=.o)))
DEP = $(OBJ:.o=.d)

all: $(DIR_OBJ) $(NAME)

$(DIR_OBJ):
	mkdir -p $@

$(NAME): $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $(NAME) -lm

$(DIR_OBJ)%.o: $(DIR_SRC)%.c Makefile 
	gcc $(FLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	rm -fr $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP)