## This repo contains the souce code of a hack tool I developed using spare time. I never use this to harm any personal computer. I just develop this to kill time and improve my programming skill :)
- All processes in this tool use process injection so that no processes would show on Task Manager. And the automatically launch function realies on serveres, so regit would not be altered.
- "assembledll" used as a backdool server, controler has client application connect this. the functions include: use pipe to open Command Prompt, search files, edit files, reboot, shutdown, download files, get processes, kill processes, mail sending.
- "Controller" is the client used for hacker to control victim's computer.
- "keyboard" contains code for keyboard capturing part. 
- "setkeyboard" contains code to start keyboard app. This program would automatically launch "keyboard" with or without hacker actively connect to server.
- "ybbbbyVir3" contains the first injection code and all the code above so the hacking procedure is as simple as double click :D
- "BadUSB" provide an more hidden way to hack. it contains the code use which hacker can utilize BadUSB. BadUSB is a keystroke injection tool disguised as a generic flash drive.
- Again, I mean no harm, just improve my ability. The only victim is my own laptop.









- It's a windows virus or back door specific aim to windows system(mainly use API to contorl the target computer)

the backdoor once installed succefully, will use system service to autostart everytime.

the backdoor have "cmd pipe","keyboard listener","email sending","process injection",etc characters.

only can be used in local network.

can be used with certain usb driver the script is demo.ino
