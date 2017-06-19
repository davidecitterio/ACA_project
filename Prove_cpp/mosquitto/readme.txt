Installing mosquitto libraries:
  sudo apt install libmosquitto-dev
  sudo apt install libmosquittopp-dev

Compiling mosquitto main:
  g++ main.cpp -o main -lmosquitto -lmosquittopp
