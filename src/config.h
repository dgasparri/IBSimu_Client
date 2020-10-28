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
#include "particle_diagnostics.h"

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

    
    void options_m(bpo::options_description& options_o);
    
    bpo::options_description commandline_options_m();
    ibsimu_client::commandline_options_t commandline_params_m(
        std::optional<bpo::variables_map>& cmdl_vm_o);


    bool clean_runpath_m(std::string current_directory, ibsimu_client::commandline_options_t& cmdlp_o);

    int num_cores_m(bpo::variables_map &vm_o, int default_v = 2);
    message_type_e message_threshold_m(bpo::variables_map &vm_o, message_type_e default_v);


}




analysis_parameters_t* analysis_parameters_m(int argc, char *argv[]); 
