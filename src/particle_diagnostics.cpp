#include "particle_diagnostics.h"


namespace ic_pd = ibsimu_client::particle_diagnostics;

void ic_pd::particle_diagnostics_options_m(
    bpo::options_description &command_line_options_o)
{
        command_line_options_o.add_options()
            ("ibsimu-beam-diagnostics-file", bpo::value<std::string>(), "Filename to write emittance statistics to")
            ("emission-x", bpo::value<std::string>()->default_value("OUT_NORMAL"), "emission x output files generated in the run [OUT_NORMAL (default, only final files), OUT_EVOLUTION (every 10 loops and last), OUT_BEGIN (first 3 loops and final), OUT_VERBOSE (first 3, every 10 loops and last)]")
            ("emission-y", bpo::value<std::string>()->default_value("LOOP_END"), "emission y output files generated in the loop [LOOP_END (default, only at the end of the loop), LOOP_VERBOSE (every step of the loop)]")
        ;

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




ic_pd::loop_end_m_t 
    ic_pd::particle_diagnostics_factory_m(
        bpo::variables_map& params_op,
        std::ofstream& diagnostics_stream_o
   )
{
    
    return [
        &params_op,
        &diagnostics_stream_o
        ](
            int loop_n,
            ParticleDataBase& pdb_o
        ) {
            TrajectoryDiagnosticData tdata;
            std::vector<trajectory_diagnostic_e> diag;
            diag.push_back( DIAG_R );
            diag.push_back( DIAG_RP );
            diag.push_back( DIAG_AP );
            diag.push_back( DIAG_CURR );
            pdb_o.trajectories_at_plane( 
                    tdata, 
                    AXIS_X, 
                    0.1, //geometry_o.max(0), 
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

            diagnostics_stream_o << "," << emit.alpha();
            diagnostics_stream_o << "," << emit.beta();
            diagnostics_stream_o << "," << emit.epsilon();

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


            diagnostics_stream_o << "," << num_traj_begin;
            diagnostics_stream_o << "," << IQ_begin;
            diagnostics_stream_o << "," << num_traj_end;
            diagnostics_stream_o << "," << IQ_end;

            diagnostics_stream_o << std::endl;
            diagnostics_stream_o.flush();

        };
    
}
