#pragma once

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/algorithm/string.hpp>

#include <functional>
#include <optional>
#include <string>

namespace bpo = boost::program_options;


namespace ibsimu_client::bpo_interface {

    typedef std::function<void(bpo::options_description&)> options_injection_m_t;
    
    bpo::options_description& options_description_factory_m()
    {
        bpo::options_description* config_file_options_op =
            new bpo::options_description("Config file options");
        return *config_file_options_op;
    }

    std::optional<bpo::variables_map> variable_map_factory_m(
        bpo::options_description& options_description_o,
        std::string config_filename_o)
    {
        bpo::variables_map vm_o;
        try {
            store(
                parse_config_file(
                    config_filename_o.c_str(), 
                    options_description_o, 
                    true), 
                vm_o);
            bpo::notify(vm_o);
        } catch (boost::exception& e) {
            std::cout<<"Error in config file."<<std::endl;
            std::cout<<diagnostic_information(e)<<std::endl;
            std::cout<<"./simulation --help for help"<<std::endl;
            return std::nullopt;
        }
        return vm_o; 
    }

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