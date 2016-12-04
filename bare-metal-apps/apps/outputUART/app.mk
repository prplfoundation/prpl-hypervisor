# Define your additional include paths
#INC_DIRS += 

#Aditional C flags
#CFLAGS += 

#Aditional Libraries
#LIBS +=

#default stack size 512 bytes
STACK_SIZE = 512

#Include your additional mk files here. 

app:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/$(APP).c -o $(TOPDIR)apps/$(APP)/$(APP).o

