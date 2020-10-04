#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
//#include <numeric>

#include <numeric>
#include <chrono>

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
#include "output.h"
#include "output_console.h"


namespace ic = ibsimu_client;
namespace ic_config = ibsimu_client::config;
namespace ic_setup = ibsimu_client::setup;
namespace ic_beam = ibsimu_client::beam;
namespace ic_output = ibsimu_client::output;
namespace ic_output_console = ibsimu_client::output_console;








void simulation( 
    Geometry &geometry_o, 
    MeshVectorField &bfield_o, 
    ic::physics_parameters_t &phy_params_o,
    ic_output::output_m_t save_output_m,
    std::ofstream& emittance_csv_stream_o,
    std::ofstream& timelaps_o,
    ic_beam::add_2d_beams_mt add_2b_beam_m,
    ic_output_console::output_console_m_t output_console_m,
    const int n_rounds 
     )
{


    //exit(0);
    std::cout<<"After mesh"<<std::endl;

    EpotField epot_o( geometry_o );
    MeshScalarField scharge( geometry_o );
    MeshScalarField scharge_ave( geometry_o );

    EpotEfield efield_o( epot_o );
    field_extrpl_e extrapl[6] = { FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE,
				  FIELD_SYMMETRIC_POTENTIAL, FIELD_EXTRAPOLATE,
				  FIELD_EXTRAPOLATE, FIELD_EXTRAPOLATE };
    efield_o.set_extrapolation( extrapl );
  
    EpotBiCGSTABSolver solver( geometry_o );
    
    //http://ibsimu.sourceforge.net/manual_1_0_6/classInitialPlasma.html
    //Initial plasma exists at coordinates less than val in axis direction
    InitialPlasma* initial_plasma_op[3];
    std::function<void(InitialPlasma*&,std::string, coordinate_axis_e, double, double)> plasma_init_helper_m = 
        [&solver](InitialPlasma* &ip_op, std::string axis, coordinate_axis_e axis_e, double init, double potential) {
            if(isnan(init)) {
                ip_op = nullptr;
                return;
            } 
            std::cout<<"Setting initial plasma before axis "<<axis<<std::endl;
            ip_op = new InitialPlasma( axis_e, init );
            solver.set_initial_plasma( 
                potential, 
                ip_op); //TODO: delete InitialPlasma istances at the end of the simulation
        };
    
    plasma_init_helper_m(initial_plasma_op[0], "X", AXIS_X, phy_params_o.plasma_init_x, phy_params_o.plasma_potential_Up);
    plasma_init_helper_m(initial_plasma_op[1], "Y", AXIS_Y, phy_params_o.plasma_init_y, phy_params_o.plasma_potential_Up);
    plasma_init_helper_m(initial_plasma_op[2], "Z", AXIS_Z, phy_params_o.plasma_init_z, phy_params_o.plasma_potential_Up);

        
    

    ParticleDataBaseCyl pdb_o( geometry_o );
    bool pmirror[6] = {false, false,
		       true, false,
		       false, false};
    pdb_o.set_mirror( pmirror );

    
    std::chrono::time_point<std::chrono::system_clock> start, end;

    timelaps_o <<"loop, time[s]"<<std::endl;

    for( int a = 0; a < n_rounds; a++ ) {

        start = std::chrono::system_clock::now();

        emittance_csv_stream_o << a << ",";
        save_output_m(a,"A.init", epot_o, pdb_o);

        ibsimu.message(1) << "Major cycle " << a << "\n";
        ibsimu.message(1) << "-----------------------\n";

        if( a == 1 ) {
            double electron_charge_density_rhoe = pdb_o.get_rhosum();
            solver.set_pexp_plasma( 
                electron_charge_density_rhoe, 
                phy_params_o.electron_temperature_Te, 
                phy_params_o.plasma_potential_Up );
        }


        solver.solve( epot_o, scharge_ave );
        if( solver.get_iter() == 0 ) {
            ibsimu.message(1) << "No iterations, breaking major cycle\n";
            break;
        }

        save_output_m(a,"B.aftersolver", epot_o, pdb_o);

        efield_o.recalculate();

        save_output_m(a,"C.afterefieldrecalculate", epot_o, pdb_o);


        pdb_o.clear();

        save_output_m(a,"D.afterpdbclear", epot_o, pdb_o);

        add_2b_beam_m(pdb_o);

        save_output_m(a,"E.afteraddbeam", epot_o, pdb_o);

        pdb_o.iterate_trajectories( scharge, efield_o, bfield_o );

        save_output_m(a,"F.aftertrajectories", epot_o, pdb_o);
        
        TrajectoryDiagnosticData tdata;
        std::vector<trajectory_diagnostic_e> diag;
        diag.push_back( DIAG_R );
        diag.push_back( DIAG_RP );
        diag.push_back( DIAG_AP );
        diag.push_back( DIAG_CURR );
        pdb_o.trajectories_at_plane( 
                tdata, 
                AXIS_X, 
                geometry_o.max(0), 
                diag );
        
        //Emittance statistics are from original data, not gridded data
        EmittanceConv emit( 
                100, // n Grid array n x m
                100, // m
                tdata(0).data(), //DIAG_R TrajectoryDiagnosticColumn
                tdata(1).data(), //DIAG_RP
                tdata(2).data(), //DIAG_AP
                tdata(3).data()  //DIAG_CURR I
                );

        emittance_csv_stream_o << emit.alpha() << ",";
        emittance_csv_stream_o << emit.beta() << ",";
        emittance_csv_stream_o << emit.epsilon() << ",";

        const int num_traj_end = tdata(3).size();
        const std::vector<double>& IQ_end_o = tdata(3).data();
        double IQ_end = 0.0;
        for(const double IQ: IQ_end_o) {
            IQ_end+= IQ;
        }
        //std::reduce C++17 not yet implemented
        //const double IQ_end = std::reduce(IQ_end_o.cbegin(), IQ_end_o.cend());

        diag.clear();
        tdata.clear();
        diag.push_back( DIAG_CURR );
        pdb_o.trajectories_at_plane( tdata, AXIS_X, 0.001, diag );

        const int num_traj_begin = tdata(0).size();
        const std::vector<double>& IQ_begin_o = tdata(0).data();
        //const double IQ_begin = std::reduce(IQ_begin_o.cbegin(), IQ_begin_o.cend());
        double IQ_begin = 0.0;
        for(const double IQ: IQ_begin_o) {
            IQ_begin+= IQ;
        }


        emittance_csv_stream_o << num_traj_begin << ",";
        emittance_csv_stream_o << IQ_begin << ",";
        emittance_csv_stream_o << num_traj_end << ",";
        emittance_csv_stream_o << IQ_end << ",";

        emittance_csv_stream_o << std::endl;
        emittance_csv_stream_o.flush();

        if( a == 0 ) {
            scharge_ave = scharge;
        } else {
            double sc_beta = 1.0-phy_params_o.space_charge_alpha;
            uint32_t nodecount = scharge.nodecount();
            for( uint32_t b = 0; b < nodecount; b++ ) {
            scharge_ave(b) = phy_params_o.space_charge_alpha*scharge(b) + sc_beta*scharge_ave(b);
            }
        }

        end = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = end-start;
        timelaps_o << a <<","<< diff.count() <<std::endl;

        timelaps_o.flush();
        
    }
    timelaps_o.close();
    save_output_m(-1,"", epot_o, pdb_o);

    MeshScalarField tdens_o( geometry_o );
    pdb_o.build_trajectory_density_field( tdens_o );
    
    
    delete(initial_plasma_op[0]); //Safe to delete nullptr
    delete(initial_plasma_op[1]);
    delete(initial_plasma_op[2]);

    output_console_m(
        epot_o,
        efield_o,
        tdens_o,
        pdb_o);

}


int main(int argc, char *argv[]) 
{

    ic::parameters_commandline_t* cmdlp_op = 
            ic_config::parameters_commandline_m(argc, argv, true);
    
    const int buffer_len = 2500;
    char current_directory[buffer_len];
    getcwd(current_directory, buffer_len);


    if (!ic_config::clean_runpath_m(current_directory, cmdlp_op)) 
    {
        ic_config::show_help(true);
        return 0;
    }


    std::cout<<"Current directory: "<<current_directory<<std::endl;
    std::cout<<"Run Directory: "<<cmdlp_op->run_o<<std::endl;
    std::cout<<"Config filename: "<<cmdlp_op->config_filename_o<<std::endl;

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

        ic::physics_parameters_t &phy_pars = *ic_setup::physics_parameters_m(*params_op);

      
        std::string& lbd_run_o = cmdlp_op->run_o;
        ibsimu_client::run_output_t lbd_run_output = cmdlp_op->run_output;
        ibsimu_client::loop_output_t lbd_loop_output = cmdlp_op->loop_output;
        const std::string& prefix_geom_o = (*params_op)["ibsimu-file-prefix-geometry"].as<std::string>();
        const std::string& prefix_epot_o = (*params_op)["ibsimu-file-prefix-epot"]    .as<std::string>();
        const std::string& prefix_pdb_o  = (*params_op)["ibsimu-file-prefix-pdb"]     .as<std::string>();

       

        ic_output::output_m_t output_m = ic_output::output_factory_m(
            lbd_run_o,
            lbd_run_output,
            lbd_loop_output,
            prefix_geom_o,
            prefix_epot_o,
            prefix_pdb_o,
            geometry_op

       );


        const bool display_console = 
                (*params_op)["display-console"].as<bool>();
        if (display_console) 
            std::cout << "Displaying the console at the end" << std::endl;
        else
            std::cout << "Not displaying the console at the end" << std::endl;            

        
        ic_output_console::output_console_m_t output_console_m =
            ic_output_console::output_console_factory_m(
                display_console,
                *geometry_op,
                *bfield_op
            );
        
        const std::string& stats_filename_o = (*params_op)["ibsimu-file-emittance-statistics"].as<std::string>();
        std::string fullpath_stats_filename_o;
        if(stats_filename_o[0]=='/')
            fullpath_stats_filename_o = stats_filename_o;
        else
            fullpath_stats_filename_o = cmdlp_op->run_o + stats_filename_o;

        std::ofstream emittance_csv(
            fullpath_stats_filename_o, 
            std::ios_base::out | std::ios_base::trunc );

        std::string fullpath_timing_filename_o;
        fullpath_timing_filename_o = cmdlp_op->run_o + "simulation-time.txt";

        std::ofstream timing_o(
            fullpath_timing_filename_o, 
            std::ios_base::out | std::ios_base::trunc );


        std::vector<ic_beam::beam_t> beams = ic_setup::beams_m(*params_op);
        ic_beam::add_2d_beams_mt add_2b_beam_m = ic_beam::add_2d_beams_helper_m(beams);


        simulation(
            *geometry_op, 
            *bfield_op, 
            phy_pars,
            output_m,
            emittance_csv,
            timing_o,
            add_2b_beam_m,
            output_console_m,
            (*params_op)["number-of-rounds"].as<int>()
            );
    	
        emittance_csv.close();

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    

    //catch( Error e ) {
	//    e.print_error_message( ibsimu.message(0) );
    //}


}
