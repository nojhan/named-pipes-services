all: pcat service service2

pcat: pcat.cpp
	clang++ pcat.cpp -o pcat

service: service.cpp
	clang++ service.cpp -o service

service2: service2.cpp
	clang++ service2.cpp -l pthread -o service2

