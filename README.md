Overview
I designed an automated, closed-loop PID luminous flux control system that dynamically maintains consistent ambient lighting. In simpler terms: I built a smart lighting system that automatically adjusts its own brightness based on the amount of natural light entering the room.
Rather than a harsh "ON/OFF" switch, the project integrates an LDR and MOSFET to simulate smooth soft-start and fade-out transitions, optimizing overall energy efficiency and hardware lifespan. By integrating an ESP32 and the Blynk IoT platform, the system can be monitored and controlled from anywhere in the world.

Tech Stack & Hardware
Microcontroller: ESP32 
Components: LDR (Light Dependent Resistor), MOSFET

Software / IoT: C++, Blynk IoT Platform

Core Engineering Concepts: PID Control Theory, Pulse Width Modulation (PWM), Deadband Filtering

Key Features
Dynamic PID Dimming: Uses a custom Proportional-Integral-Derivative (PID) algorithm to smoothly transition LED brightness. A programmed "deadband freeze" prevents the lights from flickering when ambient light levels are hovering near the target.
Live IoT Dashboard: Connected to the Blynk cloud, allowing users to remotely set the target Lux (brightness) using a slider UI.
Real-Time Energy Tracking: The system mathematically calculates the cumulative energy saved compared to a standard "always-on" light, streaming the exact savings percentage directly to the cloud dashboard.

System Architecture & Dashboard
https://github.com/user-attachments/assets/af8792e0-c254-4fe8-bddd-3d612b95799a
To replicate or run this code, you will need the Arduino IDE with the ESP32 board package installed.
Open the .ino file in Arduino IDE.
Install the Blynk library via the Library Manager.
Update the WiFi and Blynk credentials at the top of the script
Flash the code to your ESP32!
