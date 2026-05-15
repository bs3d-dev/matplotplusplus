//
// Created by Alan Freitas on 2020-07-07.
//

#include <algorithm>
#include <cmath>
#include <matplot/axes_objects/contours.h>
#include <matplot/axes_objects/histogram.h>
#include <matplot/core/axes_type.h>
#include <matplot/freestanding/plot.h>
#include <matplot/util/common.h>
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>

#include <matplot/axes_objects/polygon.h>

namespace matplot {
    cpolygon::cpolygon(class axes_type *parent, const vector_1d &x,
                     const vector_1d &y,
                     const std::vector<std::vector<int>> &faces,
                     const std::vector<matplot::color_array> &colors,
                     std::string_view line_spec)
        : axes_object(parent), line_spec_(this, line_spec), x_data_(x),
          y_data_(y), faces_(faces),  colors_(colors)
    {
        initialize_preprocessed_data();
    }

    void cpolygon::run_draw_commands() 
    {
        for (size_t f = 0; f < faces_.size(); f++) {
            vector_1d x_poly(faces_[f].size());
            vector_1d y_poly(faces_[f].size());
            for (size_t v = 0; v < faces_.size(); v++) {
                x_poly[v] = x_data_[faces_[f][v]];
                y_poly[v] = y_data_[faces_[f][v]];
            }
             parent_->draw_polygons(x_data_, y_data_, colors_[f]);
        }
    }

    std::string cpolygon::set_variables_string() { return std::string(); }
    std::string cpolygon::plot_string() { return std::string(); }
    std::string cpolygon::legend_string(std::string_view title) {
        return std::string();
    }
    std::string cpolygon::data_string() { return std::string(); }

    bool cpolygon::requires_colormap() { return true; }

    double cpolygon::xmin() { return xmin_; }
    double cpolygon::xmax() { return xmax_; }
    double cpolygon::ymin() { return ymin_; }
    double cpolygon::ymax() { return ymax_; }
    double cpolygon::zmin() { return -10; }
    double cpolygon::zmax() { return 10; }

    cpolygon &cpolygon::line_style(std::string_view str) {
        line_spec_.parse_string(str);
        touch();
        return *this;
    }

    const matplot::line_spec &cpolygon::line_spec() const { return line_spec_; }

    matplot::line_spec &cpolygon::line_spec() { return line_spec_; }

    cpolygon &cpolygon::line_spec(const class line_spec &line_spec) {
        line_spec_ = line_spec;
        touch();
        return *this;
    }

    void cpolygon::initialize_preprocessed_data() {

        xmin_ = x_data_[0];
        xmax_ = x_data_[0];
        ymin_ = y_data_[0];
        ymax_ = y_data_[0];
        for (size_t i = 0; i < x_data_.size(); ++i) {
            xmin_ = *std::min_element(x_data_.begin(), x_data_.end());
            xmax_ = *std::max_element(x_data_.begin(), x_data_.end());
            ymin_ = *std::min_element(y_data_.begin(), y_data_.end());
            ymax_ = *std::max_element(y_data_.begin(), y_data_.end());
        }
    }

    void cpolygon::make_sure_data_is_preprocessed() {}

    void cpolygon::clear_preprocessed_data() { cmap_.clear(); }

    enum axes_object::axes_category cpolygon::axes_category() {
        return axes_object::axes_category::two_dimensional;
    }
} // namespace matplot
