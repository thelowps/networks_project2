David Lopes and Andi Cescolini 

Client:
The client reads the configuration file, updates the sensor packets accordingly (with current, min, and max temperature information, as well as the action status), and then sends to the server after serializing the packet into a string.  The serialization code is found in the sensor.c file.  When the action is set to 1 (for the extra credit) it will then wait to receive a status message back.  The status will be printed to the terminal regarding the overtemp.  All of the networking code is taken care of in the eztcp.c file; all of the network calls reference this library.  

-eztcp.h (header file for our TCP handling code)
-eztcp.c (implementation of the TCP handling functions)
-sensor.h (A library to do a variety of things related to sensor data and manipulating it)
-sensor.c (A library to do a variety of things related to sensor data and manipulating it)
-therm.c (A simple client file that reads data from thermal sensors, packs them into an appropriate structure, and sends the files to a client)
-Makefile

Server: 
The server listens on a port and will fork once it receives an incoming connection.  The child process calls handle_client(), which reads the packets, deserializes, and prints to the temp_log accordingly if the action is set to 0.  If the action is set to 1 (extra credit), it will not write but will check the current temp against the maximum temp and send an appropriate status back to the client.  If it sends a 0 back to the client, the temperature is okay. If it sends a 1, the temperature is too high.  The connection for that client is then closed.  

-eztcp.h (header file for our TCP handling code)
-eztcp.c (implementation of the TCP handling functions)
-sensor.h (A library to do a variety of things related to sensor data and manipulating it)
-sensor.c (A library to do a variety of things related to sensor data and manipulating it)
-thermd.c (A simple multithreaded TCP server that accepts data from clients and organizes the data in files)
-Makefile




We completed extra credit 1 with the overtemp status. 
