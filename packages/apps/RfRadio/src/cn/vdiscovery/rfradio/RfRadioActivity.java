package cn.vdiscovery.rfradio;

import android.app.Activity;
import android.os.ServiceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.ImageButton;
import android.os.Bundle;
import android.view.View;
import android.view.MenuInflater;
import android.app.ActionBar;
import android.util.Log;
import android.content.Context;

import android.os.IRfService;

public class RfRadioActivity extends Activity {
	private static final String TAG = "Rf-Radio";

	private MenuItem mMenuItemPower = null;

	private static final String RF = "RF";
    private TextView mTextFm = null;
    private TextView mTextMHz = null;
	private TextView mTextStationValue = null;

	private int mCurrentStation = FmRadioUtils.DEFAULT_STATION;
	private ImageButton mButtonDecrease = null;
    private ImageButton mButtonPrevStation = null;
    private ImageButton mButtonNextStation = null;
    private ImageButton mButtonIncrease = null;

	IRfService rfService = null;

	@Override
    public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.main);     
		
		initUiComponent();

		rfService = IRfService.Stub.asInterface(ServiceManager.getService("RfService"));
		
		try {
			rfService.open();
			mCurrentStation = rfService.getFreq();
		}catch(android.os.RemoteException e) {}

		mTextStationValue.setText(FmRadioUtils.formatStation(mCurrentStation));
	}

	@Override
    public void onResume() {
        super.onResume();
	}

	@Override
    public void onDestroy() {
		try {
			rfService.close();
		}catch(android.os.RemoteException e) {}

		super.onDestroy();
	}

	@Override
    public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.fm_power:
				finish();
				break;
		}

		return super.onOptionsItemSelected(item);
	}

	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.fm_action_bar, menu);
        mMenuItemPower = menu.findItem(R.id.fm_power);
		ActionBar actionBar = getActionBar();
		actionBar.setTitle(R.string.app_name);

        return true;
    }

	private void initUiComponent() {
        mTextFm = (TextView) findViewById(R.id.text_fm);
        mTextFm.setText(RF);
        mTextMHz = (TextView) findViewById(R.id.text_mhz);
        mTextMHz.setText(R.string.fm_unit);
		mTextStationValue = (TextView) findViewById(R.id.station_value);
		mTextStationValue.setText(FmRadioUtils.formatStation(mCurrentStation));

		mButtonDecrease = (ImageButton) findViewById(R.id.button_decrease);
        mButtonIncrease = (ImageButton) findViewById(R.id.button_increase);
        mButtonPrevStation = (ImageButton) findViewById(R.id.button_prevstation);
        mButtonNextStation = (ImageButton) findViewById(R.id.button_nextstation);

		registerButtonClickListener();
		refreshImageButton(true);
    }

	private void refreshStation() {
		try {
			rfService.setFreq(mCurrentStation);
		}catch (android.os.RemoteException e) {}

		mTextStationValue.setText(FmRadioUtils.formatStation(mCurrentStation));
    }

	private void refreshImageButton(boolean enabled) {
        mButtonDecrease.setEnabled(enabled);
        mButtonPrevStation.setEnabled(enabled);
        mButtonNextStation.setEnabled(enabled);
        mButtonIncrease.setEnabled(enabled);
    }

	private void registerButtonClickListener() {
        mButtonDecrease.setOnClickListener(mButtonClickListener);
        mButtonIncrease.setOnClickListener(mButtonClickListener);
        mButtonPrevStation.setOnClickListener(mButtonClickListener);
        mButtonNextStation.setOnClickListener(mButtonClickListener);
    }

private void tuneToStation(final int station) {
        refreshImageButton(false);
		mCurrentStation = station;
		refreshStation();
		refreshImageButton(true);
    }

	private final View.OnClickListener mButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.button_decrease:
                    tuneToStation(FmRadioUtils.computeDecreaseStation(mCurrentStation));
                    break;
                case R.id.button_increase:
                    tuneToStation(FmRadioUtils.computeIncreaseStation(mCurrentStation));
                    break;
                case R.id.button_prevstation:
					tuneToStation(FmRadioUtils.computePreStation(mCurrentStation));
                    break;
                case R.id.button_nextstation:
					tuneToStation(FmRadioUtils.computeNextStation(mCurrentStation));
                    break;
                default:
                    Log.d(TAG, "invalid view id");
                    break;
            }
        }
    };
}
