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
#include "beam.h"

#include <string>
#include <vector>
#include <unistd.h>
#include <functional>

namespace bpo = boost::program_options;
 








namespace ibsimu_client::setup 
{
    ::bound_type_t bound_type_m(const bound_type_string_t &s);
    ::geom_mode_e geometry_mode_m(std::string gm_string_o);
    ::Geometry* geometry_m(bpo::variables_map &vm_o) ;

    void wall_bound_m(Geometry &geometry_o, bpo::variables_map &vm_o);
    void add_solid_m(Geometry &geometry_o,
                    int progressive, 
                    MyDXFFile *dxffile_op,
                    const std::string &layer_name_o, 
                    dxf_scale_factor_t dxf_scale_factor,
                    bound_type_t bound_type,
                    voltage_t  bound_V  );

    void dxfsolids_m(Geometry &geometry_o, bpo::variables_map &vm_o, std::string rundir);
    ::MeshVectorField* bfield_m(Geometry &geometry_o, bpo::variables_map &vm_o, std::string rundir_o);
    
    ibsimu_client::physics_parameters_t* physics_parameters_m(bpo::variables_map &vm_o);

    std::vector<ibsimu_client::beam::beam_t> beams_m(bpo::variables_map &vm_o); 


}

