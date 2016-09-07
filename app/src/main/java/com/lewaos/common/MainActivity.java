package com.lewaos.common;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;

import com.lewaos.common.R;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{


    private GaussianView mGaussianView;
    private Button mButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        mGaussianView = (GaussianView) findViewById(R.id.image);
        mButton = (Button)findViewById(R.id.button);
        mButton.setOnClickListener(this);
    }
    Bitmap resultBitmap = null;

    @Override
    public void onClick(View v) {
        if (v == mButton) {
            resultBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.girl_0);
            resultBitmap = Bitmap.createScaledBitmap(resultBitmap, 128, 128 * resultBitmap.getHeight() / resultBitmap.getWidth(), true);

            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.girl_1);
            bitmap = Bitmap.createScaledBitmap(bitmap, 128, 128 * resultBitmap.getHeight() / resultBitmap.getWidth(), true);

            ImageUtils.composeBitmap(resultBitmap, bitmap);

            mGaussianView.setImageBitmap(resultBitmap);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
