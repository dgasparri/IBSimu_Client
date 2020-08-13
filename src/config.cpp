//Compile with 
//g++ -o config config.cpp -lboost_program_options


/*

g++ -g `pkg-config --cflags ibsimu-1.0.6dev` -c -o config.o config.cpp -lboost_program_options
*/

#include "config.h"


namespace ic_config = ibsimu_client::config;





int ic_config::num_cores_m(bpo::variables_map &vm_o, int default_v)
{
    if(vm_o.count("ibsimu-cores"))
        return vm_o["ibsimu-cores"].as<int>();    
    else
        return default_v;
}

message_type_e ic_config::message_threshold_m(bpo::variables_map &vm_o, message_type_e default_v)
{
    if(vm_o.count("ibsimu-message-threshold")) {
        const std::string &s = vm_o["ibsimu-message-threshold"].as<std::string>();
        if(s == "MSG_VERBOSE") 
            return MSG_VERBOSE;
    }
    return default_v;

}


//argc == -1 -> prints help
//parameters_commandline_m
ibsimu_client::parameters_commandline_t* ic_config::parameters_commandline_m(int argc, char *argv[], bool is_simulation)
{
    bpo::options_description command_line_options_o("Command line options");
    command_line_options_o.add_options()
        ("help", "print help message")
        ("run", bpo::value<std::string>()->default_value(""), "directory containing the config.ini of the file. Output files will be written in that directory ")
        ("config-file", bpo::value<std::string>()->default_value(""), "configuration file, path relative to executable")
        ;

    if(is_simulation) 
        ibsimu_client::output::output_options_m(command_line_options_o);
        /*command_line_options_o.add_options()
            ("run-output", bpo::value<std::string>()->default_value("OUT_NORMAL"), "output files generated in the run [OUT_NORMAL (default, only final files), OUT_EVOLUTION (every 10 loops and last), OUT_BEGIN (first 3 loops and final), OUT_VERBOSE (first 3, every 10 loops and last)]")
            ("loop-output", bpo::value<std::string>()->default_value("LOOP_END"), "output files generated in the loop [LOOP_END (default, only at the end of the loop), LOOP_VERBOSE (every step of the loop)]")
        ;*/ 
    else
        command_line_options_o.add_options()
            ("epot-file", bpo::value<std::string>()->default_value(""), "epot file, path relative to executable [REQUIRED]")
            ("pdb-file", bpo::value<std::string>()->default_value(""), "pdb file, path relative to executable [REQUIRED]")
        ;

    
    //Prints help
    if(argc == -1) {
        std::cout<<command_line_options_o<<std::endl;
        return nullptr;
    }
    
    bpo::variables_map vm_cmdl_o;
    store(parse_command_line(argc, argv, command_line_options_o), vm_cmdl_o);

    ibsimu_client::parameters_commandline_t* options_op =
            new ibsimu_client::parameters_commandline_t; 
    
    options_op->run_o = vm_cmdl_o["run"].as<std::string>();
    options_op->config_filename_o = vm_cmdl_o["config-file"].as<std::string>();

    if(is_simulation) {
        options_op->run_output = ibsimu_client::output::output_options_run_output_m(vm_cmdl_o);
        options_op->loop_output = ibsimu_client::output::output_options_loop_output_m(vm_cmdl_o);
    }

    if(!is_simulation) {
        options_op->epot_filename_o = vm_cmdl_o["epot-file"].as<std::string>();
        options_op->pdb_filename_o = vm_cmdl_o["pdb-file"].as<std::string>();

    }

    return options_op;
} 

// configfile_o.empty() -> print help
// parameters_configfile_m
bpo::variables_map* ic_config::parameters_configfile_m(std::string config_filename_o) 
{


    bpo::options_description config_file_options_o("Config file options");
    config_file_options_o.add_options()
    
        ("ibsimu-cores", bpo::value<int>(), "Number of processor cores to use")
        ("number-of-rounds", bpo::value<int>()->default_value(50), "Number of rounds")
        ("ibsimu-message-threshold", bpo::value<std::string>(), "Output message threshold [MSG_VERBOSE]")
        ("ibsimu-file-emittance-statistics", bpo::value<std::string>(), "Filename to write emittance statistics to")
        

        ("ibsimu-file-prefix-geometry", bpo::value<std::string>()->default_value("geom"), "Prefix of file Geometry (.dat)")
        ("ibsimu-file-prefix-epot", bpo::value<std::string>()->default_value("epot"), "Prefix of file Epot (.dat)")
        ("ibsimu-file-prefix-pdb", bpo::value<std::string>()->default_value("pdb"), "Prefix of file Particle Database (.dat)")

        ("geometry-mode", bpo::value<std::string>(), "Geometry Mode (MODE_3D, MODE_CYL...)")
        ("origin-x", bpo::value<origin_t>(), "geometry origin x")
        ("origin-y", bpo::value<origin_t>(), "geometry origin y")
        ("origin-z", bpo::value<origin_t>(), "geometry origin z")
        ("mesh-cell-size-h", bpo::value<mesh_cell_size_h_t>(), "mesh cell size h")
        ("geometry-start-x", bpo::value<geometry_value_t>(), "geometry start x")
        ("geometry-start-y", bpo::value<geometry_value_t>(), "geometry start y")
        ("geometry-start-z", bpo::value<geometry_value_t>(), "geometry start z")
        ("geometry-size-x", bpo::value<geometry_value_t>(), "geometry size x")
        ("geometry-size-y", bpo::value<geometry_value_t>(), "geometry size y")
        ("geometry-size-z", bpo::value<geometry_value_t>(), "geometry size z")

        ("wall-1-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-1-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")
        ("wall-2-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-2-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")
        ("wall-3-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-3-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")
        ("wall-4-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-4-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")
        ("wall-5-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-5-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")
        ("wall-6-bound-type", bpo::value<bound_type_string_t>(), "wall bound type [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("wall-6-bound-voltage", bpo::value<voltage_t>(), "wall voltage V")

        ("dxf-filename", bpo::value<std::string>(), "dxf filename relative to the executable")
        ("dxfsolid-layername", bpo::value<std::vector<std::string>>(), "DSFSolid layername")
        ("dxfsolid-scalefactor", bpo::value<std::vector<dxf_scale_factor_t>>(), "DSFSolid scale factor")
        ("dxfsolid-bound-type", bpo::value<std::vector<bound_type_string_t>>(), "DSFSolid boundtype [BOUND_DIRICHLET, BOUND_NEUMANN]")
        ("dxfsolid-bound-voltage", bpo::value<std::vector<voltage_t>>(), "DSFSolid bound voltage")
        
        ("bfield-mode", bpo::value<std::string>(), "Bfield Mode (MODE_3D, MODE_CYL...)")
        ("bfield-filename", bpo::value<std::string>(), "Bfield filename, path relative to the executable")
        ("bfield-xscale", bpo::value<bfield_scale_factor_t>(), "DSFSolid scale xfactor")
        ("bfield-fscale", bpo::value<bfield_scale_factor_t>(), "DSFSolid scale ffactor")        
        ("bfield-sel-f1", bpo::value<bool>(), "bfield-sel-f1")
        ("bfield-sel-f2", bpo::value<bool>(), "bfield-sel-f2")
        ("bfield-sel-f3", bpo::value<bool>(), "bfield-sel-f3")
        ("bfield-translate-x", bpo::value<geometry_value_t>(), "bfield translate x")
        ("bfield-translate-y", bpo::value<geometry_value_t>(), "bfield translate y")
        ("bfield-translate-z", bpo::value<geometry_value_t>(), "bfield translate z")

        // ("ground-voltage", bpo::value<voltage_t>(), "ground voltage V")
        ("electron-temperature-Te", bpo::value<double>(), "Electron Temperature Te")
        ("plasma-potential-Up", bpo::value<double>(), "Plasma Potential Up")
        ("space-charge-alpha", bpo::value<double>(), "Sc_alpha (0.7)")

        ("plasma-init-x", bpo::value<double>(), "Plasma starting point X")
        ("plasma-init-y", bpo::value<double>(), "Plasma starting point Y")
        ("plasma-init-z", bpo::value<double>(), "Plasma starting point Z")


        ("beam-number-of-particles",       bpo::value<std::vector<int>>(), "Number of particles of the beam")
        ("beam-current-density",           bpo::value<std::vector<double>>(), "Current density A/m^2")
        ("beam-particle-charge",           bpo::value<std::vector<int>>(), "Particle charge (in multiples of e)")
        ("beam-mass",                      bpo::value<std::vector<double>>(), "Particle mass (u)")
        ("beam-mean-energy",               bpo::value<std::vector<double>>(), "Beam mean energy (eV)")
        ("beam-parallel-temperature-Tp",   bpo::value<std::vector<double>>(), "Beam parallel temperature Tp (eV)")
        ("beam-transverse-temperature-Tt", bpo::value<std::vector<double>>(), "Beam transverse temperature Tt (eV)")
        ("beam-vector-x1",                 bpo::value<std::vector<double>>(), "Beam vector x1")
        ("beam-vector-y1",                 bpo::value<std::vector<double>>(), "Beam vector y1")
        ("beam-vector-x2",                 bpo::value<std::vector<double>>(), "Beam vector x2")
        ("beam-vector-y2",                 bpo::value<std::vector<double>>(), "Beam vector y1")

    ;

    if(config_filename_o.empty()) {
        std::cout<<config_file_options_o<<std::endl;
        return nullptr;
    }

    bpo::variables_map* vm_op = new bpo::variables_map;
    //.c_str();
    store(parse_config_file(config_filename_o.c_str(), config_file_options_o, true), *vm_op);
    bpo::notify(*vm_op);
    return vm_op;
}

/*
analysis_parameters_t* analysis_parameters_m(int argc, char *argv[]) 
{

    bpo::options_description command_line_options_o("Command line options");
    command_line_options_o.add_options()
        ("help", "print help message")
        ("config-file", bpo::value<std::string>(), "configuration file, path relative to current directory")
        ("epot-file", bpo::value<std::string>(), "epot file, path relative to executable")
        ("pdb-file", bpo::value<std::string>(), "pdb file, path relative to executable")
        ("bfield-file", bpo::value<std::string>(), "bfield file, path relative to executable")

    ;


    bpo::variables_map vm_cmdl_o;
    store(parse_command_line(argc, argv, command_line_options_o), vm_cmdl_o);

    bpo::options_description config_file_options_o; // = config_file_options_m();

    bool all_files = true;
    if (!vm_cmdl_o.count("config-file")) 
        all_files = false;
    if (!vm_cmdl_o.count("epot-file")) 
        all_files = false;
    if (!vm_cmdl_o.count("pdb-file")) 
        all_files = false;

    if (!all_files) {
        std::cout<<"Usage:"<<std::endl<<" ./analysis --config-file config.ini --epot-file epot.dat --pdb-file pdb.dat"<<std::endl;
        std::cout << command_line_options_o <<std::endl;
        std::cout << config_file_options_o;
        return nullptr;
    }
    
    analysis_parameters_t *params = new analysis_parameters_t;

    bpo::variables_map vm_o;
    std::string epot_filename_o;
    std::string pdb_filename_o;


    run_parameters_t* vm_op = new run_parameters_t;
    const char* config_filename = vm_cmdl_o["config-file"].as<std::string>().c_str();
    store(parse_config_file(config_filename, config_file_options_o, true), *vm_op);
    bpo::notify(*vm_op);
    params->vm_op = vm_op;

    params->epot_filename_o = vm_cmdl_o["epot-file"].as<std::string>();
    params->pdb_filename_o = vm_cmdl_o["pdb-file"].as<std::string>();
   
    
    return params;


    

}
*/

void ic_config::show_help(bool is_simulation)
{
    parameters_commandline_m(-1, nullptr, is_simulation);
    parameters_configfile_m(to_string(""));
}

//clean_runpath
ibsimu_client::parameters_commandline_t* ic_config::clean_runpath_m(std::string current_directory, ibsimu_client::parameters_commandline_t* cmdlp_op)
{

    bool run = !cmdlp_op->run_o.empty();
    bool config = !cmdlp_op->config_filename_o.empty();
    /*
        !run & !config -> show help
        !run -> run to current dir 
        run -> clean run directory
        (run &) !config -> set config to rundir/config.ini
        (run &) config -> if config[0]!='/' set config to rundir+config

    */

    if (!run && !config) 
        return nullptr;

    if(!run) {
        cmdlp_op->run_o = to_string(current_directory) + "/";
    } 

    if(cmdlp_op->run_o[0] != '/') 
        cmdlp_op->run_o = to_string(current_directory) + "/" + cmdlp_op->run_o ;

    
    if ( cmdlp_op->run_o.find_last_of('/') != cmdlp_op->run_o.length() -1)
        cmdlp_op->run_o.append("/");

    if(!config) {
        cmdlp_op->config_filename_o = to_string("config.ini");
    }
    if(cmdlp_op->config_filename_o[0] != '/') 
        cmdlp_op->config_filename_o = cmdlp_op->run_o + cmdlp_op->config_filename_o;

    return cmdlp_op;

}






