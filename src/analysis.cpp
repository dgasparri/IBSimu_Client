#include <fstream>
#include <iostream>
#include <vector>
#include <functional>

#include <ibsimu.hpp>
#include <error.hpp>
#include <geometry.hpp>
#include <epot_field.hpp>
#include <epot_efield.hpp>
#include <meshvectorfield.hpp>
#include <epot_bicgstabsolver.hpp>
#include <gtkplotter.hpp>
#include <trajectorydiagnostics.hpp>

#include "datatype.h"
#include "config.h"
#include "config-setup.h"
#include "beam.h"



namespace ic = ibsimu_client;
namespace ic_config = ibsimu_client::config;
namespace ic_setup = ibsimu_client::setup;
namespace ic_beam = ibsimu_client::beam;


void analysis(Geometry &geometry_o,EpotField &epot_o,ParticleDataBase &pdb_o, MeshVectorField &bfield_o)
{

    EpotEfield efield_o( epot_o );
    field_extrpl_e efldextrpl[6] = { FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE, 
                    FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE, 
                    FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE };
    efield_o.set_extrapolation( efldextrpl );


	field_extrpl_e bfldextrpl[6] = { FIELD_ZERO, FIELD_ZERO, 
	 				 FIELD_ZERO, FIELD_ZERO, 
	 				 FIELD_ZERO, FIELD_ZERO };
	bfield_o.set_extrapolation( bfldextrpl );

    MeshScalarField tdens_o( geometry_o );
    pdb_o.build_trajectory_density_field( tdens_o );

    int temp_params = 0;
    GTKPlotter plotter( &temp_params, nullptr );
    plotter.set_geometry( &geometry_o );
    plotter.set_epot( &epot_o );
    plotter.set_efield( &efield_o );
    
	plotter.set_bfield( &bfield_o );
    plotter.set_trajdens( &tdens_o );
    plotter.set_particledatabase( &pdb_o );
    plotter.new_geometry_plot_window();
    plotter.run();
}







/*
void simulation( 
    Geometry &geometry_o, 
    MeshVectorField &bfield_o, 
    ic::physics_parameters_t &phy_params_o,
    save_output_prototype_t save_output_m,
    std::ofstream& emittance_csv_stream_o,
    ic_beam::add_2d_beams_mt add_2b_beam_m 
     )
{


    MeshScalarField tdens( geometry_o );
    pdb.build_trajectory_density_field( tdens );
    
    
    delete(initial_plasma_op);

    int temp_a = 1;
    GTKPlotter plotter( &temp_a, nullptr );
    plotter.set_geometry( &geometry_o );
    plotter.set_epot( &epot );
    plotter.set_efield( &efield );
	plotter.set_bfield( &bfield_o );
    plotter.set_trajdens( &tdens );
    plotter.set_particledatabase( &pdb );
    plotter.new_geometry_plot_window();
    plotter.run();


}

*/

int main(int argc, char *argv[]) 
{

    ic::commandline_options_t* cmdlp_op = 
            ic_config::parameters_commandline_m(argc, argv, false);
    
    const int buffer_len = 2500;
    char current_directory[buffer_len];
    getcwd(current_directory, buffer_len);


    if (!ic_config::clean_runpath_m(current_directory, cmdlp_op)) 
    {
        ic_config::show_help(false);
        return 0;
    }


    std::cout<<"Current directory: "<<current_directory<<std::endl;
    std::cout<<"Run Directory: "<<cmdlp_op->run_o<<std::endl;
    std::cout<<"Config filename: "<<cmdlp_op->config_filename_o<<std::endl;

    if (cmdlp_op->epot_filename_o.empty() || cmdlp_op->pdb_filename_o.empty()) 
    {
        std::cout<<"Error: missing epot and/or pdb file"<<std::endl;
        std::cout<<"Usage:"<<std::endl;
        std::cout<<" ./analysis --run simu-XX --epot-file epot.dat --pdb-file pdb.dat"<<std::endl;
        ic_config::show_help(false);
        return 0;
    }

   
    std::string fullpath_epot_filename_o;
    if(cmdlp_op->epot_filename_o[0]=='/')
        fullpath_epot_filename_o = cmdlp_op->epot_filename_o;
    else
        fullpath_epot_filename_o = cmdlp_op->run_o + cmdlp_op->epot_filename_o;

    std::string fullpath_pdb_filename_o;
    if(cmdlp_op->pdb_filename_o[0]=='/')
        fullpath_pdb_filename_o = cmdlp_op->pdb_filename_o;
    else
        fullpath_pdb_filename_o = cmdlp_op->run_o + cmdlp_op->pdb_filename_o;

    std::cout<<"Epot filename: "<<fullpath_epot_filename_o<<std::endl;
    std::cout<<"Pdb filename: "<<fullpath_pdb_filename_o<<std::endl;
    



    bpo::variables_map* params_op;
    try {
        params_op = ic_config::parameters_configfile_m(cmdlp_op->config_filename_o);
    } catch (boost::exception& e) {
        std::cout<<"Error in config file"<<std::endl;
        std::cout<<diagnostic_information(e)<<std::endl;
        std::cout<<"./simulation --help for help"<<std::endl;
        return 1;
    }
    

    try {
    	ibsimu.set_message_threshold( ic_config::message_threshold_m(*params_op, MSG_VERBOSE), 1 );
	    ibsimu.set_thread_count( ic_config::num_cores_m(*params_op, 2) );

        Geometry* geometry_op = ic_setup::geometry_m(*params_op); 

        ic_setup::wall_bound_m(*geometry_op, *params_op);
        ic_setup::dxfsolids_m(*geometry_op, *params_op, cmdlp_op->run_o);

        MeshVectorField* bfield_op = ic_setup::bfield_m(*geometry_op, *params_op, cmdlp_op->run_o);

        geometry_op->build_mesh();

        //Analysis - specific

        //bfield_op->save("bfield.prova.txt");
        std::ofstream fo("bfield-debug.txt");
        bfield_op->debug_print(fo);
        fo.close();


        std::ifstream is_epot(fullpath_epot_filename_o);
        if( !is_epot.good() )
            throw( Error( ERROR_LOCATION, (std::string)"couldn\'t open file \'" + fullpath_epot_filename_o + "\'" ) );
        
        EpotField epot_o( is_epot, *geometry_op );
        is_epot.close();


        std::ifstream is_pdb(fullpath_pdb_filename_o);
        if( !is_pdb.good() )
            throw( Error( ERROR_LOCATION, (std::string)"couldn\'t open file \'" + fullpath_pdb_filename_o + "\'" ) );

        ParticleDataBase *pdb_op;
        if(false) { //TODO
            pdb_op = new ParticleDataBase3D( is_pdb, *geometry_op );
        } else if(true) {
            pdb_op = new ParticleDataBaseCyl( is_pdb, *geometry_op );
        }
        
        
        is_pdb.close();

        analysis(
            *geometry_op,
            epot_o,
            *pdb_op, 
            *bfield_op
        );
    	

    } catch( Error e ) {
	    e.print_error_message( ibsimu.message(0) );
    }


}
