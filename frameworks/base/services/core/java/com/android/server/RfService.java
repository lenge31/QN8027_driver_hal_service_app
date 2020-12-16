package com.android.server;

import android.os.IRfService;

public class RfService extends IRfService.Stub {
	private static final String TAG = "RfService";

	public RfService() {
	}

	@Override
	public int open() {
		return openN();
	}

	@Override
	public int close() {
		return closeN();
	}


	@Override
	public int setFreq(int freq) {		// Unit is 0.1MHZ, <76MHZ~108MHZ>
		return setFreqN(freq);
	}

	@Override
	public int getFreq() {
		return getFreqN();
	}

	private native int openN();
	private native int closeN();
	private native int setFreqN(int freq);
	private native int getFreqN();
}
