
1. Install the Pico SDK using the Windows Installer found at https://github.com/raspberrypi/pico-setup-windows
2. Copy the my-projects folder to the SDK root folder
3. Run the **Developer Command Prompt for Pico**

Enter the following commands:

    [SDK folder]> cd my-projects
    [SDK folder]\my-projects> mkdir build
    [SDK folder]\my-projects> cd build
    [SDK folder]\my-projects\build> cmake -G "NMake Makefiles" ..
    [SDK folder]\my-projects\build> nmake

A **fujitsu-ps2.uf2** file will be created in **my-projects\build** that can be copied to the Pico.