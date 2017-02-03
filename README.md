# SimpleWebServer for HullPixelbot
A simple web server for Wemos devices that accepts query strings and sends them to the robot

You can run this program to allow yourself to control a HullPixelbot from a web browser. The server uses mDNS to advertise it's prescence on a local network. If you have a device running the Apple Bonjour service it will be able to discvoer the robot server as local.HullPixelbot

You can change the name of the device in the code. 

The server will pass robot commands that are received out to a robot motor contorller which may be attached to the serial port on the Wemos device.

The server uses HullPixelbot Code commands to communicate with the robot. These will cause the robot pixel to change colour according to status. Red and blue flashing indicates that the robot is searching for an access point. A white pixel indicates that the mDNS server could not be started. When the robot is connected and ready to receive commands the pixel will turn green. 

You can find full details of the HullPixelbot Code langauge [here:](https://github.com/HullPixelbot/HullPixelbotCode) 

Rob Miles
