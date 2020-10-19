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

    
    typedef std::function<void(
        EpotField&,
        EpotEfield&,
        MeshScalarField&,
        ParticleDataBaseCyl&)> particle_diagnostics_m_t;


    particle_diagnostics_m_t particle_diagnostics_factory_m(
        const bool display_console,
        Geometry& geometry_o,
        MeshVectorField& bfield_o);


}
