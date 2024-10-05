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

#include <matplot/axes_objects/isocontour.h>

namespace matplot {
    isocontour::isocontour(axes_type *parent, const vector_2d &X, const vector_2d &Y, const vector_2d &Z, std::string_view line_spec)
        : axes_object(parent), line_spec_(this, line_spec) {
        initialize_faces(X,Y,Z);
        x_data_ = flatten(X);
        y_data_ = flatten(Y);
        z_data_ = flatten(Z);
        initialize_preprocessed_data();
    }
    void isocontour::run_draw_commands() {

        _polygons.clear();
        _polygons_color.clear();

        if (z_nan_) {
            return;
        }

        if (z_const_) {

            matplot::color_array color =  parent_->colormap_interpolation(0, 0, 0);

            for (size_t i = 0; i < faces_.size(); ++i) {
                vector_1d x;
                vector_1d y;
                for (size_t j = 0; j < faces_[i].size(); j++) {
                    if (!isnan(z_data_[faces_[i][j]])) {
                        x.push_back(x_data_[faces_[i][j]]);
                        y.push_back(y_data_[faces_[i][j]]);
                    }                    
                }
                parent_->draw_polygons(x, y, color);
            }

            return;
        }

        int first;
        int last;
        int step;

        for (size_t e = 0; e < faces_.size(); e++) {

            std::map<int, std::vector<point_2d>> levels_map;
            size_t n_facie_nodes = faces_[e].size();

            for (size_t i = 0; i < n_facie_nodes; i++) {

                int node_0 = faces_[e][i];
                int node_1 = faces_[e][(i + 1) % n_facie_nodes];

                point_2d p0{x_data_[node_0], y_data_[node_0]};
                point_2d p1{x_data_[node_1], y_data_[node_1]};

                double z0 = z_data_[node_0];
                double z1 = z_data_[node_1];

                double _zmax;
                double _zmin;

                if (z0 > z1) {
                    first = n_levels_ - 1;
                    last = -1;
                    step = -1;
                    _zmin = z1;
                    _zmax = z0;
                } else {
                    first = 0;
                    last = n_levels_;
                    step = 1;
                    _zmin = z0;
                    _zmax = z1;
                }

                for (int i = first; i != last; i = i + step) {

                    if (levels_[i] < z0 && z0 < levels_[i + 1]) {
                        levels_map[i].push_back(p0);
                    } else if (levels_[i] == z0) {
                        levels_map[i].push_back(p0);
                        levels_map[i - 1].push_back(p0);
                    }

                    if (_zmin < levels_[i] && levels_[i] < _zmax) {

                        double delta_f = (levels_[i] - z0) / (z1 - z0);
                        point_2d pt{p0.x + (p1.x - p0.x) * delta_f,
                                    p0.y + (p1.y - p0.y) * delta_f};
                        levels_map[i].push_back(pt);
                        levels_map[i - 1].push_back(pt);
                    }
                }
            }

            for (auto &[idx, pts] : levels_map) {

                _polygons.push_back(pts);
                _polygons_color.push_back(
                    parent_->colormap_interpolation(idx, 0, n_levels_));
            }
        }

        for (size_t i = 0; i < _polygons.size(); ++i) {
            vector_1d x(_polygons[i].size());
            vector_1d y(_polygons[i].size());
            for (size_t j = 0; j < _polygons[i].size(); j++) {
                x[j] = _polygons[i][j].x;
                y[j] = _polygons[i][j].y;
            }
            parent_->draw_polygons(x, y, _polygons_color[i]);        
        }
    }

    std::string isocontour::set_variables_string() { return std::string(); }
    std::string isocontour::plot_string() { return std::string(); }
    std::string isocontour::legend_string(std::string_view title) {
        return std::string();
    }
    std::string isocontour::data_string() { return std::string(); }

    bool isocontour::requires_colormap() { return true; }

    double isocontour::xmin() { return xmin_; }
    double isocontour::xmax() { return xmax_; }
    double isocontour::ymin() { return ymin_; }
    double isocontour::ymax() { return ymax_; }
    double isocontour::zmin() { return zmin_; }
    double isocontour::zmax() { return zmax_; }

    isocontour &isocontour::line_style(std::string_view str) {
        line_spec_.parse_string(str);
        touch();
        return *this;
    }

    const matplot::line_spec &isocontour::line_spec() const {
        return line_spec_;
    }

    matplot::line_spec &isocontour::line_spec() {
        return line_spec_;
    }

    isocontour &isocontour::line_spec(const class line_spec &line_spec) {
        line_spec_ = line_spec;
        touch();
        return *this;
    }

    const std::vector<double> &isocontour::levels() const {
        return levels_;
    }

    size_t isocontour::n_levels() const {
        return n_levels_;
    }

    isocontour &isocontour::levels(const std::vector<double> &levels) {
        levels_ = levels;
        n_levels_ = levels_.size();
        touch();
        return *this;
    }

    isocontour &isocontour::n_levels(size_t n_levels) {

        levels_.clear();

        if (z_const_) {
            levels_.resize(2);
            levels_.front() = zmin_;
            levels_.back() = zmax_;
            return *this;
        }

        if (z_nan_) {
            levels_.resize(2);
            levels_.front() = 0;
            levels_.back() = 0;
            return *this;
        }    

        vector_1d ticks_values = calcticks(zmin_, zmax_, false, 0.01).ticks;
        n_levels_ = ticks_values.size();
        if (ticks_values.front() != zmin_) {
            levels_.push_back(zmin_ - abs(zmin_) * 0.01);
            n_levels_++;
        }
        levels_.insert(levels_.end(), ticks_values.begin(), ticks_values.end());
        if (ticks_values.back() != zmax_)
        {
            levels_.push_back(zmax_ + abs(zmax_) * 0.01);
            n_levels_++;
        }
            
        touch();
        return *this;
    }

    void isocontour::initialize_preprocessed_data() {

        xmin_ = *std::min_element(x_data_.begin(), x_data_.end());
        xmax_ = *std::max_element(x_data_.begin(), x_data_.end());
        ymin_ = *std::min_element(y_data_.begin(), y_data_.end());
        ymax_ = *std::max_element(y_data_.begin(), y_data_.end());
        zmin_ = *std::min_element(z_data_.begin(),z_data_.end(), NaNComp<double>(false));
        zmax_ = *std::max_element(z_data_.begin(),z_data_.end(), NaNComp<double>(true));

        if (zmin_ == zmax_)
            z_const_ = true;
        if (isnan(zmin_) || isnan(zmax_))
            z_nan_ = true;

        cmap_.resize(z_data_.size());
    }

    void isocontour::make_sure_data_is_preprocessed() {



    }

    void isocontour::clear_preprocessed_data() { cmap_.clear(); }

    void isocontour::initialize_faces(const vector_2d &X, const vector_2d &Y,
                                 const vector_2d &Z) {
    
     int n_cols = X.front().size();
     int n_rows = X.size();
     int n_faces = (n_cols - 1) * (n_rows - 1);
     faces_.resize(n_faces);

     auto idx2d = [n_cols](int i, int j) { return i + j * n_cols; };

     int count = 0;
     for (int j = 0; j < n_rows-1; j++) {
         for (int i = 0; i < n_cols - 1; i++) {
             faces_[count] = {idx2d(i, j), idx2d(i, j + 1), idx2d(i + 1, j + 1), idx2d(i + 1, j)};
             count++;
         }
        }      
    }

    enum axes_object::axes_category isocontour::axes_category() {
        return axes_object::axes_category::two_dimensional;
    }
} // namespace matplot
