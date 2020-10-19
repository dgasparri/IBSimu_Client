#include "particle_diagnostics.h"


namespace ic_particle_diagnostics = ibsimu_client::particle_diagnostics;


ic_particle_diagnostics::particle_diagnostics_m_t ic_output_console::output_console_factory_m(

    const bool display_console,
    Geometry& geometry_o,
    MeshVectorField& bfield_o)
{
    return [
        display_console,
        &geometry_o,
        &bfield_o
        ](
            EpotField& epot_o,
            EpotEfield& efield_o,
            MeshScalarField& tdens_o,
            ParticleDataBaseCyl& pdb_o
        ) {
           /*
            if (!display_console) return;
            int temp_a = 1;
            GTKPlotter plotter( &temp_a, nullptr );
            plotter.set_geometry( &geometry_o );
            plotter.set_epot( &epot_o );
            plotter.set_efield( &efield_o );
            plotter.set_bfield( &bfield_o );
            plotter.set_trajdens( &tdens_o );
            plotter.set_particledatabase( &pdb_o );
            plotter.new_geometry_plot_window();
            plotter.run();
            */
        };
    

}
