all: test

test:
	g++ -o test.exe main.cpp Senscape.cpp TestPrinter.h TestPrinter.cpp ../src/Dispatcher.h ../src/Dispatcher.cpp ../src/actuators/MainActuator.* ../src/actuators/PilotActuator.* -fpermissive -I ../src/ -I ../src/actuators/ -I./ -D__TEST__ -std=c++11