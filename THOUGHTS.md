# Some comments and thoughts throughout the learning progress

* Many DIY-level sensors are inexpensive and quickly delievered (e.g., via Amazon), but the quality is not guarateed and a detail specification may be unavailable. 
The (uncertainity in) quality issue may be worsen if only one sensor is bought, which a comparison or backup is not possible.
* A comprehensive learning about electronic components and IOT is not easy nor direct. Some are manageable, but some are not.
Transistor is an example of those complicated components to me. 
While the analogy of water pipe can kinda explain the function of transistor as a switch to limit current (and that across a certain limit turning the switch will not result in a greater current),
as well as the NPN or PNP chemical reaction effect from dopping electron giving/hungry elements, it seems like I am (at least) a step away from getting the whole picture.
(so i know the function...but how does it work?)
* The module was great and many items were introduced. It would be greater, if more hands-on exemples and demonstrations were offered. This could be for example, the use of strip board to make a circuit.
I believe that will be even more impressive and informative. I think many things are better learnt by seeing and experiencing, and many things can be learnt are unplanned, such as de-soldering.
* The attempt to log MQTT data via Pythin library Paho-mqtt is still unsuccessful in my PC. It could be a directory path reason or others.
Hope one day I will be capable to understand the problem and fix that. 

* **Updated 9PM 12/1/2022:** I have not given up yet and attempted to work on it again, with Google Colab which was introduced in another module (DSN) today, and I have some advancement. I suspected one possible problem was syntax style, *from paho.mqtt import client as mqtt_client* does not give an error while *Import paho.mqtt.client as mqtt* from a tutorial which will give **invalid syntax**. I was running the script from [Dekun Tao](https://www.emqx.com/en/blog/how-to-use-mqtt-in-python) with changes in mqtt connection and subscription parameters. [Steves Internet Guide](http://www.steves-internet-guide.com/subscribing-topics-mqtt-client/) also provides many guides about MQTT. 
The Paho-mqtt library is very handy and the steps are rather straight forward (connect and subscribe). The topic is being refreshed adequately while previous records are kept. The next step will be to save the data locally and extract valuable information from it. Apart from storing data from a RPi, this offers another way to log data, which I think will be quite helpful for situation when RPi is not easily available (poor student?) and also when computer/server is available, like an indoor environment.

<p align="center">
<img src="https://user-images.githubusercontent.com/91855312/149216382-b7c94c29-a5df-4b9f-808e-b7399a293284.png" width=70% height=70%>
</p>

(slightly) relieved

