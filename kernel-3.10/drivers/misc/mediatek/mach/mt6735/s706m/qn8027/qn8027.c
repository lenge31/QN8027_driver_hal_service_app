#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/switch.h>

#define BUS_NUM    2
#define QN8027_DEV_NAME    "qn8027"

static struct i2c_board_info i2c_qn8027_board_info = { I2C_BOARD_INFO(QN8027_DEV_NAME, 0x2C) };
static struct i2c_device_id i2c_qn8027_id[] = { {QN8027_DEV_NAME, 0}, {}};

#define printkf(format, ...) printk("%s:" format, __FUNCTION__, ##__VA_ARGS__)

static int qn8027_chip_setFreq(struct i2c_client *client, int freq)
{
	s32 ret = 0;
	/*
		76MHZ + 0.05*x = freq/10
		76*10 + 0.5*x = freq
		x = (freq - 760) * 2
	*/
	int x = (freq-760)*2;
	u8 x_1 = x & 0xff;
	u8 x_2 = (x>>8) & 0x03;

	ret = i2c_smbus_write_byte_data(client, 0x01, x_1);
	if(ret < 0) return ret;

	ret = i2c_smbus_write_byte_data(client, 0x00, x_2 | 0x20);
	if(ret < 0) return ret;

	return 0;
}

static s32 qn8027_chip_reset(struct i2c_client *client)
{
	s32 ret;

	printk("qn8027 chip initing.\n");

	// Reset all registers to the default value
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x81);
	if(ret < 0) return ret;

	return 0;
}

static s32 qn8027_chip_init(struct i2c_client *client)
{
	s32 ret;

	printk("qn8027 chip initing.\n");

	// Reset all registers to the default value
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x81);
	if(ret < 0) return ret;

	msleep(20);

	// Inject digital clock from XTAL1
	ret = i2c_smbus_write_byte_data(client, 0x03, 0x10);
	if(ret < 0) return ret;

	// set 12MHz clock
	ret = i2c_smbus_write_byte_data(client, 0x04, 0x32);
	if(ret < 0) return ret;

	//FSM set
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x41);	//FSM calibrate
	if(ret < 0) return ret;
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x01);	//FSM calibrate complete
	if(ret < 0) return ret;

	msleep(20);

	// Improve SNR
	ret = i2c_smbus_write_byte_data(client, 0x18, 0xe4);
	if(ret < 0) return ret;

	// Increase RF power output maximum
	ret = i2c_smbus_write_byte_data(client, 0x1b, 0xf0);
	if(ret < 0) return ret;

	// Disable RF PA off function
	ret = i2c_smbus_write_byte_data(client, 0x02, 0xb9);
	if(ret < 0) return ret;

	/*
	// Set channel frequency
	ret = i2c_smbus_write_byte_data(client, 0x01, 0xe0);
	if(ret < 0) return ret;

	// Enter Transmit mode, frequency is 100 MHZ.
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x21);
	if(ret < 0) return ret;
	*/

	printk("qn8027 chip inited.\n");

	return 0;
}

static s32 qn8027_chip_id(struct i2c_client *client, u16 *chip_id)
{
	u16 id=0;
	s32 ret;

	ret = i2c_smbus_read_byte_data(client, 0x05);
	if(ret < 0) return ret;
	id = (u16)ret & 0xff;

	ret = i2c_smbus_read_byte_data(client, 0x06);
	if(ret < 0) return ret;
	id |= ((u16)ret&0xff) << 8;

	*chip_id = id;
	
	return 0;
}

enum accdet_report_state
{
    NO_DEVICE =0,
    HEADSET_MIC = 1,
    HEADSET_NO_MIC = 2
};

extern int fake_headphone(enum accdet_report_state state);

static int switch_s = 0;
static int freq_s = 1000; // min value: 760 * 0.1MHZ = 76MHZ.

ssize_t switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", switch_s);
}

ssize_t switch_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	s32 ret;
	int value = 0;
	struct i2c_client *client;

	client = (struct i2c_client *)dev_get_drvdata(dev);

	value = simple_strtol(buf, NULL, 10);

	if(value != 0) // on
	{
		ret = qn8027_chip_init(client);
		if(ret < 0)
		{
			printkf("qn8027_chip_init failed!\n");
			return -1;
		}

		client = (struct i2c_client *)dev_get_drvdata(dev);

		ret = qn8027_chip_setFreq(client, freq_s);
		if(ret < 0)
		{
			printkf("qn8027_chip_setFreq failed!\n");
			return -1;
		}

		fake_headphone(HEADSET_NO_MIC);
	}
	else
	{
		ret = qn8027_chip_reset(client);
		if(ret < 0)
		{
			printkf("qn8027_chip_reset failed!\n");
			return -1;
		}

		fake_headphone(NO_DEVICE);
	}

	switch_s = value;

	return count;
}

ssize_t freq_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", freq_s);
}

ssize_t freq_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	s32 ret;
	int value;
	struct i2c_client *client;

	value = simple_strtol(buf, NULL, 10);
	if(freq_s<760 || freq_s>1080)
	{
		printkf("freq(%d) is invalid value, reset to 100MHZ!\n", freq_s);
		value = 1000;
	}

	client = (struct i2c_client *)dev_get_drvdata(dev);

	ret = qn8027_chip_setFreq(client, value);
	if(ret < 0)
	{
		printkf("qn8027_chip_setFreq failed!\n");
		return -1;
	}

	freq_s = value;

	return count;
}

static DEVICE_ATTR(switch, S_IRUGO|S_IWUGO, switch_show, switch_store);
static DEVICE_ATTR(freq, S_IRUGO|S_IWUGO, freq_show, freq_store);

static int i2c_qn8027_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	s32 ret;
	u16 chip_id = 0;

	printkf("enter.\n");

	ret = qn8027_chip_id(client, &chip_id);
	printkf("qn8027 chip id is 0x%x.\n", chip_id);

	dev_set_drvdata(&client->dev, client);

	ret = device_create_file(&client->dev, &dev_attr_switch);
	ret = device_create_file(&client->dev, &dev_attr_freq);

	return 0;
}

static int i2c_qn8027_remove(struct i2c_client *client)
{
	printkf("enter.\n");

	return 0;
}

static int i2c_qn8027_attach_adapter(struct i2c_adapter *client)
{
	printkf("enter.\n");

	return 0;
}

void i2c_qn8027_alert(struct i2c_client *client, unsigned int data)
{
	printkf("enter, data=%d.\n", data);
}

static void i2c_qn8027_shutdown(struct i2c_client *client)
{
	s32 ret;

	printkf("enter.\n");

	// Reset all registers to the default value
	ret = i2c_smbus_write_byte_data(client, 0x00, 0x81);

}

static int i2c_qn8027_suspend(struct i2c_client *client, pm_message_t msg)
{
	s32 ret;

	printkf("enter, pm_message_t.event=%d.\n", msg.event);
	
	ret = qn8027_chip_reset(client);
	if(ret < 0)
	{
		printkf("qn8027_chip_reset failed!\n");
		return -1;
	}

	return 0;
}

static int i2c_qn8027_resume(struct i2c_client *client)
{
	s32 ret;

	printkf("enter.\n");

	ret = qn8027_chip_init(client);
	if(ret < 0)
	{
		printkf("qn8027_chip_init failed!\n");
		return -1;
	}

	ret = qn8027_chip_setFreq(client, freq_s);
	if(ret < 0)
	{
		printkf("qn8027_chip_setFreq failed!\n");
		return -1;
	}

	return 0;
}

static int i2c_qn8027_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	printkf("enter, cmd=%d, *arg=%p.\n", cmd, arg);

	return 0;
}

static int i2c_qn8027_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	printkf("enter, *info=%p.\n", info);

	return 0;
}

static struct i2c_driver i2c_qn8027_driver = 
{
	.driver = 
	{
		.name = QN8027_DEV_NAME,
	},
	.id_table = i2c_qn8027_id,

	.probe = i2c_qn8027_probe,
	.remove = i2c_qn8027_remove,

	.attach_adapter = i2c_qn8027_attach_adapter,

	.alert = i2c_qn8027_alert,

	.shutdown = i2c_qn8027_shutdown,
	.suspend = i2c_qn8027_suspend,
	.resume = i2c_qn8027_resume,

	.command = i2c_qn8027_command,

	.detect = i2c_qn8027_detect
};

static int qn8027_init(void)
{
	printkf("enter.\n");

	i2c_register_board_info(BUS_NUM, &i2c_qn8027_board_info, sizeof(i2c_qn8027_board_info)/sizeof(struct i2c_board_info));

	if (i2c_add_driver(&i2c_qn8027_driver))
	{
		printkf("qn8027 add driver failed!\n");
		return -1;
	}

	return 0;
}

static void qn8027_exit(void)
{
	printkf("enter.\n");
	i2c_del_driver(&i2c_qn8027_driver);
}

module_init(qn8027_init);
module_exit(qn8027_exit);
