#pragma once

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/algorithm/string.hpp>

#include <optional>
#include <string>

namespace bpo = boost::program_options;


namespace ibsimu_client::bpo_interface {
    template <typename T>
    std::optional<T> get(bpo::variables_map &params_op, std::string key)
    {
        try {
            const T& value = 
                params_op[key].as<T>();
            return value;
        } 
        catch(const std::exception& e) {
            return std::nullopt;
        }

        return std::nullopt;
    }
}