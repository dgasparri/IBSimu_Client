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

void particle_console()
{

    //read gtkparticlediagdialog.cpp GTKParticleDiagDialog::run

    particle_diag_plot_type_e type = PARTICLE_DIAG_PLOT_NONE;
    // PARTICLE_DIAG_PLOT_HISTO2D
    // PARTICLE_DIAG_PLOT_HISTO1D
    // PARTICLE_DIAG_PLOT_SCATTER

    
	coordinate_axis_e axis;
    //AXIS_X;
    //AXIS_R;
    // AXIS_Y;
    //AXIS_Z;
	

	// Read level
	double level;
	//const char *entry_text = gtk_entry_get_text( GTK_ENTRY(entry_level) );
	//level = atof( entry_text );
	

	// Read plot type and set diagnostic accordingly
	trajectory_diagnostic_e diagx = DIAG_NONE, 
            diagy = DIAG_NONE;
    //diagx = DIAG_X;
	//diagy = DIAG_XP;
    //diagx = DIAG_R;
	//diagy = DIAG_RP;
    //diagx = DIAG_Y;
	//diagy = DIAG_YP;
    //diagx = DIAG_R;
	//diagy = DIAG_AP;
    //diagx = DIAG_Z;
	//diagy = DIAG_ZP;
	    //diagx = DIAG_EK;
        //diagx = DIAG_QM;
        //diagx = DIAG_MASS;
	    //diagx = DIAG_CHARGE;

    //axis
    //type
    //level
    //type
    //diagx
    //diagy

    //_plotter->new_particle_plot_window( axis, level, type, diagx, diagy );
}


