

#include "config-setup.h"



namespace ic_setup = ibsimu_client::setup;

bound_type_t ic_setup::bound_type_m(const bound_type_string_t &s) 
{
    if(s == "BOUND_DIRICHLET")
        return BOUND_DIRICHLET;
    if (s == "BOUND_NEUMANN")
        return BOUND_NEUMANN;
    
    throw new Error("Config-file error: Bound type unknown ");
     
}



geom_mode_e ic_setup::geometry_mode_m(std::string gm_string_o)
{
    geom_mode_e geometry_mode;
    if (gm_string_o == "MODE_3D")
        geometry_mode = MODE_3D;
    else if (gm_string_o == "MODE_CYL")
        geometry_mode = MODE_CYL;
    else if (gm_string_o == "MODE_2D")
        geometry_mode = MODE_2D;
    else if (gm_string_o == "MODE_1D")
        geometry_mode = MODE_1D;
    else
        throw new Error("Config-file error: geometry-mode not implemented");
    return geometry_mode;
}

Geometry* ic_setup::geometry_m(bpo::variables_map &vm_o) 
{

    const std::string &gm_string_o = vm_o["geometry-mode"].as<std::string>();
    geom_mode_e geometry_mode = geometry_mode_m(gm_string_o);

    const origin_t origin_x = vm_o["origin-x"].as<origin_t>();
    const origin_t origin_y = vm_o["origin-y"].as<origin_t>();
    const origin_t origin_z = vm_o["origin-z"].as<origin_t>();
    
    const mesh_cell_size_h_t mesh_cell_size_h = vm_o["mesh-cell-size-h"].as<mesh_cell_size_h_t>();
    const geometry_value_t geometry_start_x = vm_o["geometry-start-x"].as<geometry_value_t>();
    const geometry_value_t geometry_start_y = vm_o["geometry-start-y"].as<geometry_value_t>();
    const geometry_value_t geometry_start_z = vm_o["geometry-start-z"].as<geometry_value_t>();
    const geometry_value_t geometry_size_x = vm_o["geometry-size-x"].as<geometry_value_t>() - geometry_start_x;
    const geometry_value_t geometry_size_y = vm_o["geometry-size-y"].as<geometry_value_t>() - geometry_start_y;
    const geometry_value_t geometry_size_z = vm_o["geometry-size-z"].as<geometry_value_t>() - geometry_start_z;
    
    
    const Vec3D origin_o(origin_x, origin_y, origin_z );

    const Int3D size_3D_o(
        (int)floor(geometry_size_x/mesh_cell_size_h)+1,
        (int)floor(geometry_size_y/mesh_cell_size_h)+1,
        (int)floor(geometry_size_z/mesh_cell_size_h)+1
        );
    
    Geometry *geometry_op = new Geometry(geometry_mode, size_3D_o, origin_o, mesh_cell_size_h);
    
    return geometry_op;
}

namespace ic_beam = ibsimu_client::beam;
std::vector<ic_beam::beam_t> ic_setup::beams_m(bpo::variables_map &vm_o) 
{


    std::vector<int>    nump   = vm_o["beam-number-of-particles"      ].as<std::vector<int>>();
    std::vector<double> curr   = vm_o["beam-current-density"          ].as<std::vector<double>>();
    std::vector<int>    charge = vm_o["beam-particle-charge"          ].as<std::vector<int>>();
    std::vector<double> mass   = vm_o["beam-mass"                     ].as<std::vector<double>>();
    std::vector<double> mean_E = vm_o["beam-mean-energy"              ].as<std::vector<double>>();
    std::vector<double> Tp     = vm_o["beam-parallel-temperature-Tp"  ].as<std::vector<double>>();
    std::vector<double> TT     = vm_o["beam-transverse-temperature-Tt"].as<std::vector<double>>();
    std::vector<double> x1     = vm_o["beam-vector-x1"                ].as<std::vector<double>>();
    std::vector<double> y1     = vm_o["beam-vector-y1"                ].as<std::vector<double>>();
    std::vector<double> x2     = vm_o["beam-vector-x2"                ].as<std::vector<double>>();
    std::vector<double> y2     = vm_o["beam-vector-y2"                ].as<std::vector<double>>();

    const int size = nump.size();
    std::vector<ic_beam::beam_t> beams;

    for(int i = 0; i< size; i++) 
    {
        ic_beam::beam_t b;
        b.n_particles = nump[i];
        b.current_density_Am2 = curr[i];
        b.particle_charge = charge[i];
        b.mass = mass[i];
        b.mean_energy = mean_E[i];
        b.par_temp_Tp = Tp[i];
        b.trans_temp_Tt = TT[i];
        b.x1 = x1[i];
        b.y1 = y1[i];
        b.x2 = x2[i];
        b.y2 = y2[i];
        beams.emplace_back(b);
    }
    return beams;       
}


void ic_setup::wall_bound_m(Geometry &geometry_o, bpo::variables_map &vm_o)
{
    //ITERATING through [wall-1-bound-type... wall-6-bound-type]
    
    std::string label_begin_o = "wall-";
    std::string label_type_end_o = "-bound-type";
    std::string label_voltage_end_o = "-bound-voltage";
    for(int i = 1; i <= 6; i++) {
        std::string label_type_o    = label_begin_o + std::to_string(i) + label_type_end_o;
        std::string label_voltage_o = label_begin_o + std::to_string(i) + label_voltage_end_o;
        if(vm_o.count(label_type_o)) {
            const bound_type_string_t bound_type_string_o = vm_o[label_type_o].as<bound_type_string_t>();
            const voltage_t bound_voltage = vm_o[label_voltage_o].as<voltage_t>();    
            const bound_type_t bound_type = bound_type_m(bound_type_string_o);

            geometry_o.set_boundary( i, Bound(bound_type, bound_voltage) );
        }
    }

}


void ic_setup::add_solid_m(Geometry &geometry_o,
                 int progressive, 
                 MyDXFFile *dxffile_op,
                 const std::string &layer_name_o, 
                 dxf_scale_factor_t dxf_scale_factor,
                 bound_type_t bound_type,
                 voltage_t  bound_V  ) 
{
    
    DXFSolid *s = new DXFSolid( dxffile_op, layer_name_o);
    s->scale( (double) dxf_scale_factor );
    geometry_o.set_solid(progressive, s);
    geometry_o.set_boundary(progressive, 
                Bound(bound_type, (double) bound_V));

}



void ic_setup::dxfsolids_m(Geometry &geometry_o, bpo::variables_map &vm_o, std::string rundir_o) 
{

    const std::string &dxf_filename_o = vm_o["dxf-filename"].as<std::string>();
    std::string filename_o;

    if(dxf_filename_o[0] == '/')
        filename_o = dxf_filename_o;
    else
        filename_o = rundir_o + dxf_filename_o;



    MyDXFFile *dxffile_op = new MyDXFFile;
    dxffile_op->set_warning_level( 3 );

    try {
        dxffile_op->read( filename_o);
    } catch (Error &e) {
        const std::string &message = e.get_error_message();
        std::cout << std::endl;
        std::cout << "Error in .dxf file: " << message;
        std::cout << std::endl;
         
        //e.print_error_message(std::cout);
    }
    



    const std::vector<std::string> &layer_names_o = 
                vm_o["dxfsolid-layername"].as<std::vector<std::string>>();

    const std::vector<dxf_scale_factor_t> &dxf_scale_factors_o = 
                vm_o["dxfsolid-scalefactor"].as<std::vector<dxf_scale_factor_t>>();

    const std::vector<voltage_t> &dxf_bound_voltages_o = 
                vm_o["dxfsolid-bound-voltage"].as<std::vector<voltage_t>>();

    const std::vector<std::string> &dxf_bound_type_strings_o = 
                vm_o["dxfsolid-bound-type"].as<std::vector<std::string>>();
  
    
    for(unsigned int i=0; i< layer_names_o.size(); i++) {

        const int progressive = i+ 7; // 0..6 are reserved
        const bound_type_t bound_type = bound_type_m(dxf_bound_type_strings_o[i]);

        std::cout<<i<<": "<<layer_names_o[i]<<" "
                    <<dxf_scale_factors_o[i]<<" "
                    <<bound_type<<": "
                    <<dxf_bound_voltages_o[i]<<std::endl;

        add_solid_m(geometry_o,
                    progressive, 
                    dxffile_op,
                    layer_names_o[i], 
                    dxf_scale_factors_o[i],
                    bound_type,
                    dxf_bound_voltages_o[i]) ;


    }

}



MeshVectorField* ic_setup::bfield_m(Geometry &geometry_o, bpo::variables_map &vm_o, std::string rundir_o)
{
    const std::string &bfield_mode_string_o = vm_o["bfield-mode"].as<std::string>();
    const std::string &bfield_filename_o = vm_o["bfield-filename"].as<std::string>();
    const bfield_scale_factor_t bfield_xscale = vm_o["bfield-xscale"].as<bfield_scale_factor_t>();
    const bfield_scale_factor_t bfield_fscale = vm_o["bfield-fscale"].as<bfield_scale_factor_t>();
    const bool sel_f1 = vm_o["bfield-sel-f1"].as<bool>();
    const bool sel_f2 = vm_o["bfield-sel-f2"].as<bool>();    
    const bool sel_f3 = vm_o["bfield-sel-f3"].as<bool>();    
    const geometry_value_t bfield_translate_x = vm_o["bfield-translate-x"].as<geometry_value_t>();
    const geometry_value_t bfield_translate_y = vm_o["bfield-translate-y"].as<geometry_value_t>();
    const geometry_value_t bfield_translate_z = vm_o["bfield-translate-z"].as<geometry_value_t>();


    std::string fullpath_filename_o;
    if(bfield_filename_o[0]=='/')
        fullpath_filename_o = bfield_filename_o;
    else
        fullpath_filename_o = rundir_o + bfield_filename_o;

    geom_mode_e bfield_mode = geometry_mode_m(bfield_mode_string_o);

    bool fsel[3] = {sel_f1, sel_f2, sel_f3};
    MeshVectorField *bfield = 
        new MeshVectorField(
            bfield_mode, 
            fsel, 
            (double) bfield_xscale, 
            (double) bfield_fscale, 
            fullpath_filename_o);
    bfield->translate( Vec3D(
                        bfield_translate_x,
                        bfield_translate_y,
                        bfield_translate_z) );

    return bfield;
}




ibsimu_client::physics_parameters_t* ic_setup::physics_parameters_m(bpo::variables_map &vm_o)
{
    double Te = vm_o["electron-temperature-Te"].as<double>();
    double Up = vm_o["plasma-potential-Up"].as<double>();
    double sc_alpha = vm_o["space-charge-alpha"].as<double>();
    // voltage_t gndV = vm_o["ground-voltage"].as<voltage_t>();

    ibsimu_client::physics_parameters_t* phypars = new ibsimu_client::physics_parameters_t;
    phypars->electron_temperature_Te = Te;
    phypars->plasma_potential_Up = Up;
    phypars->space_charge_alpha = sc_alpha;
    // phypars->ground_V = gndV;

    std::function<double(const char*)> plasma_init_m = [&vm_o](const char* str) {
        if(vm_o.count(str)) 
            return vm_o[str].as<double>();
        return std::numeric_limits<double>::quiet_NaN();
    };

    phypars->plasma_init_x  = plasma_init_m("plasma-init-x");
    phypars->plasma_init_y  = plasma_init_m("plasma-init-y");
    phypars->plasma_init_z  = plasma_init_m("plasma-init-z");


/*
    double plasma_init_x  = vm_o["plasma-init-x"].as<double>();
    double plasma_init_y  = vm_o["plasma-init-y"].as<double>();
    double plasma_init_z  = vm_o["plasma-init-z"].as<double>();
    phypars->plasma_init_x  = plasma_init_x ;
    phypars->plasma_init_y  = plasma_init_y ;
    phypars->plasma_init_z  = plasma_init_z ;
    */

    return phypars;
}

