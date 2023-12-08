# Floatwheel's Light Control Module (LCM)

Board inside the ADV Floatwheel that controls lighting, power, charing and buzzer. That board is controlled by a tiny microcontroller, the HK32F030MF4P6.

# Requirements

* Floatwheels's LCM
* Programmer
  * ST-LINK: ST-Link downloads [https://www.st.com/en/development-tools/stsw-link007.html](https://www.st.com/en/development-tools/stsw-link007.html)
* PyOCD [https://pyocd.io/docs/installing](https://pyocd.io/docs/installing)
* Compilier
  * Keil μVision

# Building

## Clone This Repository

Clone this repository to local workspace

## Install Keil μVision (Windows)

Download Keil μVision from here https://www.keil.com/demo/eval/arm.htm

1: Install Keil μVision
2: Ignore the package manager of keil, you dont need aditional packages
3: Ignore the One or more Device Family Pack Devices are not present
4: In project go to Options for target (Alt + F7) -> In the target window, switch the Arm compiler (V6.19 works)
5: Start the compiling by going to project -> build target (F7)

# Flashing

## Install pyocd

```bash
pip install pyocd
```

This will install PyOCD into:

```txt
/home/[user]/.local/bin/pyocd
/home/[user]/.local/bin/pyocd-gdbserver
/home/[user]/.local/lib/python3.10/site-packages/pyocd-0.34.2.dist-info/*
/home/[user]/.local/lib/python3.10/site-packages/pyocd/*
```

.profile will take care of the PATH, run `source ~/.profile` to make pyocd command available

## Flash

Run flash.bat (windows) or flash.sh (linux/mac) from the repo


TODO and/or in the works:
Include Float package buzzer support (let the vesc controll the buzzer).
Add a delay for switching the direction of the main lights.
Add a fade / transition to the lights when switching direction. 