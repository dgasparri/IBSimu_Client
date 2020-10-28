#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
#include <optional>
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
#include "bpo_interface.h"


namespace ic = ibsimu_client;
namespace ic_config = ibsimu_client::config;
namespace ic_setup = ibsimu_client::setup;
namespace ic_beam = ibsimu_client::beam;
namespace ic_output = ibsimu_client::output;
namespace ic_output_console = ibsimu_client::output_console;
namespace ic_pd = ibsimu_client::particle_diagnostics;
namespace ic_bpo = ibsimu_client::bpo_interface;








void simulation( 
    Geometry &geometry_o, 
    MeshVectorField &bfield_o, 
    ic::physics_parameters_t &phy_params_o,
    ic_output::output_m_t save_output_m,
    std::ofstream& timelaps_o,
    ic_beam::add_2d_beams_mt add_2b_beam_m,
    ic_output_console::output_console_m_t output_console_m,
    ic_pd::loop_start_m_t diagnostics_loop_start_m,
    ic_pd::loop_end_m_t diagnostics_loop_end_m,
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

        diagnostics_loop_start_m(a);

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

        diagnostics_loop_end_m(a, pdb_o);
/* begin particle_diagnostics

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

end particle_diagnostics

*/



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

