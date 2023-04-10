## IoT-River-Pollution-Monitoring
This project was a part of a proof of concept demo for water pollution monitoring using 2 ESP8266 and DHT sensors for EEE-F411 done at BITS Pilani.
## BUILD FLOW STEPS -
### Interfacing sensors
DHT 11 sensor for temperature & humidity sensing.The DHT11 is a basic, ultra-low-cost digital temperature and humidity sensor. It uses a capacitive humidity sensor and a thermistor to measure the surrounding air and gives out a digital signal on the data pin (no analog input pins needed). We also use an Ultrasonic sensor to take distance values in place of a pH sensor. This is done at both master and slave nodes.
### Connecting Devices 
2 ESP8266 modules communicate with each other using ESPnow MAC protocol without any external support, the master node sends data to the slave which in turn communicates both its data and the slave's data to the firebase using a Wifi hotspot. Both the inter-node communication and firebase communication happen simultaneously.
### Website
This is a static website hosted on a local development server that displays the change in the data updated in real-time. 
1. Create a virtual environment using: python -m venv venv
2. Access the virtual environment: venv\Scripts\activate
3. Install the dependencies: pip install -r requirements.txt
3. Run the flask server: python server.py
4. Open port 5000 to view the website.
### Workflow
![image](https://user-images.githubusercontent.com/60233052/230834939-57ab8cfe-66a0-45ad-8f12-2e54359729f7.png)
