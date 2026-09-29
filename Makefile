CC = gcc  # compiler
CFLAGS = -Wall  # flags
TARGET = fs_indexed  # final executable file

all: $(TARGET)

$(TARGET): main.c fs_indexed.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c fs_indexed.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
