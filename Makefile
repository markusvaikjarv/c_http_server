TARGET = c_http_server
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic

$(TARGET): server.c
	$(CC) $(CFLAGS) server.c -o $@ 
clean:
	rm -f $(TARGET)