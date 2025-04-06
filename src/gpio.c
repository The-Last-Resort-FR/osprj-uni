#include <gpio.h>
#include <sensor.h>

extern GlobalData gd;
static int uart_fd = -1;

void* setLedPin(void* args) {
    uint32_t* pin = (uint32_t*)args;
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int check;

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        printf("Failed to open GPIO chip");
        return NULL;
    }

    line = gpiod_chip_get_line(chip, *pin);
    if (!line) {
        printf("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return NULL;
    }

    check = gpiod_line_request_output(line, "SetPin", 0);
    if (check < 0) {
        printf("Failed to request line as output");
        gpiod_chip_close(chip);
        return NULL;
    }

    while(!gd.shouldStop) {
        check = gpiod_line_set_value(line, gd.ledState ? 0 : 1);
        if (check < 0) {
            printf("Failed to set line value");
        }
        sleep(2);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return NULL;
}

int initUART(const char *device, speed_t baudrate) {
    uart_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Unable to open UART");
        return -1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);

    // Set baud rate
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    // Configure: 8N1
    options.c_cflag &= ~PARENB; // No parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits

    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    options.c_oflag &= ~OPOST; // Raw output

    tcsetattr(uart_fd, TCSANOW, &options);

    return 0;
}

void uartSendString(const char *str) {
    if (uart_fd != -1 && str != NULL) {
        write(uart_fd, str, strlen(str));
    }
}

void closeUART() {
    if (uart_fd != -1) {
        close(uart_fd);
        uart_fd = -1;
    }
}