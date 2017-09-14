package com.prolificinteractive.parallaxproject;

import android.content.DialogInterface;
import android.graphics.Typeface;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.NumberPicker;
import android.widget.TextView;
import android.widget.Toast;
import com.prolificinteractive.parallaxpager.ParallaxContainer;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.Executors;

public class ParallaxFragment extends Fragment implements ViewPager.OnPageChangeListener, View.OnTouchListener {

  private ArduinoAction arduinoAction;
  private MediaPlayer player;
  private int whichPage = 0;
  private int delay = 0;

  private static final String SERVER_IP = "192.168.4.1";
  private static final int PORT = 80;
  private static final String START = "start";
  private static final String TURNOFF = "turnoff";
  private static final String TITLE = "Vous voulez choisir:";

  private static final int MODE_TAKING_BREAK = 0;
  private static final int MODE_MEDITATION = 1;
  private static final int MODE_RESPIRATION = 2;

  // control signal for arduino
  private static final int BLUE_LIGHT_DIM = 1;
  private static final int BLUE_LIGHT_BRIGHT = 6;
  private static final int GREEN_LIGHT = 2;
  private static final int PURPLE_LIGHT = 3;
  private static final int FAN = 4;
  private static final int TURN_OFF = 5;

  @Override public View onCreateView(LayoutInflater inflater, ViewGroup container,
      Bundle savedInstanceState) {

/*    //start the socket between arduino
    Executors.newSingleThreadExecutor().submit(new ArduinoThread());*/

    View view = inflater.inflate(R.layout.fragment_parallax, container, false);

    // find the parallax container
    ParallaxContainer parallaxContainer =
        (ParallaxContainer) view.findViewById(R.id.parallax_container);

    // specify whether pager will loop
    parallaxContainer.setLooping(true);

    // wrap the inflater and inflate children with custom attributes
    parallaxContainer.setupChildren(inflater,
        R.layout.parallax_view_1,
        R.layout.parallax_view_2,
        R.layout.parallax_view_3);


    parallaxContainer.findViewById(R.id.button_start1).setOnTouchListener(this);
    parallaxContainer.findViewById(R.id.button_start2).setOnTouchListener(this);
    parallaxContainer.findViewById(R.id.button_start3).setOnTouchListener(this);
    parallaxContainer.findViewById(R.id.button_turnoff).setOnTouchListener(this);


    // optionally set a ViewPager.OnPageChangeListener
    parallaxContainer.setOnPageChangeListener(this);

    // initiate the mediaplayer with the first son
    player = MediaPlayer.create(getContext(), R.raw.meditation);

    // set the font family for the title_text
    TextView textView = (TextView) view.findViewById(R.id.title_text);
    Typeface typeface = Typeface.DEFAULT.createFromAsset(getContext().getAssets(), "fonts/Omnibus.otf");
    textView.setTypeface(typeface);


    return view;
  }

  @Override public void onPageScrolled(int position, float offset, int offsetPixels) {
//    // example of manually setting view visibility
//    if (position == 1 && offset > 0.2) {
//      // just before leaving the screen, Earth will disappear
//      mEarthImageView.setVisibility(View.INVISIBLE);
//    } else if (position == 0 || position == 1) {
//      mEarthImageView.setVisibility(View.VISIBLE);
//    } else {
//      mEarthImageView.setVisibility(View.GONE);
//    }

  }

  @Override public void onPageSelected(int position) {
    whichPage = position;
    player.release();
    switch (position % 3) {
      case 0:
        //player = MediaPlayer.create(getContext(), R.raw.son1);
        break;
      case 1:
        player = MediaPlayer.create(getContext(), R.raw.meditation);
        break;
      case 2:
        player = MediaPlayer.create(getContext(), R.raw.cardiaque);
        break;
    }
  }

  @Override public void onPageScrollStateChanged(int state) {

  }

  @Override
  public void onStart() {
    super.onStart();
    //start the socket between arduino
    arduinoAction = new ArduinoAction();
    arduinoAction.execute();
    Log.i("info", "start");
    AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
    builder.setTitle(TITLE);

    // set up the number picker
    final NumberPicker np = new NumberPicker(getContext());
    np.setMinValue(15);
    np.setMaxValue(60);
    builder.setView(np);

    // Set up the buttons
    builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        delay = np.getValue();
      }
    });
    builder.setNegativeButton("Annuler", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {
        delay = 30;
        dialog.cancel();
      }
    });

    builder.show();
  }

  @Override
  public void onStop() {
    //stop all the fan
    arduinoAction.send(TURN_OFF);
    stopMusic();
    Toast.makeText(getContext(), "stop all the things", Toast.LENGTH_SHORT).show();
    arduinoAction.close();
    super.onStop();
  }


  @Override
  public boolean onTouch(View v, MotionEvent event) {
    switch (event.getAction()) {
      case MotionEvent.ACTION_DOWN: {
        switch ((String)v.getTag()) {
          case START:
            //v.setBackgroundResource(R.drawable.bizi2);
            break;
        }
        break;
      }
      case MotionEvent.ACTION_UP: {
        switch ((String)v.getTag()) {
          case START:
            //v.setBackgroundResource(R.drawable.bizi);
            try {
              play();
            } catch (InterruptedException e) {
              e.printStackTrace();
            }
            break;
          case TURNOFF:
            arduinoAction.send(TURN_OFF);
            break;
        }
        break;
      }
    }
    return true;
  }

  private void playMusic() {
    if (player.isPlaying()) {
      player.pause();
      player.seekTo(0);
    } else {
      player.start();
    }
  }

  private void stopMusic() {
    if (player.isPlaying()) {
      player.pause();
      player.seekTo(0);
    }
  }

  private class ArduinoAction extends AsyncTask<Void, Void, Void> {
    private Socket socket;
    private PrintWriter writer;

    @Override
    protected Void doInBackground(Void... voids) {
      try {
        InetAddress address = InetAddress.getByName(SERVER_IP);
        Log.d("debug", "try to connect to " + SERVER_IP);
        socket = new Socket(address, PORT);
        try {
          writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);
          if (socket.isConnected()) {
            Log.d("debug", "connect");
            Toast.makeText(getContext(), "Successfully connect to Arduino", Toast.LENGTH_SHORT).show();
          } else {
            Toast.makeText(getContext(), "Fail to connect to Arduino", Toast.LENGTH_SHORT).show();
            Log.d("debug", "fail to connect");
          }
        } catch (IOException e) {
          e.printStackTrace();
        }
      } catch (UnknownHostException e) {
        e.printStackTrace();
      } catch (IOException e) {
        Log.e("error", e.getMessage());
        e.printStackTrace();
      }
      return null;
    }

    protected void send(int num) {
      writer.println(num);
    }

    protected void close() {
      try {
        socket.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
      writer.close();
    }
  }

  private void play() throws InterruptedException {
    switch (whichPage) {
      case MODE_TAKING_BREAK:
        // start the blue light
        Toast.makeText(getContext(), "start the blue light...", Toast.LENGTH_SHORT).show();
        arduinoAction.send(BLUE_LIGHT_DIM);
        Thread.sleep(delay * 1000);
        arduinoAction.send(BLUE_LIGHT_BRIGHT);
        stopMusic();
        break;

      case MODE_MEDITATION:
        // start the green light
        Toast.makeText(getContext(), "start the green light...", Toast.LENGTH_SHORT).show();
        arduinoAction.send(GREEN_LIGHT);
        // start the music
        Toast.makeText(getContext(), "start the music...", Toast.LENGTH_SHORT).show();
        playMusic();
        // start the fan
        Toast.makeText(getContext(), "start the fan...", Toast.LENGTH_SHORT).show();
        arduinoAction.send(FAN);
        // wait for a couple of secondes
        Thread.sleep(delay * 1000);
        // then turn off all the things
        arduinoAction.send(TURN_OFF);
        stopMusic();
        break;

      case MODE_RESPIRATION:
        // start the purple light
        Toast.makeText(getContext(), "start the purple light...", Toast.LENGTH_SHORT).show();
        arduinoAction.send(PURPLE_LIGHT);
        // start the music
        Toast.makeText(getContext(), "start the music...", Toast.LENGTH_SHORT).show();
        playMusic();
        // wait for a couple of secondes
        Thread.sleep(delay * 1000);
        // then turn off all the things
        arduinoAction.send(TURN_OFF);
        stopMusic();
        break;
    }

  }
}
