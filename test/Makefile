all: recovery

recovery:
	g++ -o recovery.exe recovery.cpp Senscape.cpp TestPrinter.h TestPrinter.cpp AssertListener.h AssertListener.cpp ../src/Dispatcher.h ../src/Dispatcher.cpp ../src/actuators/MainActuator.* ../src/actuators/PilotActuator.* -fpermissive -I ../src/ -I ../src/actuators/ -I./ -D__TEST__ -std=c++11

clean:
	rm *.exe