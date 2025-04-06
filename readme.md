# System programming project for university

This is a project requiered from OS programming labs as final exam  
All code is self explaining

## Setup

- Make sure to have all the dependencies installed
Debian :
```bash
sudo apt install libgpiod-dev gpiod gcc make git
```

- Enable UART and I2C
```bash
raspi-config
```

- Ensure user has permission to access /dev/serial0 or /dev/tty0 and /dev/i2c-1 or run as sudo

## Usage

Clone the repo :
```bash
git clone https://github.com/The-Last-Resort-FR/osprj-uni.git
cd osprj-uni
```

To change the default temperature and humidity alert limits as well as the interval at which the sensor gets read change the defines in sensor.h  
Build :
```bash
make
```
Run the app :
```bash
./bin/osprj.run
```

## Documentation

The documentation is available in docs/html/index.html

## Fixes

- It might not close on ctrl+c after the 30 seconds because it waits for another API request, making a request to the API should unstuck it
- It was working in class but I couldn't test the aquisition of the sensor in the final version (don't have this sensor home)