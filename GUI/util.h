#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include <sstream>
#include <algorithm>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "TargetRepo.h"

namespace {
    auto load_classes(const std::string &path) {
        std::vector<std::string> out;
        std::ifstream fp(path);
        while (fp) {
            std::string cls;
            std::getline(fp, cls);
            if (!cls.empty()) {
                out.push_back(cls);
            }
        }
        fp.close();
        return out;
    }

    cv::Scalar color_map(int64_t n) {
        auto bit_get = [](int64_t x, int64_t i) { return x & (1 << i); };

        int64_t r = 0, g = 0, b = 0;
        int64_t i = n;
        for (int64_t j = 7; j >= 0; --j) {
            r |= bit_get(i, 0) << j;
            g |= bit_get(i, 1) << j;
            b |= bit_get(i, 2) << j;
            i >>= 3;
        }
        return cv::Scalar(b, g, r);
    }

    void draw_text(cv::Mat &img, const std::string &str,
                   const cv::Scalar &color, cv::Point pos, bool reverse = false) {
        auto t_size = cv::getTextSize(str, cv::FONT_HERSHEY_PLAIN, 1, 1, nullptr);
        cv::Point bottom_left, upper_right;
        if (reverse) {
            upper_right = pos;
            bottom_left = cv::Point(upper_right.x - t_size.width, upper_right.y + t_size.height);
        } else {
            bottom_left = pos;
            upper_right = cv::Point(bottom_left.x + t_size.width, bottom_left.y - t_size.height);
        }

        cv::rectangle(img, bottom_left, upper_right, color, -1);
        cv::putText(img, str, bottom_left, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255) - color);
    }

    cv::Rect2f unnormalize_rect(cv::Rect2f rect, float w, float h) {
        rect.x *= w;
        rect.y *= h;
        rect.width *= w;
        rect.height *= h;
        return rect;
    }

    void draw_bbox(cv::Mat &img, cv::Rect2f bbox,
                   const std::string &label = "", const cv::Scalar &color = {0, 0, 0}) {
        bbox = unnormalize_rect(bbox, img.cols, img.rows);
        cv::rectangle(img, bbox, color);
        if (!label.empty()) {
            draw_text(img, label, color, bbox.tl());
        }
    }

    void draw_trajectories(cv::Mat &img, const std::map<int, cv::Rect2f> &traj,
                           const cv::Scalar &color = {0, 0, 0}) {
        if (traj.size() < 2) return;

        auto cur = traj.begin()->second;
        auto pt1 = cur.br();
        pt1.x -= cur.width / 2;
        pt1.x *= img.cols;
        pt1.y *= img.rows;

        for (auto it = ++traj.begin(); it != traj.end(); ++it) {
            cur = it->second;
            auto pt2 = cur.br();
            pt2.x -= cur.width / 2;
            pt2.x *= img.cols;
            pt2.y *= img.rows;
            cv::line(img, pt1, pt2, color);
            pt1 = pt2;
        }
    }

    wxImage cvMat2wxImage(cv::Mat mat) {
        return wxImage(mat.cols, mat.rows, mat.data, true);
    }
}
#endif //UTIL_H
