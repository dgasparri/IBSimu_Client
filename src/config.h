#pragma once

#include <boost/exception/all.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <ibsimu.hpp>
#include <error.hpp>
#include <geometry.hpp>
#include <dxf_solid.hpp>
#include <mydxffile.hpp>
#include <epot_field.hpp>
#include <epot_efield.hpp>
#include <meshvectorfield.hpp>

#include "datatype.h"
#include "output.h"
#include "output_console.h"

#include <string>
#include <vector>
#include <unistd.h>

namespace bpo = boost::program_options;
 





struct analysis_parameters_t {
    bpo::variables_map *vm_op;
    std::string epot_filename_o;
    std::string pdb_filename_o;
};










namespace ibsimu_client::config {

   
    bpo::variables_map* parameters_configfile_m(std::string config_filename_o); 
    ibsimu_client::parameters_commandline_t* parameters_commandline_m(int argc, char *argv[], bool is_simulation = true);
    ibsimu_client::parameters_commandline_t* clean_runpath_m(std::string current_directory, ibsimu_client::parameters_commandline_t* cmdlp_op);
    void show_help(bool is_simulation = true);

    int num_cores_m(bpo::variables_map &vm_o, int default_v = 2);
    message_type_e message_threshold_m(bpo::variables_map &vm_o, message_type_e default_v);


}




analysis_parameters_t* analysis_parameters_m(int argc, char *argv[]); 
