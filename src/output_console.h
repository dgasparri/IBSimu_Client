#pragma once

#include <ibsimu.hpp>
#include <error.hpp>
#include <geometry.hpp>
#include <epot_field.hpp>
#include <epot_efield.hpp>
#include <meshvectorfield.hpp>
#include <epot_bicgstabsolver.hpp>
#include <gtkplotter.hpp>
#include <trajectorydiagnostics.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include "datatype.h"
#include "config.h"

namespace bpo = boost::program_options;

namespace ibsimu_client::output_console {

    //void output_console_options_m(bpo::options_description& command_line_options_o);
    
    typedef std::function<void(
        EpotField&,
        EpotEfield&,
        MeshScalarField&,
        ParticleDataBaseCyl&)> output_console_m_t;


    output_console_m_t output_console_factory_m(
        const bool display_console,
        Geometry& geometry_o,
        MeshVectorField& bfield_o);


}
