#include "dither.hpp"

#include <vector>

void DitherProcessor::_bind_methods() {
    gd::ClassDB::bind_method(gd::D_METHOD("process", "source", "target_w", "target_h"),
                             &DitherProcessor::process);
}

gd::Ref<gd::Image> DitherProcessor::process(gd::Ref<gd::Image> source, int target_w, int target_h) {
    gd::Ref<gd::Image> img = source->duplicate();
    img->resize(target_w, target_h, gd::Image::INTERPOLATE_NEAREST);

    int w = img->get_width();
    int h = img->get_height();

    std::vector<float> gray_buffer(w * h);
    std::vector<float> alpha_buffer(w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            gd::Color p = img->get_pixel(x, y);
            gray_buffer[y * w + x] = 0.2126f * p.r + 0.7152f * p.g + 0.0722f * p.b;
            alpha_buffer[y * w + x] = p.a;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            if (alpha_buffer[idx] < 0.5f) {
                continue;
            }

            float gray = gray_buffer[idx];
            float new_gray = gray > 0.5f ? 1.0f : 0.0f;
            float error = gray - new_gray;
            gray_buffer[idx] = new_gray;

            auto add_error = [&](int nx, int ny, float coeff) {
                if (nx < 0 || nx >= w || ny < 0 || ny >= h) {
                    return;
                }
                int nidx = ny * w + nx;
                if (alpha_buffer[nidx] < 0.5f) {
                    return;
                }
                gray_buffer[nidx] = gd::CLAMP(gray_buffer[nidx] + error * coeff, 0.0f, 1.0f);
            };

            add_error(x + 1, y, 7.0f / 16.0f);
            add_error(x - 1, y + 1, 3.0f / 16.0f);
            add_error(x, y + 1, 5.0f / 16.0f);
            add_error(x + 1, y + 1, 1.0f / 16.0f);
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            float g = alpha_buffer[idx] < 0.5f ? 0.0f : gray_buffer[idx];
            float a = alpha_buffer[idx] < 0.5f ? 0.0f : 1.0f;
            img->set_pixel(x, y, gd::Color(g, g, g, a));
        }
    }

    return img;
}
