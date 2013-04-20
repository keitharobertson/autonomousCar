all: system tests

system: console.o Subsystem.o Sensor.o Actuator.o SystemControl.o Steering.o Sonar.o shirtt.o Motor.o GPS.o Compass.o
	 arm-linux-gnueabi-g++ bin/*.o -o 0system/System.o -lpthread -lrt 

tests: motor_basic_test sonar_basic_test

Compass.o: Compass.cpp
	arm-linux-gnueabi-g++ -c Compass.cpp -o bin/Compass.o

GPS.o: GPS.cpp
	arm-linux-gnueabi-g++ -c GPS.cpp -o bin/GPS.o

Motor.o: Motor.cpp
	arm-linux-gnueabi-g++ -c Motor.cpp -o bin/Motor.o

shirtt.o: shirtt.cpp
	arm-linux-gnueabi-g++ -c shirtt.cpp -o bin/shirtt.o

Sonar.o: Sonar.cpp
	arm-linux-gnueabi-g++ -c Sonar.cpp -o bin/Sonar.o

Steering.o: Steering.cpp
	arm-linux-gnueabi-g++ -c Steering.cpp -o bin/Steering.o

Sensor.o: Sensor.cpp
	arm-linux-gnueabi-g++ -c Sensor.cpp -o bin/Sensor.o

Actuator.o: Actuator.cpp
	arm-linux-gnueabi-g++ -c Actuator.cpp -o bin/Actuator.o

Subsystem.o: Subsystem.cpp
	arm-linux-gnueabi-g++ -c Subsystem.cpp -o bin/Subsystem.o

SystemControl.o: SystemControl.cpp
	arm-linux-gnueabi-g++ -c SystemControl.cpp -o bin/SystemControl.o

console.o: console.cpp
	arm-linux-gnueabi-g++ -c console.cpp -o bin/console.o

sonar_basic_test: test/Sonar/sonar_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/Sonar/sonar_basic_test.o test/Sonar/sonar_basic_test.cpp -lpthread -lrt

motor_basic_test: test/Motor/motor_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/Motor/motor_basic_test.o test/Motor/motor_basic_test.cpp -lpthread -lrt

clean:
	rm -rf test/*/*.o bin/*.o 0system/*.o
