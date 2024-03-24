# Makefile for the Java RMI Assignment

# Target: make all
all:
	javac Client/*.java
	javac Server/*.java	
# Target: make rmi
rmi:
	rmiregistry

# Target: make server
server:
	cd Server && java RemoteServer

# Target: make client vigenere
client-vigenere:
	cd Client && java ClientVigenere

# Target: make client caesar
client-caesar:
	cd Client && java ClientCaesar

# Target: make clean
clean:
	rm -f Client/*.class Server/*.class
