#pragma once

#include <boost/program_options/variables_map.hpp>

#include <ibsimu.hpp>
#include <epot_field.hpp>

#include <iostream>



typedef double origin_t; //Start [3]
typedef double mesh_cell_size_h_t; //h
typedef double geometry_value_t; //sizereq[3]
typedef double voltage_t;
typedef double dxf_scale_factor_t;
typedef double bfield_scale_factor_t;
typedef std::string bound_type_string_t;
typedef bound_e bound_type_t;
typedef boost::program_options::variables_map run_parameters_t;


namespace ibsimu_client {

    enum run_output_t {OUT_NORMAL, OUT_EVOLUTION, OUT_BEGIN, OUT_VERBOSE};
    enum loop_output_t {LOOP_END, LOOP_VERBOSE};

    struct parameters_commandline_t {
        std::string run_o;
        std::string config_filename_o;
        run_output_t run_output;
        loop_output_t loop_output;
        std::string epot_filename_o;
        std::string pdb_filename_o;
    };

    struct physics_parameters_t {
        //double electron_charge_density_rhoe;
        double electron_temperature_Te;
        double plasma_potential_Up;
        // double ground_V;
        double plasma_init_x;
        double plasma_init_y;
        double plasma_init_z;
        double space_charge_alpha; //sc_alpha, 0.7, Idk
    };


}