package android.os;

interface IRfService
{
	int open();
	int close();
	int setFreq(int val);
	int getFreq();
}
