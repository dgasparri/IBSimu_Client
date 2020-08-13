#include "beam.h"




ibsimu_client::beam::add_2d_beams_mt ibsimu_client::beam::add_2d_beams_helper_m(std::vector<ibsimu_client::beam::beam_t> beams) 
{
    return [beams](ParticleDataBaseCyl &pdb) 
    {
        for(beam_t beam: beams) {        
            pdb.add_2d_beam_with_energy( 
                beam.n_particles,
                beam.current_density_Am2,
                beam.particle_charge,
                beam.mass,
                beam.mean_energy,
                beam.par_temp_Tp,
                beam.trans_temp_Tt,
                beam.x1, 
                beam.y1,
                beam.x2, 
                beam.y2);

        }
    };

}


