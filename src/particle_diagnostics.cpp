#include "particle_diagnostics.h"


namespace ic_pd = ibsimu_client::particle_diagnostics;

void ic_pd::particle_diagnostics_options_m(
    bpo::options_description &command_line_options_o)
{
        command_line_options_o.add_options()
            ("ibsimu-beam-diagnostics-file", bpo::value<std::string>(), "Filename to write emittance statistics to")
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
    if (param == PD_X_AVERAGE) return ic_pd::eXaverage;
    if (param == PD_X_P_AVE) return ic_pd::eXpave;
    if (param == PD_I_TOT) return ic_pd::eItot;
    if (param == PD_N_PARTICLES) return ic_pd::eNparticles;
    return ic_pd::eUnknown;
}


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
}




std::ofstream& ic_pd::diagnostics_stream_open_m(bpo::variables_map &params_op, std::string fullpath)
{

    const std::string& stats_filename_o = 
        params_op["ibsimu-beam-diagnostics-file"].as<std::string>();
    std::string fullpath_stats_filename_o;
    if(stats_filename_o[0]=='/')
        fullpath_stats_filename_o = stats_filename_o;
    else
        fullpath_stats_filename_o = fullpath + stats_filename_o;

    std::ofstream *csv_op = new std::ofstream(
        fullpath_stats_filename_o, 
        std::ios_base::out | std::ios_base::trunc );
    
    return *csv_op;
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




std::optional<ic_pd::loop_end_m_t> 
    ic_pd::particle_diagnostics_factory_m(
        bpo::variables_map& params_o,
        std::ofstream& diagnostics_stream_o
   )
{
    
    const std::vector<std::string> &diag_at_loop = params_o["diagnostics-at-loop"].as<std::vector<std::string>>();
    const std::vector<std::string> &diag_axis_str = params_o["diagnostics-axis"].as<std::vector<std::string>>();
    const std::vector<double> &diag_plane_coordinate = params_o["diagnostics-plane-coordiante"].as<std::vector<double>>();
    const std::vector<std::string> &diag_extr_params_str = params_o["diagnostics-extraction-parameters"].as<std::vector<std::string>>();

    const std::size_t extractions = diag_at_loop.size();
    
    if(diag_axis_str.size() != extractions
       || diag_plane_coordinate.size() != extractions 
       || diag_extr_params_str.size() != extractions)
    {
        std::cout << "There must be the same number of diagnostics-at-loop, diagnostics-axis, diagnostics-plane-coordiante and diagnostics-extraction-parameters." << std::endl;
        std::cout << "Particle diagnostics unable to instantiate. Aborting. "  << std::endl;
        return std::nullopt;
    }

    //std::unordered_set<std::string> axes = {PD_AXIS_X, PD_AXIS_Y, PD_AXIS_Z, PD_AXIS_R};
    std::vector<coordinate_axis_e> diag_axis;
    for(std::string axis: diag_axis_str)
    {
        boost::algorithm::trim(axis);
        if(axis == PD_AXIS_X) diag_axis.emplace_back(AXIS_X);
            else
        if(axis == PD_AXIS_Y) diag_axis.emplace_back(AXIS_Y);
            else
        if(axis == PD_AXIS_Z) diag_axis.emplace_back(AXIS_Z);
            else
        if(axis == PD_AXIS_R) diag_axis.emplace_back(AXIS_R);
            else
        {
            std::cout << "Axis " << axis << " unknown." << std::endl;
            std::cout << "Particle diagnostics unable to instantiate. Aborting. "  << std::endl;
            return std::nullopt;
        }
    }

    //TODO: Check diag_plane_coordinate against geometry_o.max(0), 

    std::vector<std::vector<std::string>> diag_extr_params;
    for(auto &params_str: diag_extr_params_str)
    {
        diag_extr_params.push_back(ic_pd::diagnostic_parameters_vector_m(params_str));
    }

    return [
        &params_o,
        &diagnostics_stream_o,
        diag_at_loop,
        diag_axis,
        diag_plane_coordinate,
        diag_extr_params
        ](
            int loop_n,
            ParticleDataBase& pdb_o
        ) {

            //No extraction to do, no need to load data
            std::size_t extractions = diag_axis.size();
            if(!extractions)
                return;            

            TrajectoryDiagnosticData tdata;
            
            std::vector<trajectory_diagnostic_e> diag;

            diag.push_back( DIAG_R );
            diag.push_back( DIAG_RP );
            diag.push_back( DIAG_AP );
            diag.push_back( DIAG_CURR );
            
            //Iniziare loop
            for(std::size_t i = 0; i < extractions; i++) {
                pdb_o.trajectories_at_plane( 
                        tdata, 
                        diag_axis[i], 
                        diag_plane_coordinate[i], 
                        diag );
                
                //Emittance statistics are from original data, not gridded data
                EmittanceConv emit( 
                        PD_GRID_N , // n Grid array n x m
                        PD_GRID_M, // m
                        tdata(0).data(), //DIAG_R TrajectoryDiagnosticColumn
                        tdata(1).data(), //DIAG_RP
                        tdata(2).data(), //DIAG_AP
                        tdata(3).data()  //DIAG_CURR I
                        );

                for(auto param: diag_extr_params[i]) {
                    switch(ic_pd::hash_parameters(param)) {
                        case ic_pd::eAlpha:
                            diagnostics_stream_o << "," << emit.alpha();
                            break;
                        case ic_pd::eBeta:
                            diagnostics_stream_o << "," << emit.beta();
                            break;
                        case ic_pd::eGamma:
                            diagnostics_stream_o << "," << emit.gamma();
                            break;
                        case ic_pd::eEpsilon:
                            diagnostics_stream_o << "," << emit.epsilon();
                            break;
                        case ic_pd::eAngle:
                            diagnostics_stream_o << "," << emit.angle();
                            break;
                        case ic_pd::eRmajor:
                            diagnostics_stream_o << "," << emit.rmajor();
                            break;
                        case ic_pd::eRminor:
                            diagnostics_stream_o << "," << emit.rminor();
                            break;
                        case ic_pd::eItot:
                            {
                                const std::vector<double>& IQ_end_o = tdata(3).data();
                                const double iTot = std::reduce(std::execution::par, IQ_end_o.cbegin(),IQ_end_o.cend());
                                diagnostics_stream_o << "," << iTot;
                                break;
                            }
                        case ic_pd::eNparticles:
                            {
                                const size_t num_traj_end = tdata(3).size();
                                diagnostics_stream_o << "," << num_traj_end;
                                break;
                            }
                        default:
                            std::cout<<"Particle Diagnostics: parameter *" << param << "* unknown, ignoring it." << std::endl;
                    }
                }

                diagnostics_stream_o << std::endl;
                diagnostics_stream_o.flush();
                diag.clear();
                tdata.clear();
            }
            
            
            





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
