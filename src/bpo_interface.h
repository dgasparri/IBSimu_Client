#pragma once

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/algorithm/string.hpp>

#include <optional>
#include <string>

namespace bpo = boost::program_options;


namespace ibsimu_client::bpo_interface {
    template <typename T>
    const std::optional<T> get(bpo::variables_map &params_op, std::string key);

}