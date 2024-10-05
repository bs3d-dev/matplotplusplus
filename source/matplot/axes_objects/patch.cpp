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

#include <matplot/axes_objects/patch.h>

namespace matplot {
    patch::patch(axes_type *parent, const vector_2d &X, const vector_2d &Y,
                 const vector_2d &Z, std::string_view line_spec)
        : axes_object(parent), line_spec_(this, line_spec) {
        initialize_faces(X,Y,Z);
        x_data_ = flatten(X);
        y_data_ = flatten(Y);
        z_data_ = flatten(Z);
        initialize_preprocessed_data();
    }
    patch::patch(axes_type *parent, const vector_1d &x, const vector_1d &y,
                 const vector_1d &z, const std::vector<std::vector<int>> &faces,
                 std::string_view line_spec)
        : axes_object(parent), line_spec_(this, line_spec), x_data_(x),
          y_data_(y), z_data_(z), faces_(faces) {
        initialize_preprocessed_data();
    }
    void patch::run_draw_commands() {

        for (size_t i = 0; i < z_data_.size(); ++i)
            cmap_[i] =
                parent_->colormap_interpolation(z_data_[i], zmin_, zmax_);

        parent_->draw_polygons(x_data_, y_data_, faces_, cmap_);
    }

    std::string patch::set_variables_string() { return std::string(); }
    std::string patch::plot_string() { return std::string(); }
    std::string patch::legend_string(std::string_view title) {
        return std::string();
    }
    std::string patch::data_string() { return std::string(); }

    bool patch::requires_colormap() { return true; }

    double patch::xmin() { return xmin_; }
    double patch::xmax() { return xmax_; }
    double patch::ymin() { return ymin_; }
    double patch::ymax() { return ymax_; }
    double patch::zmin() { return zmin_; }
    double patch::zmax() { return zmax_; }

    patch &patch::line_style(std::string_view str) {
        line_spec_.parse_string(str);
        touch();
        return *this;
    }

    const matplot::line_spec &patch::line_spec() const {
        return line_spec_;
    }

    matplot::line_spec &patch::line_spec() {
        return line_spec_;
    }

    patch &patch::line_spec(const class line_spec &line_spec) {
        line_spec_ = line_spec;
        touch();
        return *this;
    }

    void patch::initialize_preprocessed_data() {

        xmin_ = x_data_[0];
        xmax_ = x_data_[0];
        ymin_ = y_data_[0];
        ymax_ = y_data_[0];
        zmin_ = z_data_[0];
        zmax_ = z_data_[0];
        for (size_t i = 0; i < z_data_.size(); ++i) {
            xmin_ = *std::min_element(x_data_.begin(), x_data_.end());
            xmax_ = *std::max_element(x_data_.begin(), x_data_.end());
            ymin_ = *std::min_element(y_data_.begin(), y_data_.end());
            ymax_ = *std::max_element(y_data_.begin(), y_data_.end());
            zmin_ = *std::min_element(z_data_.begin(), z_data_.end());
            zmax_ = *std::max_element(z_data_.begin(), z_data_.end());
        }

        cmap_.resize(z_data_.size());
    }

    void patch::make_sure_data_is_preprocessed() {}

    void patch::clear_preprocessed_data() { cmap_.clear(); }

    void patch::initialize_faces(const vector_2d &X, const vector_2d &Y,
                                 const vector_2d &Z) {
    
     int n_cols = X.front().size();
     int n_rows = X.size();
     int n_faces = (n_cols - 1) * (n_rows - 1);
     faces_.resize(n_faces);

     auto idx2d = [n_rows](int i, int j) { return i + j * n_rows; };

     int count = 0;
     for (int j = 0; j < n_cols-1; j++) {
         for (int i = 0; i < n_rows - 1; i++) {
             faces_[count] = {idx2d(i, j), idx2d(i, j + 1), idx2d(i + 1, j + 1), idx2d(i + 1, j)};
             count++;
         }
        }      
    }

    enum axes_object::axes_category patch::axes_category() {
        return axes_object::axes_category::two_dimensional;
    }
} // namespace matplot
