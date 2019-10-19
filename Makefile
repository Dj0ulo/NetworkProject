CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wshadow
CFLAGS += -Wextra
CFLAGS += -fstack-protector-all

OBJDIR = obj
SRCDIR = src

receiver: obj_dir obj/main.o obj/packet.o obj/socket.o obj/utils.o obj/connection.o obj/cons_manage.o
	$(CC) $(CFLAGS) -o receiver obj/main.o obj/packet.o -lz obj/socket.o obj/utils.o obj/connection.o obj/cons_manage.o
	
$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/packet.o: $(SRCDIR)/packet.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/socket.o: $(SRCDIR)/socket.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/utils.o: $(SRCDIR)/utils.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/connection.o: $(SRCDIR)/connection.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/cons_manage.o: $(SRCDIR)/cons_manage.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
#$(OBJDIR)/%.o: $(SRCDIR)/%.c obj_dir
	#$(CC) $(CFLAGS) -c -o $@ $<
	
obj_dir:
	[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)

all : receiver
clean:
	rm -rf $(OBJDIR) receiver
