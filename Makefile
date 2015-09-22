##
## Makefile for  in /home/sabour_f/github/moulinette-server
##
## Made by Florian SABOURIN
## Login   <sabour_f@epitech.net>
##
## Started on  Tue Sep 22 10:37:14 2015 Florian SABOURIN
## Last update Tue Sep 22 10:37:14 2015 Florian SABOURIN
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
CFLAGS		=	-Wall -W -Wshadow -fno-diagnostics-show-caret -D_GNU_SOURCE -Iinclude 
CXXFLAGS	=	-Wall -W -Wshadow -fno-diagnostics-show-caret -D_GNU_SOURCE -Iinclude 
LDFLAGS		=	-lsqlite3 -lpthread -lcurl 

# Files
CSRC		=	src/handle_client.c
CSRC		+=	src/database.c
CSRC		+=	src/run.c
CSRC		+=	src/config.c
CSRC		+=	src/auth.c
CSRC		+=	src/register.c
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

src/handle_client.o: src/handle_client.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/handle_client.o src/handle_client.c

src/database.o: src/database.c include/database.h
	$(CC) $(CFLAGS) -c -o src/database.o src/database.c

src/run.o: src/run.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/run.o src/run.c

src/config.o: src/config.c include/config.h
	$(CC) $(CFLAGS) -c -o src/config.o src/config.c

src/auth.o: src/auth.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/auth.o src/auth.c

src/register.o: src/register.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/register.o src/register.c

src/main.o: src/main.c include/database.h include/mouli.h include/config.h
	$(CC) $(CFLAGS) -c -o src/main.o src/main.c

src/aes.o: src/aes.c
	$(CC) $(CFLAGS) -c -o src/aes.o src/aes.c

