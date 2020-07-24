# ESP8266_NRF_controller
This Repo contains the Arduino Code for an ESP-12E module connected to an NRF24L01 and an MPU6050 IMU.
The Purpose is to reduce the power consumption by using NRF24L01 with a toned down protocol and mainly to be compatible with RC cars working with NRF24L01, however you can certainly use the espnow protocol or some other methods like Websockets but this controller is specifically for the purpose of interfacting NRF24L01 with ESP8266.
Make sure you import the RF24 library link [here](https://github.com/nRF24/RF24).
