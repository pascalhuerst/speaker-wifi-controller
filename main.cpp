#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <unistd.h>

const static int KYGO_KEY_CODE = 363;   // Key code of the KYGO button
const static int KYGO_KEY_INPUT_NR = 1; // What we want is in /dev/input/event1

static int open_input_dev(const int nr)
{
    int  fd;
    char tmp[128];

    snprintf(tmp, sizeof(tmp), "/dev/input/event%d", nr);
    fd = open(tmp, O_RDONLY);

    return fd;
}

static inline int diff_in_ms(struct timeval* start, struct timeval* stop)
{
  struct timeval diff;

  diff.tv_sec  = stop->tv_sec - start->tv_sec;
  diff.tv_usec = stop->tv_usec - start->tv_usec;

  return static_cast<int>((diff.tv_sec * 1000) + (diff.tv_usec / 1000));
}

static int test_key(int fd, int code, int timeout)
{
    struct input_event ev;
    struct pollfd pfd;
    struct timeval start = {0, 0}, stop = {0, 0};
    int ret, wanted_state = 1;

    start = stop;

    for (;;) {
        if (timeout > 0) {
            timeout -= diff_in_ms(&start, &stop);
            if (timeout <= 0)
                return 1;
        }

        pfd.fd     = fd;
        pfd.events = POLLIN;

        gettimeofday(&start, nullptr);
        ret = poll(&pfd, 1, timeout);
        gettimeofday(&stop, nullptr);

        // Data ready to read
        if (ret > 0) {
            ret = static_cast<int>(read(fd, &ev, sizeof(ev)));
            if (ret < 0)
                return ret;

            if (ev.type != EV_KEY || ev.code != code)
                continue;

            if (ev.value == wanted_state) {
                if (wanted_state)
                    wanted_state = 0;
                else
                    break;
            }
        }

        else if (ret == 0) {
            return 1;
        } else {
            return ret;
        }
    }
    return 0;
}

int main()
{
    int key_fd;

    key_fd = open_input_dev(KYGO_KEY_INPUT_NR);
    if (key_fd < 0) {
        perror("Can not open input0\n");
        return key_fd;
    }

    int ret = 0;
    while (1) {
        ret = test_key(key_fd, KYGO_KEY_CODE, -1);
        if (ret < 0)
            break;

        printf("keypress: %i\n", ret);
    }

    return ret;
}
