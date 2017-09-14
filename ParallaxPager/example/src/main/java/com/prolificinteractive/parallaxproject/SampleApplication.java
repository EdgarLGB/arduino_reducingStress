package com.prolificinteractive.parallaxproject;

import android.app.Application;
import android.os.StrictMode;

import uk.co.chrisjenx.calligraphy.CalligraphyConfig;

public class SampleApplication extends Application {

  @Override public void onCreate() {
    super.onCreate();
    StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder().detectAll().penaltyLog().penaltyDeath().build());
    StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder().detectAll().penaltyLog().penaltyDeath().build());
    CalligraphyConfig.initDefault(
    new CalligraphyConfig.Builder()
        .setDefaultFontPath("Bitter-Bold.ttf")
        .build()
    );
  }
}
