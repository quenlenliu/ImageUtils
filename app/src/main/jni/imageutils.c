#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <math.h>
#include <assert.h>
#include <malloc.h>

#define LOG_TAG "NEL_ImageUtils"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


typedef struct {
    double_t alpha;
    double_t red;
    double_t green;
    double_t blue;
} argb;

static void gaussianBlur(AndroidBitmapInfo* info, void* pixels, uint8_t radius);

static double_t kernel(double_t distance, double_t sigma);

static void get_weights(uint8_t radius, double_t* weights);

static void set_pixels(AndroidBitmapInfo* info, void * pixels, uint16_t x, uint16_t y, uint32_t pixel);
static uint32_t get_pixels(AndroidBitmapInfo* info, void * pixels, uint16_t x, uint16_t y);

static argb* getColorValue(uint32_t color, argb * value)
{
    value->alpha = color >> 24 & 0xFF;
    value->red = color >> 16 & 0xFF;
    value->green = color >> 8 & 0xFF;
    value->blue = color & 0xFF;
    return value;
}

static uint32_t toColor(argb* color)
{
    return (uint32_t)color->alpha << 24 | (uint32_t )color->red << 16 | (uint32_t )color->green << 8 | (uint32_t )color->blue;
}


static void set_pixels(AndroidBitmapInfo* info, void * pixels, uint16_t x, uint16_t y, uint32_t pixel)
{
    assert(x < info->width && x >= 0);
    assert(y < info->height && y >= 0);
    uint32_t * pixels_p = (uint32_t *) pixels;
    pixels_p[y * info->width + x] = pixel;
}

static uint32_t get_pixels(AndroidBitmapInfo* info, void * pixels, uint16_t x, uint16_t y)
{
    assert(x < info->width && x >= 0);
    assert(y < info->height && y >= 0);
    uint32_t * pixels_p = (uint32_t *) pixels;
    uint32_t p = pixels_p[y * info->width + x];
    return p;
}

static double_t kernel(double_t distance, double_t sigma)
{
    double_t sigma2 = sigma * sigma;
    double_t result = exp(-0.5 * distance * distance / sigma2) / sqrt(2.0 * M_PI * sigma2);
    return result;
}
#define WEIGHT_FACTOR 0.95

static void get_weights(uint8_t radius, double_t * weights)
{
    assert(radius > 0);
    if (radius == 1)
    {
        weights[0] = 1;
        return;
    }
    double_t sigma = radius / 6.0f;
    double_t  sum_weights = 0.0;
    uint8_t i;
    for (i = 0; i < radius; ++i)
    {
        weights[i] = kernel(i, sigma);
        sum_weights += weights[i];
    }
    sum_weights = 2 * sum_weights - weights[0];
    sum_weights = sum_weights / WEIGHT_FACTOR;
    for (i = 0; i < radius; ++i)
    {
        weights[i] /= sum_weights;
    }
}

static argb*argb_multi(argb *value, double_t mul)
{
    value->alpha *= mul;
    value->red *= mul;
    value->green *= mul;
    value->blue *= mul;
    return value;
}

static argb* argb_add(argb* a, argb* b)
{
    a->alpha += b->alpha;
    a->red += b->red;
    a->green += b->green;
    a->blue += b->blue;
    return a;
}

static void gaussianBlur(AndroidBitmapInfo* info, void* pixels, uint8_t radius) {
    double_t weights[radius];
    get_weights(radius, weights);

    size_t size = info->height * info->stride;
    uint16_t width = info->width;
    uint16_t height = info->height;
    uint16_t x, y, i;

    void* temp_pixels = malloc(size);

    argb* result = (argb*) malloc(sizeof(argb));
    argb* temp = (argb*) malloc(sizeof(argb));
    LOGD("Start ----------------------------- ");
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++)
        {
            result = getColorValue(get_pixels(info, pixels, x, y), result);
            result = argb_multi(result, weights[0]);

            for (i = 1; i < radius; i++)
            {
                uint16_t up = y - i > 0 ? y - i : 0;
                temp = getColorValue(get_pixels(info, pixels, x, up), temp);
                temp = argb_multi(temp, weights[i]);
                result = argb_add(result, temp);

                uint16_t bottom = y + i < height ? y + i : height - 1;
                temp = getColorValue(get_pixels(info, pixels, x, bottom), temp);
                temp = argb_multi(temp, weights[i]);
                result = argb_add(result, temp);
            }

            set_pixels(info, temp_pixels, x, y, toColor(result));
        }
    }

    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x)
        {
            result = getColorValue(get_pixels(info, temp_pixels, x, y), result);
            result = argb_multi(result, weights[0]);

            for (i = 1; i < radius; ++i)
            {
                uint16_t left = x - i > 0  ? x - i : 0;
                temp = getColorValue(get_pixels(info, temp_pixels, left, y), temp);
                temp = argb_multi(temp, weights[i]);
                result = argb_add(result, temp);

                uint16_t right = x + i < width ? x + i : width - 1;
                temp = getColorValue(get_pixels(info, temp_pixels, right, y), temp);
                temp = argb_multi(temp, weights[i]);
                result = argb_add(result, temp);
            }

            set_pixels(info, pixels, x, y, toColor(result));
        }
    }
    LOGD("End -----------------------------  ");

    free(temp);
    free(result);
    if (pixels != temp_pixels)
    {
        free(temp_pixels);
    }
    return;
}


JNIEXPORT void JNICALL
Java_com_lewaos_launcher_common_ImageUtils_nGaussianBlur(JNIEnv *env, jobject obj, jobject bitmap, jint radius)
{
    AndroidBitmapInfo info;
    void* pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error = %d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888!");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }
    LOGI("AndroidBitmapInfo: Size:%d x %d, Stride:%d, Format:%d, Flag: %d", info.width, info.height, info.stride, info.format, info.flags);

    //Do real thing
    gaussianBlur(&info, pixels, radius);
    //printPixels(&info, pixels);
    if ((ret = AndroidBitmap_unlockPixels(env, bitmap) < 0)){
        LOGE("AndroidBitmap_unlockPixels() failed ! error=%d", ret);
    }
    LOGD("Precess End ................ ");
    return;
}


