#pragma once

#include <ibsimu.hpp>
#include <error.hpp>
#include <geometry.hpp>
#include <epot_field.hpp>
#include <epot_efield.hpp>
#include <meshvectorfield.hpp>
#include <epot_bicgstabsolver.hpp>
#include <trajectorydiagnostics.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/algorithm/string.hpp>

#include <optional>
#include <functional>
//#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>
#include <numeric>
#include <execution>

#include "datatype.h"
#include "config.h"
#include "bpo_interface.h"

#define PD_GRID_N 10 // grid NxM, was 100x100
#define PD_GRID_M 10
#define PD_AXIS_X "AXIS_X"
#define PD_AXIS_Y "AXIX_Y"
#define PD_AXIS_Z "AXIS_Z"
#define PD_AXIS_R "AXIS_R"
#define PD_ALL "ALL"
#define PD_ALPHA "ALPHA"
#define PD_ANGLE "ANGLE"
#define PD_BETA "BETA"
#define PD_EPSILON "EPSILON"
#define PD_GAMMA "GAMMA"
#define PD_RMAJOR "RMAJOR"
#define PD_RMINOR "RMINOR"
#define PD_X_AVERAGE "X_AVERAGE"
#define PD_X_P_AVE "X_P_AVE"
#define PD_I_TOT "I_TOT"
#define PD_N_PARTICLES "N_PARTICLES"


namespace bpo = boost::program_options;

namespace ibsimu_client::particle_diagnostics {

    void options_m(
        bpo::options_description &options_o);
    
    enum pd_extraction_parameters {
        eAlpha,
        eAngle,
        eBeta,
        eEpsilon,
        eGamma,
        eRmajor,
        eRminor,
        eXaverage,
        eXpave,
        eItot,
        eNparticles,
        eUnknown
    };

    pd_extraction_parameters hash_parameters(std::string param);

    std::vector<std::string> diagnostic_parameters_vector_m(std::string input_string);

    void diagnostics_stream_open_m(
        bpo::variables_map &params_o, 
        std::string fullpath,
        std::ofstream &csv_o);

    typedef std::function<void(
        int,
        ParticleDataBase&
        )> loop_end_m_t;

    typedef std::optional<loop_end_m_t> loop_end_optional_m_t;

    loop_end_optional_m_t particle_diagnostics_factory_m(
        bpo::variables_map &params_op,
        std::ofstream &diagnostics_stream_o
        /*
        const bool display_console,
        Geometry& geometry_o,
        MeshVectorField& bfield_o
        */
        );

    typedef std::function<void(int)> loop_start_m_t;

    loop_start_m_t loop_start_factory_m(
        bpo::variables_map &params_op,
        std::ofstream& diagnostics_stream_o
    );

}
