#include "bpo_interface.h"


namespace ic_bpo = ibsimu_client::bpo_interface;

bpo::options_description& ic_bpo::options_description_factory_m()
{
    bpo::options_description* config_file_options_op =
        new bpo::options_description("Config file options");
    return *config_file_options_op;
}

std::optional<bpo::variables_map> ic_bpo::variable_map_factory_m(
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
    } catch (boost::bad_any_cast& e) {
        std::cout<<"Error in provided options  (bad_any_cast): " << e.what() << std::endl;
        std::cout<<boost::diagnostic_information(e)<<std::endl;
        std::cout<<"./simulation --help for help"<<std::endl;
        return std::nullopt;
    } catch (std::exception& e) {
        std::cout<<"Error in provided options (general error): " << e.what() << std::endl;
        std::cout<<boost::diagnostic_information(e)<<std::endl;
        std::cout<<"./simulation --help for help"<<std::endl;
        return std::nullopt;
    } 
    
    
    
    return vm_o; 
}


std::optional<bpo::variables_map> ic_bpo::cmdl_variable_map_factory_m(
    const bpo::options_description &cmdl_options_o,
    int argc, 
    char *argv[])
{
    if(argc < 2) {
        return std::nullopt;
    }

    bpo::variables_map vm_o;
    
    try {

        store(
            parse_command_line(argc, argv, cmdl_options_o), 
            vm_o);
        bpo::notify(vm_o);
    } catch (boost::bad_any_cast& e) {
        std::cout<<"Error in provided command line arguments  (bad_any_cast): " << e.what() << std::endl;
        std::cout<<boost::diagnostic_information(e)<<std::endl;
        std::cout<<"./simulation --help for help"<<std::endl;
        return std::nullopt;
    } catch (std::exception& e) {
        std::cout<<"Error in provided command line arguments (general error): " << e.what() << std::endl;
        std::cout<<boost::diagnostic_information(e)<<std::endl;
        std::cout<<"./simulation --help for help"<<std::endl;
        return std::nullopt;
    }
    return vm_o; 
}

