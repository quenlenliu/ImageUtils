package com.lewaos.launcher.common;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.widget.ImageView;

/**
 * Created by alex on 16-9-2.
 */
public class GaussianView extends ImageView {

    private Bitmap mBitmap;
    private ImageUtils mUtils;
    public GaussianView(Context context) {
        super(context);
    }

    public GaussianView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public GaussianView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }


    public void setBitmap(Bitmap bitmap) {
        mBitmap = bitmap;
        sRect.set(0, 0, mBitmap.getWidth(), mBitmap.getHeight());
        invalidate();
    }

    private Paint mPaint = new Paint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
    private Rect sRect = new Rect();
    private Rect dRect = new Rect();

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        if (changed) {
            dRect.set(left, top, right, bottom);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (mUtils == null) {
            mUtils = new ImageUtils();
        }
        if (mBitmap != null) {
            synchronized (mBitmap) {
                mUtils.gaussianBlur(mBitmap);
                canvas.drawBitmap(mBitmap, sRect, dRect, mPaint);
            }
        }
    }
}
