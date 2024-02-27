CC = cl.exe
LD = link.exe
CFLAGS = 	/Od /Zi /FAsu /std:c17 /permissive- /W4 /WX 		\
					/w14242 /w14254 /w14287 /we4289 /w14296 /w14311 \
					/w14545 /w14546 /w14547 /w14549 /w14555 /w14619 \
					/w14640 /w14826 /w14905 /w14906
LDFLAGS = /DEBUG /MACHINE:X64 /ENTRY:wWinMainCRTStartup /SUBSYSTEM:WINDOWS
LDLIBS = user32.lib gdi32.lib
PREFIX = ../src/

all: app.exe

app.exe: app.obj
	$(LD) $(LDFLAGS) /OUT:app.exe app.obj $(LDLIBS)

app.obj: $(PREFIX)app.c
	$(CC) /c $(CFLAGS) $(PREFIX)app.c

clean:
	rmdir /q /s build
