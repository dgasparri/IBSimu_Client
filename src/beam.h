#pragma once

#
#include <particledatabase.hpp>

#include <vector>
#include <functional>

namespace ibsimu_client::beam {

    struct beam_t {
        int n_particles;
        double current_density_Am2; //A/m^2
        int particle_charge;
        double mass;
        double mean_energy;
        double par_temp_Tp;
        double trans_temp_Tt;
        double x1;
        double y1;
        double x2;
        double y2;

    };


    typedef std::function<void(ParticleDataBaseCyl&)> add_2d_beams_mt;

    add_2d_beams_mt add_2d_beams_helper_m(std::vector<beam_t> beams);
}