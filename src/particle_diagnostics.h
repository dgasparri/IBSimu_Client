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

#include "datatype.h"
#include "config.h"

namespace bpo = boost::program_options;

namespace ibsimu_client::particle_diagnostics {

    void particle_diagnostics_options_m(
        bpo::options_description &command_line_options_o);
    
    std::ofstream& diagnostics_stream_open_m(bpo::variables_map &params_op, std::string fullpath);

    typedef std::function<void(
        EpotField&,
        EpotEfield&,
        MeshScalarField&,
        ParticleDataBaseCyl&)> particle_diagnostics_m_t;


    particle_diagnostics_m_t particle_diagnostics_factory_m(
        bpo::variables_map &params_op,
        std::ofstream &diagnostics_stream_o,
        Geometry& geometry_o
        /*
        const bool display_console,
        Geometry& geometry_o,
        MeshVectorField& bfield_o
        */
        );


}
