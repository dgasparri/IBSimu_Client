#include bpo_interface.h

namespace ic_bpo = ibsimu_client::bpo_interface;

template <typename T>
const std::optional<T> ic_bpo::get(bpo::variables_map &params_op, std::string key)
{
    try {
        const T& value = 
            params_op[key].as<T>();

    } 
    catch(const std::exception& e) {
        //std::cout << e.what() << std::endl;
        //return 1;
        return std::nullopt;

    }

    return value;
}