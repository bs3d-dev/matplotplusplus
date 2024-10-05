//
// Created by Alan Freitas on 2020-07-04.
//

#ifndef MATPLOTPLUSPLUS_PATCH_H
#define MATPLOTPLUSPLUS_PATCH_H

#include <matplot/detail/config.h>

#include <array>
#include <matplot/util/common.h>
#include <matplot/util/concepts.h>
#include <matplot/util/contourc.h>
#include <matplot/util/handle_types.h>

#include <matplot/core/figure_type.h>

#include <matplot/core/axes_object.h>
#include <matplot/core/line_spec.h>

namespace matplot {
    class axes_type;

    class MATPLOT_EXPORTS patch : public axes_object {
      public:
        patch(class axes_type *parent, const vector_2d &X,
                 const vector_2d &Y, const vector_2d &Z,
                 std::string_view line_spec = "");

        patch(class axes_type *parent, const vector_1d &x, const vector_1d &y, const vector_1d &z, const std::vector<std::vector<int>> & faces,
                 std::string_view line_spec = "");

        /// If we receive an axes_handle, we can convert it to a raw
        /// pointer because there is no ownership involved here
        template <class... Args>
        patch(const axes_handle &parent, Args &&...args)
            : patch(parent.get(), std::forward<Args>(args)...) {}

        virtual ~patch() = default;

      public /* mandatory virtual functions */:
        void run_draw_commands() override;
        std::string set_variables_string() override;
        std::string plot_string() override;
        std::string legend_string(std::string_view title) override;
        std::string data_string() override;
        bool requires_colormap() override;
        double xmax() override;
        double xmin() override;
        double ymax() override;
        double ymin() override;
        double zmin() override;
        double zmax() override;
        enum axes_object::axes_category axes_category() override;

      public /* getters and setters */:
        class patch &line_style(std::string_view line_spec);

        const matplot::line_spec &line_spec() const;
        matplot::line_spec &line_spec();
        class patch &line_spec(const class line_spec &line_spec);

      protected:
        void initialize_preprocessed_data();
        void make_sure_data_is_preprocessed();
        void clear_preprocessed_data();

      protected:
        /// Line style
        class line_spec line_spec_;

        /// Data in the xlim
        vector_1d x_data_{};
        vector_1d y_data_{};
        vector_1d z_data_{};
        std::vector<std::vector<int>> faces_{};

        /// Pre-processed data
        std::vector<class line_spec> linestyles_{};
        std::vector<color_array> cmap_ = {};
        double xmin_{NaN};
        double xmax_{NaN};
        double ymin_{NaN};
        double ymax_{NaN};
        double zmin_{NaN};
        double zmax_{NaN};

        /// True if visible
        bool visible_{true};

        private:
        void initialize_faces(const vector_2d &X, const vector_2d &Y,const vector_2d &Z);
    };

} // namespace matplot

#endif // MATPLOTPLUSPLUS_PATCH_H
