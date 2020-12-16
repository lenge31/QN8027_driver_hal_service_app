#define LOG_TAG "RfService"
#include "utils/Log.h"

#include <stdio.h>
#include <unistd.h>

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include "hardware/hardware.h"

struct qn8027_device_t
{
	struct hw_device_t common;

	int switch_fd;

	int freq_fd;												// frequency attribute file descriptor, unit is 0.1MHZ, <76MHZ~108MHZ>.
	int (*set_freq)(struct qn8027_device_t *dev, int freq);
	int (*get_freq)(struct qn8027_device_t *dev, int *freq);
};

#define MODULE_ID    "RfRadio"

namespace android
{

struct qn8027_device_t *qn8027_device = NULL;

static jint open(JNIEnv* env, jobject clazz)
{
	const struct hw_module_t *module = NULL;
	int ret;

	ret = hw_get_module(MODULE_ID, &module);
	if(ret < 0 || module == NULL)
	{
		ALOGE("get %s failed!", MODULE_ID);
		return -1;
	}

	ret = module->methods->open(module, MODULE_ID, (struct hw_device_t **)&qn8027_device);
	if(ret < 0)
	{
		ALOGE("open device failed!");
		return -1;
	}

	return 0;
}

static jint close(JNIEnv* env, jobject clazz)
{
	int ret = -1;

	if(!qn8027_device)
	{
		ALOGE("device is not open!");
		return -1;
	}

	ret = qn8027_device->common.close((struct hw_device_t *)qn8027_device);
	if(ret < 0)
	{
		ALOGE("close device failed!!");
		return -1;
	}

	qn8027_device = NULL;

	return 0;
}

static jint set_freq(JNIEnv* env, jobject clazz, jint freq)
{
	int ret = -1;

	if(!qn8027_device)
	{
		ALOGE("device is not open!");
		return -1;
	}

	ret = qn8027_device->set_freq(qn8027_device, freq);
	if(ret < 0)
	{
		ALOGE("set frequency failed!");
		return -1;
	}

	return freq;
}

static jint get_freq(JNIEnv* env, jobject clazz)
{
	int ret = -1;
	int freq;

	if(!qn8027_device)
	{
		ALOGE("device is not open!");
		return -1;
	}

	ret = qn8027_device->get_freq(qn8027_device, &freq);
	if(ret < 0)
	{
		ALOGE("get frequency failed!");
		return -1;
	}

	return freq;
}

static const JNINativeMethod method_table[] =
{
	{"openN", "()I", (void *)open},
	{"closeN", "()I", (void *)close},
	{"setFreqN", "(I)I", (void *)set_freq},
	{"getFreqN", "()I", (void *)get_freq}
};

int register_android_service_RfService(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(env, "com/android/server/RfService", method_table, NELEM(method_table));
}

}
