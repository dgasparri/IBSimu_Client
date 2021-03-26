#include "simulation.hpp"

int main(int argc, char *argv[]) 
{

    bpo::options_description cmdlopt_descriptions_o = 
        ic_config::commandline_options_m();

    std::optional<bpo::variables_map> cmdl_vm_o =
        ic_bpo::cmdl_variable_map_factory_m(
            cmdlopt_descriptions_o,
            argc, 
            argv);


    ic::commandline_options_t cmdlp_o = 
            ic_config::commandline_params_m(cmdl_vm_o);
    
    const int buffer_len = 2500;
    char current_directory[buffer_len];
    getcwd(current_directory, buffer_len);


    if (!ic_config::clean_runpath_m(current_directory, cmdlp_o)) 
    {
        //Show command line help
        std::cout << cmdlopt_descriptions_o << std::endl;
        return 0;
    }


    std::cout<<"Current directory: "<<current_directory<<std::endl;
    std::cout<<"Run Directory: "<<cmdlp_o.run_o<<std::endl;
    std::cout<<"Config filename: "<<cmdlp_o.config_filename_o<<std::endl;

    //return 0;
    bpo::options_description &options_o = ic_bpo::options_description_factory_m();
    ic_config::options_m(options_o);
    ic_output::options_m(options_o);
    ic_pd::options_m(options_o);
    // ... other options files here

    std::optional<bpo::variables_map> params_optional_o = 
        ic_bpo::variable_map_factory_m(
            options_o,
            cmdlp_o.config_filename_o);

    if (!params_optional_o) {
        std::cout << options_o;
        return 1;
    }

    bpo::variables_map &params_o = params_optional_o.value();
 
    try {
    	ibsimu.set_message_threshold( ic_config::message_threshold_m(params_o, MSG_VERBOSE), 1 );
	    ibsimu.set_thread_count( ic_config::num_cores_m(params_o, 2) );

        Geometry* geometry_op = ic_setup::geometry_m(params_o); 

        ic_setup::wall_bound_m(*geometry_op, params_o);
        ic_setup::dxfsolids_m(*geometry_op, params_o, cmdlp_o.run_o);

        MeshVectorField* bfield_op = ic_setup::bfield_m(*geometry_op, params_o, cmdlp_o.run_o);

        geometry_op->build_mesh();

        ic::physics_parameters_t &phy_pars = *ic_setup::physics_parameters_m(params_o);

      
        std::string& lbd_run_o = cmdlp_o.run_o;
        //ibsimu_client::run_output_t lbd_run_output = cmdlp_o.run_output;
        //ibsimu_client::loop_output_t lbd_loop_output = cmdlp_o.loop_output;
        const std::string& prefix_geom_o = (params_o)["ibsimu-file-prefix-geometry"].as<std::string>();
        const std::string& prefix_epot_o = (params_o)["ibsimu-file-prefix-epot"]    .as<std::string>();
        const std::string& prefix_pdb_o  = (params_o)["ibsimu-file-prefix-pdb"]     .as<std::string>();

        /*      

        ic_output::output_m_t output_m = ic_output::output_factory_m(
            lbd_run_o,
            lbd_run_output,
            lbd_loop_output,
            prefix_geom_o,
            prefix_epot_o,
            prefix_pdb_o,
            geometry_op

        );

        std::ofstream diagnostics_file_o;
        //changes diagnostics_file_o;
        ic_pd::diagnostics_stream_open_m(
            params_o, 
            cmdlp_o.run_o,
            diagnostics_file_o);

        if(!diagnostics_file_o.is_open()) {
            std::cout << "Aborting." << std::endl;
            return 1;
        }

        ic_pd::loop_start_m_t diagnostics_loop_start_m = 
            ic_pd::loop_start_factory_m(
                (params_o),
                diagnostics_file_o
            );
        
        double geometry_origo[3] = {
            geometry_op->origo(0),
            geometry_op->origo(1),
            geometry_op->origo(2)
        };
        double geometry_max[3] = {
            geometry_op->max(0),
            geometry_op->max(1),
            geometry_op->max(2)
        };

        ic_pd::loop_end_optional_m_t diagnostics_loop_end_m = 
            ic_pd::particle_diagnostics_factory_m(
                (params_o),
                diagnostics_file_o,
                geometry_origo,
                geometry_max,
                geometry_op->geom_mode()
            );


        const std::optional<bool> display_console = 
                ic_bpo::get<bool>(params_o,"display-console");
                //(params_o)["display-console"].as<bool>();
        if (display_console.value_or(true)) 
            std::cout << "Displaying the console at the end" << std::endl;
        else
            std::cout << "Not displaying the console at the end" << std::endl;            

        */
        ic_output_console::output_console_m_t output_console_m =
            ic_output_console::output_console_factory_m(
                true,
                *geometry_op,
                *bfield_op
            );
        
        std::fstream pdb_f;
        pdb_f.open(lbd_run_o + prefix_pdb_o + ".dat",
                    std::fstream::in | std::fstream::binary);
        //fs.open("/home/ibsimu/lnl-runs/simu.133.1-EC2.2021-03-15.50k-He-newgeom-I25Am2-plasmainit/pdb.dat",
        //        std::fstream::in | std::fstream::binary);
        
        ParticleDataBaseCyl pdb_o(pdb_f, *geometry_op);
        pdb_f.close();
        std::cout << "Loaded " << pdb_o.size() << " particles." <<std::endl;

        // uint32_t part_i = 10;
        
        double time;
        Vec3D loc;
        Vec3D vel;
        std::vector<uint32_t> parts_i = {10, 1000, 10000, 20000, 30000, 40000, 49900 };

        for(uint32_t part_i: parts_i ) {
            size_t traj_size = pdb_o.traj_size(part_i);
            std::string out_file
                = lbd_run_o + "trajectories." + std::to_string(part_i) + ".csv";
            std::cout << "Out file: " << out_file;
            std::fstream f_out;
            f_out.open(out_file,
                std::fstream::out | std::fstream::trunc);
            

            f_out << "t#,time,x0,x1,x2,v1,v2,v3" << std::endl;
            for(size_t n = 0; n < traj_size; n++) {
                pdb_o.trajectory_point(time, loc, vel, part_i, n);
                f_out << n << ",";
                f_out << time << ",";
                f_out << loc[0] << ",";
                f_out << loc[1] << ",";
                f_out << loc[2] << ",";
                f_out << vel[0] << ",";
                f_out << vel[1] << ",";
                f_out << vel[2];
                f_out << std::endl;
            }
            f_out.close();
            std::cout <<"Salvato traj " << part_i << std::endl;

        }

        




        std::ifstream epot_f( lbd_run_o + prefix_epot_o + ".dat" );
        EpotField epot_o(epot_f, *geometry_op);
        epot_f.close();
        EpotEfield efield_o(epot_o);

        MeshScalarField tdens_o(*geometry_op);
        pdb_o.build_trajectory_density_field(tdens_o);

        output_console_m(
            epot_o,
            efield_o,
            tdens_o,
            pdb_o
        );

        /*
        std::string fullpath_timing_filename_o;
        fullpath_timing_filename_o = cmdlp_o.run_o + "simulation-time.txt";

        std::ofstream timing_o(
            fullpath_timing_filename_o, 
            std::ios_base::out | std::ios_base::trunc );


        std::vector<ic_beam::beam_t> beams = ic_setup::beams_m(params_o);
        ic_beam::add_2d_beams_mt add_2b_beam_m = ic_beam::add_2d_beams_helper_m(beams);

        if(!diagnostics_loop_end_m) {
            std::cout << "Error in the config file, aborting."<<std::endl;
            return 0;
        }

        simulation(
            *geometry_op, 
            *bfield_op, 
            phy_pars,
            output_m,
            timing_o,
            add_2b_beam_m,
            output_console_m,
            diagnostics_loop_start_m,
            *diagnostics_loop_end_m,
            (params_o)["number-of-rounds"].as<int>()
            );
    	
        diagnostics_file_o.close();
        */

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    

    //catch( Error e ) {
	//    e.print_error_message( ibsimu.message(0) );
    //}


}
