
<img src="https://user-images.githubusercontent.com/12032605/144592178-241d3673-aecd-458c-95ea-8bf568ecdf4c.jpg" width="48">

https://user-images.githubusercontent.com/12032605/144593895-142e1e6e-6bde-4be6-915e-ca3745a1a5f4.mp4
https://user-images.githubusercontent.com/12032605/144593904-b06fa253-6bf7-4199-a8b3-f0614a0324da.mp4

# README #
This Application was made by Carlos Cano Domingo and Manuel Soler Ortiz as a project for the 2021 European Researchers Night.
The main objective of this project was to show to the youngest assistants the possibilities of Embedded design joint to the Electronics Design. 
The Project center is a 1794 3D Cube built up by the TIC-019 UAL team.
![IMG_20210918_175533](https://user-images.githubusercontent.com/12032605/144584135-14942414-0c22-43df-b1fe-a70222d4b9f4.jpg)
The Cube is formed by a 7 x 7 column Matrix, each of them individually addressable. Each column is constituted by 4 identical led strips, with 9 LEDs in groups of 3. Each group is individually addressable. 
The input connection of each column ios split by 4, one for each side of the column, to show the same color pattern around the column while allowing the device to increase the number of addressable columns (This is so due to the SPI Speed which limits the total number of different addressable LEDs). The output of the column is taken just of one led strips because all strips are in parallel.
![photo_2021-12-03_11-46-01 (2)](https://user-images.githubusercontent.com/12032605/144590407-bee738f2-8405-4f0e-9213-099994b79063.jpg)
![photo_2021-12-03_11-49-56 (2)](https://user-images.githubusercontent.com/12032605/144590394-472c8d21-3701-468c-bb27-16e2737ef07e.jpg)
![IMG_20210920_103139](https://user-images.githubusercontent.com/12032605/144592205-08c170d6-87de-4e97-85c2-835cb3cb43eb.jpg)

![photo_2021-09-11_23-24-47](https://user-images.githubusercontent.com/12032605/144588555-f8debddd-259b-4b3f-8d57-fd4ccd48eb0a.jpg)
![photo_2021-09-11_23-24-47 (2)](https://user-images.githubusercontent.com/12032605/144588561-c551bf21-66bc-44fc-aa62-57cd23e11178.jpg)

The numbero of different addresses is 7 x 7 x 3 = 147.


This repository uses an ESP32 as the central processor for managing all the peripherals, including the CUBE 3D LEDs. 
The Connection between the 3D Cube and the ESP32 is made by an SPI transfer Using the library FASTLED.
![IMG_20210924_140904](https://user-images.githubusercontent.com/12032605/144593162-5a099aeb-b24c-4954-af2d-d59cd6099dee.jpg)

In this example, we use other sensors to change the color pattern of the 3D-Cube. 
1. Joystick to move a small 3D-cube pattern inside the 3D-Cube.
2. Ultrasound detects the 2D position in front of the Cube and lights the zone with the corresponding position in the 3D Cube.
3. Accelerometer to use as a pencil 3D, which fills the corresponding zone in the 3D Cube with color.

Apart from that:
1. We used the library Dabble BLE as a framework to connect the device with an  Android APP. The main use was to detect a color through the phone camera and send it to the Cube LED to fill the whole Cube with the pecked color.
![photo_2021-12-03_12-15-34 (5)](https://user-images.githubusercontent.com/12032605/144593813-93ec4425-6ced-4cd8-a0b4-73a44b5d5c96.jpg)

3. We used MQTT through a WiFi connection and Nodered.js to visualize and manage the primary color used by the sensor application using a web page over the internet.
![photo_2021-09-23_04-42-35](https://user-images.githubusercontent.com/12032605/144593830-bb145617-0121-4407-8eea-46ef9b1f5574.jpg)
![photo_2021-09-23_04-42-08](https://user-images.githubusercontent.com/12032605/144593844-5c2c6483-d326-4232-8bff-df13e9f064fe.jpg)

### Who do I talk to? ###
carcandom@ual.es
![image](https://user-images.githubusercontent.com/12032605/144584634-254a1ca0-02f3-4f18-ba3b-258a033543b6.png)
