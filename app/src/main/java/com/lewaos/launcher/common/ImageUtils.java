package com.lewaos.launcher.common;

import android.graphics.Bitmap;

/**
 * Provider image process method
 */
public class ImageUtils {
    private static final String LIBRARY_NAME = "ImageUtils";

    static {
        System.loadLibrary(LIBRARY_NAME);
    }

    public void gaussianBlur(Bitmap bitmap) {
        nGaussianBlur(bitmap, 32);
    }

    private native void nGaussianBlur(Bitmap bitmap, int radius);
}
