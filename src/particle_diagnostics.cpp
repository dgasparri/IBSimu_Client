#include "particle_diagnostics.h"


namespace ic_pd = ibsimu_client::particle_diagnostics;
namespace ic_bpo = ibsimu_client::bpo_interface;

void ic_pd::options_m(
    bpo::options_description &options_o)
{
        options_o.add_options()
            ("ibsimu-file-beam-diagnostics", bpo::value<std::string>(), "Filename to write emittance statistics to")
            ("diagnostics-axis", bpo::value<std::vector<std::string>>(), "Diagnostics axis [AXIS_X, AXIX_Y, AXIS_Z, AXIS_R]")
            ("diagnostics-at-loop", bpo::value<std::vector<std::string>>(), "Comma separated values of loops at which diagnostics should be recorded, for example: FIRST, 10, 20, 30, 40, LAST [ALL, FIRST, LAST, loop number (1,2,...)]")
            ("diagnostics-plane-coordiante", bpo::value<std::vector<double>>(), "plane coordinate")
            ("diagnostics-extraction-parameters", bpo::value<std::vector<std::string>>(), "Comma separated list of diagnostics values at plane [ALPHA, ANGLE, BETA, EPSILON, GAMMA, RMAJOR, RMINOR, X_AVERAGE, X_P_AVE, I_TOT, N_PARTICLES] ")
            //("emission-x", bpo::value<std::string>()->default_value("OUT_NORMAL"), "emission x output files generated in the run [OUT_NORMAL (default, only final files), OUT_EVOLUTION (every 10 loops and last), OUT_BEGIN (first 3 loops and final), OUT_VERBOSE (first 3, every 10 loops and last)]")
        ;

}


ic_pd::pd_extraction_parameters ic_pd::hash_parameters(std::string param)
{
    if (param == PD_ALPHA) return ic_pd::eAlpha;
    if (param == PD_BETA) return ic_pd::eBeta;
    if (param == PD_EPSILON) return ic_pd::eEpsilon;
    if (param == PD_GAMMA) return ic_pd::eGamma;
    if (param == PD_RMAJOR) return ic_pd::eRmajor;
    if (param == PD_RMINOR) return ic_pd::eRminor;
    if (param == PD_ANGLE) return ic_pd::eAngle;
    if (param == PD_X_AVERAGE) return ic_pd::eXaverage;
    if (param == PD_X_P_AVE) return ic_pd::eXpave;
    if (param == PD_I_TOT) return ic_pd::eItot;
    if (param == PD_N_PARTICLES) return ic_pd::eNparticles;
    return ic_pd::eUnknown;
}

/**
 * 
 * From comma separated list + "ALL" to the full parameter list
 * 
 */ 
std::vector<std::string> ic_pd::diagnostic_parameters_vector_m(std::string input_string)
{
    
    std::vector<std::string> params;
    boost::algorithm::split(params, input_string, boost::is_any_of(","));
    for(std::string &param: params) {
        boost::algorithm::trim(param);
        //std::cout<<"Params: ***"<<param<<"**";
    }
    
    for(std::vector<std::string>::iterator i = params.begin(); i != params.end(); i++) 
    {
        //std::cout<<*i<<std::endl;

        if(*i == PD_ALL) {
            i = params.erase(i);
            i = params.emplace(i, PD_ALPHA);
            i = params.emplace(i+1, PD_BETA);
            i = params.emplace(i+1, PD_GAMMA);
            i = params.emplace(i+1, PD_EPSILON);
            i = params.emplace(i+1, PD_ANGLE);
            i = params.emplace(i+1, PD_RMAJOR);
            i = params.emplace(i+1, PD_RMINOR);
            i = params.emplace(i+1, PD_X_AVERAGE);
            i = params.emplace(i+1, PD_X_P_AVE);
            i = params.emplace(i+1, PD_I_TOT);
            i = params.emplace(i+1, PD_N_PARTICLES);
        }
    }
    
    return params;
}




void ic_pd::diagnostics_stream_open_m(
    bpo::variables_map &params_o, 
    std::string fullpath,
    std::ofstream &csv_o)
{

    //const std::string& stats_filename_o = 
    //    params_o["ibsimu-file-beam-diagnostics"].as<std::string>();
    const std::optional<std::string> stats_filename_o = ic_bpo::get<std::string>(params_o, "ibsimu-file-beam-diagnostics");
    if(!stats_filename_o) {
        std::cout << "Required key ibsimu-beam-diagnostics-file not found in config file." << std::endl;
        return;
    }
    std::string fullpath_stats_filename_o;
    if((*stats_filename_o)[0]=='/')
        fullpath_stats_filename_o = (*stats_filename_o);
    else
        fullpath_stats_filename_o = fullpath + (*stats_filename_o);

    csv_o.open(
        fullpath_stats_filename_o, 
        std::ios_base::out | std::ios_base::trunc );
    
    if (!csv_o.is_open()) {
        std::cout << "Unable to open diagnostics file: " << fullpath_stats_filename_o << std::endl;
        return;
    }

}

ic_pd::loop_start_m_t 
    ic_pd::loop_start_factory_m(
        bpo::variables_map &params_op,
        std::ofstream& diagnostics_stream_o
)
{
    return [
        &diagnostics_stream_o
    ](int loop_n) {
        diagnostics_stream_o << loop_n;
    };
}


std::optional<coordinate_axis_e>
    ic_pd::particle_diagnostics_axis_m(std::string &diag_axis_str)
{
    if(diag_axis_str == PD_AXIS_X_R_RP || diag_axis_str == PD_AXIS_X_R_AP || diag_axis_str == PD_AXIS_X_Z_ZP)
        return AXIS_X;
    if(diag_axis_str == PD_AXIS_Y) 
        return AXIS_Y;
    if(diag_axis_str == PD_AXIS_Z) 
        return AXIS_Z;
    if(diag_axis_str == PD_AXIS_R_X_XP) 
        return AXIS_R;
    return std::nullopt;
}


std::optional<std::vector<trajectory_diagnostic_e>>
    ic_pd::particle_diagnostics_trajectories_m(std::string &diag_axis_str)
{
    std::vector<trajectory_diagnostic_e> diag;
    if(diag_axis_str == PD_AXIS_X_R_RP)  {
        diag.push_back( DIAG_R );
        diag.push_back( DIAG_RP );
        // diag.push_back( DIAG_NONE );
        diag.push_back( DIAG_CURR );
        return diag;
    }
    
    if (diag_axis_str == PD_AXIS_X_R_AP) {
        diag.push_back( DIAG_R );
        diag.push_back( DIAG_AP );
        diag.push_back( DIAG_NONE );
        diag.push_back( DIAG_CURR );
        return diag;
    } 

    if (diag_axis_str == PD_AXIS_X_Z_ZP) {
        //Emittance conv
        diag.push_back( DIAG_R );
        diag.push_back( DIAG_RP );
        diag.push_back( DIAG_AP );
        diag.push_back( DIAG_CURR );
        return diag;
    }
    
    if(diag_axis_str == PD_AXIS_R_X_XP) 
    {
        diag.push_back( DIAG_X );
        diag.push_back( DIAG_XP );
        diag.push_back( DIAG_NONE );
        diag.push_back( DIAG_CURR );
        return diag;
    }
    return std::nullopt;
}



//std::optional<ic_pd::loop_end_m_t> 
ic_pd::loop_end_optional_m_t
    ic_pd::particle_diagnostics_factory_m(
        bpo::variables_map& params_o,
        std::ofstream& diagnostics_stream_o,
        double *geometry_origo,
        double *geometry_max,
        geom_mode_e geometry_mode
   )
{
    
    const std::optional<std::vector<std::string>> diag_at_loop = 
        ic_bpo::get<std::vector<std::string>>(params_o,"diagnostics-at-loop");
    const std::optional<std::vector<std::string>> diag_axis_str = 
        ic_bpo::get<std::vector<std::string>>(params_o,"diagnostics-axis");
    const std::optional<std::vector<double>> diag_plane_coordinate = 
        ic_bpo::get<std::vector<double>>(params_o,"diagnostics-plane-coordiante");
    const std::optional<std::vector<std::string>> diag_extr_params_str = 
        ic_bpo::get<std::vector<std::string>>(params_o,"diagnostics-extraction-parameters");

    if (   !diag_at_loop 
        && !diag_axis_str
        && !diag_plane_coordinate
        && !diag_extr_params_str)
    {
        //No diagnostics required by project, returning dummy function
        std::cout << "No diagnostics required. Will output an empty diagnostics file." << std::endl;
        return [
            &diagnostics_stream_o
        ](
            int loop_n,
            ParticleDataBase& pdb_o
        ) {
            diagnostics_stream_o << std::endl;
            diagnostics_stream_o.flush();
        };
    }

    const std::size_t extractions = (*diag_at_loop).size();
    
    if((*diag_axis_str).size() != extractions
       || (*diag_plane_coordinate).size() != extractions 
       || (*diag_extr_params_str).size() != extractions)
    {
        std::cout << "There must be the same number of diagnostics-at-loop, diagnostics-axis, diagnostics-plane-coordiante and diagnostics-extraction-parameters." << std::endl;
        std::cout << "Particle diagnostics unable to instantiate. Aborting. "  << std::endl;
        return std::nullopt;
    }

    //std::unordered_set<std::string> axes = {PD_AXIS_X, PD_AXIS_Y, PD_AXIS_Z, PD_AXIS_R};
    std::vector<std::vector<trajectory_diagnostic_e>> diag_trajectories;
    std::vector<coordinate_axis_e> diag_axis;
    std::vector<bool> use_emittanceconv;
    for(std::string axis: (*diag_axis_str))
    {
        boost::algorithm::trim(axis);
        std::optional<coordinate_axis_e> axis_opt = 
            particle_diagnostics_axis_m(axis);
        if (!axis_opt) {
            std::cout << "Axis " << axis << " unknown." << std::endl;
            std::cout << "Particle diagnostics unable to instantiate. Aborting. "  << std::endl;
            return std::nullopt;
        }

        std::optional<std::vector<trajectory_diagnostic_e>> trajectories_opt = 
            particle_diagnostics_trajectories_m(axis);
        if(!trajectories_opt) {
            std::cout << "Unknown " << axis << " trajectory for diagnostics." << std::endl;
            std::cout << "Particle diagnostics unable to instantiate. Aborting. "  << std::endl;
            return std::nullopt;
        }

        diag_axis.emplace_back(*axis_opt);
        diag_trajectories.emplace_back(*trajectories_opt);
        use_emittanceconv.emplace_back(axis == PD_AXIS_X_Z_ZP);
    }

    //TODO: Check diag_plane_coordinate against geometry_o.max(0), 

    std::vector<std::vector<std::string>> diag_extr_params;
    for(auto &params_str: *diag_extr_params_str)
    {
        diag_extr_params.push_back(ic_pd::diagnostic_parameters_vector_m(params_str));
    }

    diagnostics_stream_o << "loop#";
    for(size_t i = 0; i < diag_extr_params.size(); i++) {
        std::string t_axis_pos = (*diag_axis_str)[i];
        /*
            if(diag_axis[i] == AXIS_X) t_axis_pos = PD_AXIS_X;
            if(diag_axis[i] == AXIS_Y) t_axis_pos = PD_AXIS_Y;
            if(diag_axis[i] == AXIS_Z) t_axis_pos = PD_AXIS_Z;
            if(diag_axis[i] == AXIS_R) t_axis_pos = PD_AXIS_R;
        */
        t_axis_pos += ":";
        t_axis_pos += to_string((*diag_plane_coordinate)[i]);
        for(const std::string &t_param: diag_extr_params[i]) {
            diagnostics_stream_o << "," 
                                << t_param
                                << "[" << t_axis_pos << "]";
        }
    }
    diagnostics_stream_o << std::endl;
    diagnostics_stream_o.flush();

    return [
        //&params_o,
        &diagnostics_stream_o,
        diag_at_loop,
        diag_axis,
        diag_trajectories,
        use_emittanceconv,
        diag_plane_coordinate,
        diag_extr_params,
        geometry_origo,
        geometry_max,
        geometry_mode
        ](
            int loop_n,
            ParticleDataBase& pdb_o
        ) {

            //No diagnostics at loop zero - risk of segmentation fault
            if(!loop_n)
                return; 
            //No extraction to do, no need to load data
            std::size_t extractions = diag_axis.size();
            if(!extractions)
                return;            

            //Iniziare loop
            for(std::size_t i = 0; i < extractions; i++) {
                std::cout<<"Extracting diagnostic data at "<<(*diag_plane_coordinate)[i]<<std::endl;

                TrajectoryDiagnosticData tdata;
                /* 
                    std::vector<trajectory_diagnostic_e> diag;
                    diag.push_back( DIAG_R );
                    diag.push_back( DIAG_RP );
                    diag.push_back( DIAG_AP );
                    diag.push_back( DIAG_CURR );
                */

                pdb_o.trajectories_at_plane( 
                        tdata, 
                        diag_axis[i], 
                        (*diag_plane_coordinate)[i], 
                        diag_trajectories[i] ); //was diag

                Emittance *emit;                
                if(!use_emittanceconv[i]) {
                    bool mirror[6];
                    pdb_o.get_mirror(mirror);
                    
                    // Mirror in x-direction
                    if( mirror[0] ) {
                        tdata.mirror( AXIS_X, geometry_origo[0] );
                    } else if( mirror[1] ) {
                        tdata.mirror( AXIS_X, geometry_max[0] );
                    } else 
                    // Mirror in y-direction, forced if cylindrical geometry
                    if( geometry_mode == MODE_CYL || mirror[2] ) {
                        tdata.mirror( AXIS_Y, geometry_origo[1] );
                    } else if( mirror[3] ) {
                        tdata.mirror( AXIS_Y, geometry_max[1] );
                    } else
                    // Mirror in z-direction
                    if( mirror[4] ) {
                        tdata.mirror( AXIS_Z, geometry_origo[2] );
                    } else if( mirror[5] ) {
                        tdata.mirror( AXIS_Z, geometry_max[2] );
                    }
                    
                    emit = new Emittance(tdata(0).data(), 
				                         tdata(1).data(), 
				                         tdata(2).data() );


                } else {
                    //Emittance statistics are from original data, not gridded data
                    emit = new EmittanceConv( 
                            PD_GRID_N , // n Grid array n x m
                            PD_GRID_M, // m
                            tdata(0).data(), //DIAG_R TrajectoryDiagnosticColumn
                            tdata(1).data(), //DIAG_RP
                            tdata(2).data(), //DIAG_AP
                            tdata(3).data()  //DIAG_CURR I
                            );

                }
                

                
                

                for(auto param: diag_extr_params[i]) {
                    switch(ic_pd::hash_parameters(param)) {
                        case ic_pd::eAlpha:
                            diagnostics_stream_o << "," << emit->alpha();
                            break;
                        case ic_pd::eBeta:
                            diagnostics_stream_o << "," << emit->beta();
                            break;
                        case ic_pd::eGamma:
                            diagnostics_stream_o << "," << emit->gamma();
                            break;
                        case ic_pd::eEpsilon:
                            diagnostics_stream_o << "," << emit->epsilon();
                            break;
                        case ic_pd::eAngle:
                            diagnostics_stream_o << "," << emit->angle();
                            break;
                        case ic_pd::eRmajor:
                            diagnostics_stream_o << "," << emit->rmajor();
                            break;
                        case ic_pd::eRminor:
                            diagnostics_stream_o << "," << emit->rminor();
                            break;
                        case ic_pd::eXaverage:
                            diagnostics_stream_o << "," << emit->xave();
                            break;
                        case ic_pd::eXpave:
                            diagnostics_stream_o << "," << emit->xpave();
                            break;
                        case ic_pd::eItot:
                            {
                                const std::vector<double>& IQ_end_o = tdata(tdata.diag_size()-1).data();
                                const double iTot = std::reduce(std::execution::par, IQ_end_o.cbegin(),IQ_end_o.cend());
                                diagnostics_stream_o << "," << iTot;
                            }
                            break;
                        case ic_pd::eNparticles:
                            {
                                const size_t num_traj_end = tdata(tdata.diag_size()-1).size();
                                diagnostics_stream_o << "," << num_traj_end;
                            }
                            break;
                        default:
                            std::cout<<"Particle Diagnostics: parameter *" << param << "* unknown, ignoring it." << std::endl;
                            diagnostics_stream_o << "," << "Unknown";
                            break;
                    }
                }

                tdata.clear();
            }
            diagnostics_stream_o << std::endl;
            diagnostics_stream_o.flush();

/*
        //try - da cancellare da qui in poi:
        //TrajectoryDiagnosticData tdata;
        //std::vector<trajectory_diagnostic_e> diag;
        diag.push_back( DIAG_R );
        diag.push_back( DIAG_RP );
        diag.push_back( DIAG_AP );
        diag.push_back( DIAG_CURR );
        pdb_o.trajectories_at_plane( 
                tdata, 
                AXIS_X, 
                0.5, 
                diag );
        */
        //Emittance statistics are from original data, not gridded data
        /*
        EmittanceConv emit( 
                100, // n Grid array n x m
                100, // m
                tdata(0).data(), //DIAG_R TrajectoryDiagnosticColumn
                tdata(1).data(), //DIAG_RP
                tdata(2).data(), //DIAG_AP
                tdata(3).data()  //DIAG_CURR I
                );

        std::cout << "Alpha" << emit.alpha() << std::endl;
        std::cout << "beta" << emit.beta() << std::endl;
        std::cout << "epsilon" << emit.beta() << std::endl;

        const int num_traj_end = tdata(3).size();
        const std::vector<double>& IQ_end_o = tdata(3).data();
        double IQ_end = 0.0;
        for(const double IQ: IQ_end_o) {
            IQ_end+= IQ;
        }

        std::cout << "NTraj end: " << num_traj_end <<std::endl;
        std::cout << "IQ End: " << IQ_end <<std::endl;

        */



            //const int num_traj_end = tdata(3).size();
            //const std::vector<double>& IQ_end_o = tdata(3).data();
            //const double iTot = std::reduce(std::execution::par, IQ_end_o.cbegin(),IQ_end_o.cend());
            //double IQ_end = 0.0;
            //for(const double IQ: IQ_end_o) {
            //    IQ_end+= IQ;
            //}
            //std::reduce C++17 not yet implemented
            //const double IQ_end = std::reduce(IQ_end_o.cbegin(), IQ_end_o.cend());


        };
    
}
