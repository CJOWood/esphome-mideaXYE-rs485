# esphome-mideaXYE-rs485
ESPhome files to communicate with Midea AC units via RS485 XYE (CCM) terminals.

This project will allow you to wirelessly communicate with your Midea AC unit via the XYE terminals.  These terminals are normally used for communication with a Central Controller Module (CCM) but the data has been reverse engineered (https://codeberg.org/xye/xye).  I initially used the code from bunicutz project https://github.com/Bunicutz/ESP32_Midea_RS485/tree/main, but as of ESPHome 2025.2.0 custom components are no longer supported and that code does not work.  This project doesn't use custom components, just several lambda functions and a header file.  I'm not a C++ developer, so forgive me if the code isn't very elegant. It gets the job done though!

# Hardware
1. ESPHome device
2. RS-485 transceiver with **automatic flow control** (if not you will need to edit my code to switch flow control manually after serial read/write)
3. Midea-like heat pump or air handler with **XYE terminals**.
  
1. Connect ESP Vin, GND, GPIO16 to RX, and GPIO17 to TX on the UART side of the transceiver (can edit these in the header file if necessary).
2. On the RS-485 transceiver connect A to X, B to Y, and GND to E (optional).

# Software
This was intended for use with Home Assistant, the YAML file is for ESPHome. 

1. Create a new ESPHome device and configure it with your wifi info
2. Paste the contents of "midea-xye.yaml" under the auto-populated info in your new ESPHome device.
3. Under the esphome yaml header in your file add:
```
includes:
  - xyeVars.h
```
4. Copy "xyeVars.h" into your /config/esphome directory (or wherever your ESPhome YAML files are in your setup).
5. Edit the #define section of the xyeVars.h file to match your GPIO pins for RX and TX (if necessary).
6. Change `internal: True` to False under the thermostat section if you want ESPHome to create a thermostat entity
7. When I changed my wired controller to Fahrenheit it seems that internally the air handler / heat pump is using all degrees Fahrenheit now.  If you're using Celsius then comment out the Fahrenheit lines and uncomment Celsius lines in the yaml file. As long as you have your default units set correctly in Home Assistant AND your "wired controller" for your unit is reading out in Celsius then everything should be fine, no conversions or math necessary.

Note: The ESPhome thermostat entity kind of sucks. Because the heat pump can heat and cool, the displayed thermostat shows up as a dual-setpoint thermostat which isn't really accurate.  I recommend creating your own template thermostat with the custom entity from https://github.com/jcwillox/hass-template-climate/tree/main.

# Home Assistant
The device that is created will have the following entities:
- Select Fan Mode - these are standard Home Assistant fan modes, compatible with a climate device
- Select HVAC Mode - these are standard HVAC modes, compatible with a climate device
- Input Number Set temperature - this is the current target temperature and input for target temperature

- Sensor Inlet Air Temperature - T1, air measured coming into the air handler. If you have "follow me" turned on this sensor will eventually get stale.
- Sensor Coil A Temp - Refrigerant temperature, this value will get hot/cold when actively heating or cooling
- Sensor Coil B Temp - Refrigerant temperature (this value is stuck for me, not useful)
- Sensor Outdoor Temp
- Sensor Error Codes - 2 bytes of error codes. I haven't had an error code to test these but they will be the integer form of the 2 hex codes. The reverese engineered XYE notes aren't very clear on these.
- Sensor Full data string - Integer list of all the data received most recently from the Midea unit. Can match these up to the XYE reverse engineered values to check what data is coming in. Sorry it's not in hex, my C++ isn't strong enough haha.

# Background
I have a Carrier heatpump (38MARB) and ducted air handler (40MBAB), and the heat pump is a rebranded Midea unit.  The included controls are fine, but I wanted to add wireless control and integrate with Home Assistant without using a 24V thermostat.  I can't find anything definitive but I suspect using a 24V thermostat would prevent any benefits from the variable frequency drive of the heat pump (the unit won't know the setpoint so it won't know how to modulate).  

There are already ESPHome components for communication with Midea heat pumps via H+/- terminals and IR, but my controller is HA/HB which is not compatible with the hardwired code for H+/- and I wanted 2-way communication of data which isn't possible with the IR component.  I found bunicutz's code for XYE terminals and discovered my air handler had these terminals, so I tried it out and it worked! Unfortunately, it relies on custom components so I had to rewrite it without them using lambda functions. 

# Things I've discovered
- Huge resource has been these training videos: https://siglernorcal.com/video-category/
- These heat pumps and air handlers have a built-in offset of anywhere between 2-4 degrees Celsius to try to account for stratification of the air.  This MAY be configurable on SOME units (I think wall units) but I couldn't change it with my air handler.  Reference p.34: https://cematraining.com/wp-content/uploads/2021/06/CEs-Carrier-Bryant-DLS-Service-Manual.pdf
- That same reference shows the "operating band" and it's pretty big, several degrees Celsius.  So if you feel like you're getting big swings in temperature, that's why. Possibly configurable with a remote, I can't tell if I was able to change anything (no feedback mechanism).
- If you look a few pages further in that reference you'll see that with the remotes you can change some of these factory settings. This is also discussed in the wireless remote manual (RG10 for me) but uses a different set of codes.  Neither method worked for me... but in this video he uses it to change settings on a wall unit: https://siglernorcal.com/aiovg_videos/service-remote-7-of-16/
- If you have the same air handler as me (40MBAB) and it is installed on it's RIGHT side (right horizontal in the installation manual) then the T1 sensor has to be pulled out and moved to the other side of the coils.  Your installer should know what that means, but it's not in the manual.  If you don't, then your unit will sense T1 AFTER the coils so when it turns on it will immediately think the room got hot/cold and turn right back off. If it's acting crazy, that's probably why!  This video explains it: https://siglernorcal.com/aiovg_videos/understanding-the-t1-sensor/
- Even in "follow me" mode the unit will include the T1 sensor in the calculation with a 30% weight. Found that in a random video, and can confirm based on behavior of my unit.  When the T1 was in the wrong spot and reading 90F when heating it skewed my follow-me temperature way up even though the wall unit was seeing 67F.  Once my installer moved the T1 sensor it's much happier.
- I've probably got other resources where I've found some nugget of information... just not thinking of everything right now. Feel free to ask!
