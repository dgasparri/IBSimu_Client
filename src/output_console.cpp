#include "output_console.h"


namespace ic_output_console = ibsimu_client::output_console;

/*
void ic_output_console::output_console_options_m(
    bpo::options_description &command_line_options_o)
{
        command_line_options_o.add_options()
            ("display-console", bpo::value<bool>(), "Display console at end, set false for batch simulation")
        ;


}
*/

ic_output_console::output_console_m_t ic_output_console::output_console_factory_m(
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

        };
    

}


/*
void ic_output_console::output_console_m(
    Geometry& geometry_o,
    EpotField& epot_o,
    EpotEfield& efield_o,
    MeshVectorField& bfield_o,
    MeshScalarField& tdens_o,
    ParticleDataBaseCyl& pdb_o) 
{

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
}
*/