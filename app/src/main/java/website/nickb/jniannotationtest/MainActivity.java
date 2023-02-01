package website.nickb.jniannotationtest;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;

import dalvik.annotation.optimization.CriticalNative;
import dalvik.annotation.optimization.FastNative;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = MainActivity.class.getSimpleName();

    private static final int TEN_MILLION = 10000000;

    static {
        System.loadLibrary("jniannotationtest");
    }

    private static void benchmark() {
        long timeStart = System.currentTimeMillis();
        for (int i = 0; i < TEN_MILLION; i++)
            func_normal();
        long timeEnd = System.currentTimeMillis();

        Log.wtf(TAG, "Normal:         " + (timeEnd - timeStart) + " ms");

        timeStart = System.currentTimeMillis();
        for (int i = 0; i < TEN_MILLION; i++)
            func_bangJni();
        timeEnd = System.currentTimeMillis();

        Log.wtf(TAG, "!bang JNI:      " + (timeEnd - timeStart) + " ms");

        timeStart = System.currentTimeMillis();
        for (int i = 0; i < TEN_MILLION; i++)
            func_fastnative();
        timeEnd = System.currentTimeMillis();

        Log.wtf(TAG, "FastNative:     " + (timeEnd - timeStart) + " ms");

        timeStart = System.currentTimeMillis();
        for (int i = 0; i < TEN_MILLION; i++)
            func_criticalnative();
        timeEnd = System.currentTimeMillis();

        Log.wtf(TAG, "CriticalNative: " + (timeEnd - timeStart) + " ms");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Warmup
        Log.wtf(TAG, "First run (warmup):");
        benchmark();

        Log.wtf(TAG, "");

        // Real benchmark
        Log.wtf(TAG, "Second run:");
        benchmark();
    }

    public static native void func_normal();

    public static native void func_bangJni();

    @FastNative
    public static native void func_fastnative();

    @CriticalNative
    public static native void func_criticalnative();
}
