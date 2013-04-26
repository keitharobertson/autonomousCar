all: system tests

system: bin/console.o bin/Subsystem.o bin/Sensor.o bin/Actuator.o bin/SystemControl.o bin/Steering.o bin/Sonar.o bin/shirtt.o bin/Motor.o bin/GPS.o bin/Compass.o
	 arm-linux-gnueabi-g++ bin/*.o -o 0system/System.o -lpthread -lrt 

tests:  motor_tests sonar_tests compass_tests steering_tests integration_tests

motor_tests: test/bin/motor_basic_test.o test/bin/motor_fine_control.o

sonar_tests: test/bin/sonar_basic_test.o

compass_tests: test/bin/compass_basic_test.o test/bin/compass_square.o

steering_tests: test/bin/steer_basic.o test/bin/steer_fine_control.o

integration_tests: \
	test/bin/sonar_compass_steering.o \
	test/bin/motor_steering_compass.o \
	test/bin/steering_motor_spiral.o \
	test/bin/straight_line_no_compass.o \
	test/bin/compass_steering.o \
	test/bin/sonar_motor.o

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
	
test/bin/motor_fine_control.o: test/Motor/motor_fine_control.cpp
	arm-linux-gnueabi-g++ -o test/bin/motor_fine_control.o test/Motor/motor_fine_control.cpp -lpthread -lrt

test/bin/compass_basic_test.o: test/Compass/compass_basic_test.cpp
	arm-linux-gnueabi-g++ -o test/bin/compass_basic_test.o test/Compass/compass_basic_test.cpp -lpthread -lrt

test/bin/sonar_compass_steering.o: test/integration/sonar_compass_steering.cpp
	arm-linux-gnueabi-g++ -o test/bin/sonar_compass_steering.o test/integration/sonar_compass_steering.cpp -lpthread -lrt

test/bin/motor_steering_compass.o: test/integration/motor_steering_compass.cpp
	arm-linux-gnueabi-g++ -o test/bin/motor_steering_compass.o test/integration/motor_steering_compass.cpp -lpthread -lrt

test/bin/compass_steering.o: test/integration/compass_steering.cpp
	arm-linux-gnueabi-g++ -o test/bin/compass_steering.o test/integration/compass_steering.cpp -lpthread -lrt

test/bin/compass_square.o: test/integration/compass_square.cpp
	arm-linux-gnueabi-g++ -o test/bin/compass_square.o test/integration/compass_square.cpp -lpthread -lrt

test/bin/steer_basic.o: test/Steering/steer_basic.cpp
	arm-linux-gnueabi-g++ -o test/bin/steer_basic.o test/Steering/steer_basic.cpp -lpthread -lrt

test/bin/steer_fine_control.o: test/Steering/steer_fine_control.cpp
	arm-linux-gnueabi-g++ -o test/bin/steer_fine_control.o test/Steering/steer_fine_control.cpp -lpthread -lrt

test/bin/steering_motor_spiral.o: test/integration/steering_motor_spiral.cpp
	arm-linux-gnueabi-g++ -o test/bin/steering_motor_spiral.o test/integration/steering_motor_spiral.cpp -lpthread -lrt

test/bin/straight_line_no_compass.o: test/integration/straight_line_no_compass.cpp
	arm-linux-gnueabi-g++ -o test/bin/straight_line_no_compass.o test/integration/straight_line_no_compass.cpp -lpthread -lrt

test/bin/sonar_motor.o: test/integration/sonar_motor.cpp
	arm-linux-gnueabi-g++ -o test/bin/sonar_motor.o test/integration/sonar_motor.cpp -lpthread -lrt

clean:
	rm -rf test/*/*.o bin/*.o 0system/*.o
