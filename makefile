install: netdot.cpp
	g++ -o netdot netdot.cpp
	mv netdot /usr/bin/

uninstall:
	rm /usr/bin/netdot

clean: 
	rm netdot