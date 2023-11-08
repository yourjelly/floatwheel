# Floatwheel's Light Control Module (LCM)

Board inside the ADV Floatwheel that controls lighting, power, charing and buzzer. That board is controlled by a tiny microcontroller, the HK32F030MF4P6.

# Requirements

* Floatwheels's LCM
* Programmer
  * J-Link: J-Link OB programmer Software and Documentation pack [https://www.segger.com/downloads/jlink/](https://www.segger.com/downloads/jlink/)
  * ST-LINK: ST-Link downloads [https://www.st.com/en/development-tools/stsw-link007.html](https://www.st.com/en/development-tools/stsw-link007.html)
* PyOCD [https://pyocd.io/docs/installing](https://pyocd.io/docs/installing)
* Compilier
  * GNU Arm Embedded Toolchain
  * Keil μVision

# Building

## Clone This Repository

Clone this repository to local workspace

## A. Install GNU Arm Embedded Toolchain

Download the toolchain yourself from [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) according to your pc architecture, extract the files. Or use the provided link in the commands below

```bash
cd ~/Downloads
curl  https://developer.arm.com/-/media/Files/downloads/gnu/12.2.mpacbti-rel1/binrel/arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi.tar.xz -L -o arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi.tar.xz
tar xvf arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi.tar.xz
sudo mkdir -p /opt/gcc-arm 
cd /opt/gcc-arm/
sudo mv ~/Downloads/arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi/ .
sudo chown -R root:root arm-gnu-toolchain-12.2.mpacbti-rel1-x86_64-arm-none-eabi/
```

### Build
```bash
# clean source code
make clean
# build
make
# or make with verbose output
V=1 make
```

## B. Install Keil μVision (Windows)

Download Keil μVision from here https://www.keil.com/demo/eval/arm.htm

1: Install Keil μVision
2: Ignore the package manager of keil, you dont need aditional packages
3: Ignore the One or more Device Family Pack Devices are not present
4: In project go to Options for target (Alt + F7) -> In the target window, switch the Arm compiler (V6.19 works)
5: Start the compiling by going to project -> build target (F7)



# Flashing

## Option #A: Install SEGGER J-Link

Download and install JLink from [J-Link / J-Trace Downloads](https://www.segger.com/downloads/jlink/).

```bash
# installation command for .deb
sudo dpkg -i JLink_Linux_V770a_x86_64.deb
```

The default installation directory is */opt/SEGGER*

### Flash

```bash
make flash
```

## Option #B: Install pyocd

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

### Flash

Run flash.bat or flash.sh from the repo


# Building With Keil μVision (Windows)



TODO and/or in the works:
Include Float package buzzer support (let the vesc controll the buzzer).
Add a delay for switching the direction of the main lights.
Add a fade / transition to the lights when switching direction. 