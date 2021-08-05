all: pcat service1 service2

pcat: pcat.cpp
	${CXX} pcat.cpp -o pcat

1service1: service1.cpp
	${CXX} service1.cpp -o service1

service2: service2.cpp
	${CXX} service2.cpp -l pthread -o service2

