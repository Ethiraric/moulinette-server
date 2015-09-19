##
## Makefile for  in /home/sabour_f/github/moulinette-server
##
## Made by Florian SABOURIN
## Login   <sabour_f@epitech.net>
##
## Started on  Sat Sep 19 20:32:56 2015 Florian SABOURIN
## Last update Sat Sep 19 20:32:56 2015 Florian SABOURIN
##

# Executables
CC		=	gcc
CXX		=	g++
AR		=	ar rcs
RM		=	@rm -vf
MAKE		+=	--no-print-directory

# Names
NAME		=	mouliserver

# Flags
CFLAGS		=	-Wall -W -Wshadow -fno-diagnostics-show-caret -ggdb3 -Iinclude 
CXXFLAGS	=	-Wall -W -Wshadow -fno-diagnostics-show-caret -ggdb3 -Iinclude 
LDFLAGS		=	-lsqlite3 -lpthread 

# Files
CSRC		=	src/handle_client.c
CSRC		+=	src/database.c
CSRC		+=	src/run.c
CSRC		+=	src/main.c
CSRC		+=	src/aes.c


# Objects
OBJ		=	$(CSRC:.c=.o) $(CXXSRC:.cpp=.o)

# Rules
$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean:
	$(RM) $(OBJ) $(NAME)

re: fclean all

.PHONY: 	all clean fclean re

src/handle_client.o: src/handle_client.c include/mouli.h
	$(CC) $(CFLAGS) -c -o src/handle_client.o src/handle_client.c

src/database.o: src/database.c
	$(CC) $(CFLAGS) -c -o src/database.o src/database.c

src/run.o: src/run.c include/mouli.h
	$(CC) $(CFLAGS) -c -o src/run.o src/run.c

src/main.o: src/main.c include/database.h include/mouli.h
	$(CC) $(CFLAGS) -c -o src/main.o src/main.c

src/aes.o: src/aes.c
	$(CC) $(CFLAGS) -c -o src/aes.o src/aes.c

