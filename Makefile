##
## Makefile for  in /home/sabour_f/github/moulinette-server
##
## Made by Florian SABOURIN
## Login   <sabour_f@epitech.net>
##
## Started on  Mon Jan 25 15:36:35 2016 Florian SABOURIN
## Last update Mon Jan 25 15:36:35 2016 Florian SABOURIN
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
CFLAGS		=	 -Wall -W -Wshadow -fno-diagnostics-show-caret -D_GNU_SOURCE  -Iinclude 
CXXFLAGS	=	 -Wall -W -Wshadow -fno-diagnostics-show-caret -D_GNU_SOURCE  -Iinclude 
LDFLAGS		=	 -lsqlite3 -lpthread -lcurl 

# Files
CSRC		=	src/aes.c
CSRC		+=	src/auth.c
CSRC		+=	src/config.c
CSRC		+=	src/database.c
CSRC		+=	src/handle_client.c
CSRC		+=	src/main.c
CSRC		+=	src/register.c
CSRC		+=	src/run.c

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

src/aes.o: src/aes.c
	$(CC) $(CFLAGS) -c -o src/aes.o src/aes.c

src/auth.o: src/auth.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/auth.o src/auth.c

src/config.o: src/config.c include/config.h
	$(CC) $(CFLAGS) -c -o src/config.o src/config.c

src/database.o: src/database.c include/database.h
	$(CC) $(CFLAGS) -c -o src/database.o src/database.c

src/handle_client.o: src/handle_client.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/handle_client.o src/handle_client.c

src/main.o: src/main.c include/database.h include/mouli.h include/config.h
	$(CC) $(CFLAGS) -c -o src/main.o src/main.c

src/register.o: src/register.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/register.o src/register.c

src/run.o: src/run.c include/mouli.h include/database.h include/config.h
	$(CC) $(CFLAGS) -c -o src/run.o src/run.c

