#include <hardware/hardware.h>
#include <stdlib.h>
#include <fcntl.h>

#define LOG_TAG "qn8027_hal"
#include <utils/Log.h>

struct qn8027_device_t
{
	struct hw_device_t common;

	int switch_fd;

	int freq_fd; // frequency attribute file descriptor, unit is 0.1MHZ, <76MHZ~108MHZ>.
	int (*set_freq)(struct qn8027_device_t *dev, int freq);
	int (*get_freq)(struct qn8027_device_t *dev, int *freq);
};

static int qn8027_open(struct qn8027_device_t *dev)
{
	int ret;

	ret = write(dev->switch_fd, "1", 1);
	if (ret <= 0)
	{
		ALOGE("write failed!");
		return -1;
	}

	return 0;
}

static int qn8027_close(struct hw_device_t *dev)
{
	int ret;

	ret = write(((struct qn8027_device_t *)dev)->switch_fd, "0", 1);
	if (ret <= 0)
	{
		ALOGE("write failed!");
		return -1;
	}

	return 0;
}

#define VALUE_LEN 10

static int qn8027_set_freq(struct qn8027_device_t *dev, int freq)
{
	int ret;
	char freq_s[VALUE_LEN];

	snprintf(freq_s, sizeof(freq_s), "%d", freq);
	ret = write(dev->freq_fd, freq_s, strlen(freq_s));
	if (ret <= 0)
	{
		ALOGE("write failed!");
		return -1;
	}

	return 0;
}

static int qn8027_get_freq(struct qn8027_device_t *dev, int *freq)
{
	ssize_t ret;
	char freq_s[VALUE_LEN];

	ret = read(dev->freq_fd, freq_s, sizeof(freq_s));
	if (ret <= 0)
	{
		ALOGE("read failed!");
		return -1;
	}

	*freq = strtol(freq_s, NULL, 10);

	return 0;
}

#define switch_name "/sys/devices/bus.2/11009000.I2C2/i2c-2/2-002c/switch"
#define freq_name "/sys/devices/bus.2/11009000.I2C2/i2c-2/2-002c/freq"

static struct qn8027_device_t qn8027_device =
{
	.common =
	{
		.tag = HARDWARE_DEVICE_TAG,
		.version = 0,
		.close = qn8027_close
	},
	.set_freq = qn8027_set_freq,
	.get_freq = qn8027_get_freq
};

static int device_open(const struct hw_module_t* module, const char* id, struct hw_device_t** device)
{
	qn8027_device.common.module = (struct hw_module_t*)module;

	qn8027_device.switch_fd = open(switch_name, O_RDWR);
	if(qn8027_device.switch_fd == -1)
	{
		ALOGE("id=%s, open file(%s) failed, mode=%d!", id, switch_name, O_RDONLY);
		return -1;
	}

	qn8027_device.freq_fd = open(freq_name, O_RDWR);
	if(qn8027_device.freq_fd == -1)
	{
		ALOGE("id=%s, open file(%s) failed!", id, freq_name);
		return -1;
	}

	qn8027_open(&qn8027_device);

	*device = (struct hw_device_t *)&qn8027_device;

	return 0;
}

static hw_module_methods_t module_methods =
{
	.open = device_open
};

#define MODULE_ID    "RfRadio"
#define MODULE_NAME	"RfRadio"
#define MODULE_AUTHOR "wanzhipeng"

struct hw_module_t HAL_MODULE_INFO_SYM =
{
	.tag = HARDWARE_MODULE_TAG,
	.module_api_version = 0x0001,
	.hal_api_version = 0x0,
	.id = MODULE_ID,
	.name = MODULE_NAME,
	.author = MODULE_AUTHOR,
	.methods = &module_methods
};
