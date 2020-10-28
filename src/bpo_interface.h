#pragma once

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <functional>
#include <optional>
#include <string>

namespace bpo = boost::program_options;


namespace ibsimu_client::bpo_interface {

    typedef std::function<void(bpo::options_description&)> options_injection_m_t;
    
    bpo::options_description& options_description_factory_m();

    std::optional<bpo::variables_map> variable_map_factory_m(
        bpo::options_description& options_description_o,
        std::string config_filename_o);

    std::optional<bpo::variables_map> cmdl_variable_map_factory_m(
        const bpo::options_description &cmdl_options_o,
        int argc, 
        char *argv[]);


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