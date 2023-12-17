# Makefile

CC = gcc

CFLAGS_SV = -Wall -Wextra -Werror -Wunused -pedantic -pthread
CFLAGS_C = -Wall -Wextra -Werror -Wunused -pedantic

SRC_C = client/*.c
SRC_SV = server/*.c

TARGET_C = user
TARGET_SV = AS

ALL: $(SRC) $(SRC_SV)
	$(CC) $(CFLAGS_C) $(SRC_C) -o $(TARGET_C)
	$(CC) $(CFLAGS_SV) $(SRC_SV) -o $(TARGET_SV)

clean:
	rm -f $(TARGET_C) $(TARGET_SV)