package cn.vdiscovery.rfradio;

import android.util.Log;
import java.util.Locale;

/**
 * This class provider interface to compute station and frequency, get project
 * string
 */
public class FmRadioUtils {
    private static final String TAG = "FmRadioUtils";

    public static final int DEFAULT_STATION = 1000;

	private static final int STEP = 1;
	private static final int STEP_10 = 10;
	// maximum station frequency
    private static final int HIGHEST_STATION = 1080;
    // minimum station frequency
    private static final int LOWEST_STATION = 875;
	// convert rate
    private static final int CONVERT_RATE = 10;

    public static int computeIncreaseStation(int station) {
        int result = station + STEP;
        if (result > HIGHEST_STATION) {
            result = LOWEST_STATION;
        }
        return result;
    }

	public static int computeNextStation(int station) {
        int result = station + STEP_10;
        if (result > HIGHEST_STATION) {
            result = LOWEST_STATION;
        }
        return result;
    }

    public static int computeDecreaseStation(int station) {
        int result = station - STEP;
        if (result < LOWEST_STATION) {
            result = HIGHEST_STATION;
        }
        return result;
    }

	public static int computePreStation(int station) {
        int result = station - STEP_10;
        if (result < LOWEST_STATION) {
            result = HIGHEST_STATION;
        }
        return result;
    }

    /**
     * According station to get frequency string
     *
     * @param station for 100KZ, range 875-1080
     *
     * @return string like 87.5
     */
    public static String formatStation(int station) {
        float frequency = (float) station / CONVERT_RATE;
        String result = String.format(Locale.ENGLISH, "%.1f",
                Float.valueOf(frequency));
        return result;
    }
}
