all: system tests

system: bin/console.o bin/Subsystem.o bin/Sensor.o bin/Actuator.o bin/SystemControl.o bin/Steering.o bin/Sonar.o bin/shirtt.o bin/Motor.o bin/GPS.o bin/Compass.o
	 arm-linux-gnueabi-g++ bin/*.o -o 0system/System.o -lpthread -lrt 

tests: test/bin/motor_basic_test.o test/bin/sonar_basic_test.o test/bin/compass_basic_test.o test/bin/sonar_compass_steering.o test/bin/motor_steering_compass.o

bin/Compass.o: Compass.cpp
	arm-linux-gnueabi-g++ -c Compass.cpp -o bin/Compass.o

bin/GPS.o: GPS.cpp
	arm-linux-gnueabi-g++ -c GPS.cpp -o bin/GPS.o

bin/Motor.o: Motor.cpp
	arm-linux-gnueabi-g++ -c Motor.cpp -o bin/Motor.o

bin/shirtt.o: shirtt.cpp
	arm-linux-gnueabi-g++ -c shirtt.cpp -o bin/shirtt.o

bin/Sonar.o: Sonar.cpp
	arm-linux-gnueabi-g++ -c Sonar.cpp -o bin/Sonar.o

bin/Steering.o: Steering.cpp
	arm-linux-gnueabi-g++ -c Steering.cpp -o bin/Steering.o

bin/Sensor.o: Sensor.cpp
	arm-linux-gnueabi-g++ -c Sensor.cpp -o bin/Sensor.o

bin/Actuator.o: Actuator.cpp
	arm-linux-gnueabi-g++ -c Actuator.cpp -o bin/Actuator.o

bin/Subsystem.o: Subsystem.cpp
	arm-linux-gnueabi-g++ -c Subsystem.cpp -o bin/Subsystem.o

bin/SystemControl.o: SystemControl.cpp
	arm-linux-gnueabi-g++ -c SystemControl.cpp -o bin/SystemControl.o

bin/console.o: console.cpp
	arm-linux-gnueabi-g++ -c console.cpp -o bin/console.o

test/bin/sonar_basic_test.o: test/Sonar/sonar_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/bin/sonar_basic_test.o test/Sonar/sonar_basic_test.cpp -lpthread -lrt

test/bin/motor_basic_test.o: test/Motor/motor_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/bin/motor_basic_test.o test/Motor/motor_basic_test.cpp -lpthread -lrt

test/bin/compass_basic_test.o: test/Compass/compass_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/bin/compass_basic_test.o test/Compass/compass_basic_test.cpp -lpthread -lrt

test/bin/sonar_compass_steering.o: test/integration/sonar_compass_steering.cpp
	arm-linux-gnueabi-g++ -o test/bin/sonar_compass_steering.o test/integration/sonar_compass_steering.cpp -lpthread -lrt

test/bin/motor_steering_compass.o: test/integration/motor_steering_compass.cpp
	arm-linux-gnueabi-g++ -o test/bin/motor_steering_compass.o test/integration/motor_steering_compass.cpp -lpthread -lrt

clean:
	rm -rf test/*/*.o bin/*.o 0system/*.o
