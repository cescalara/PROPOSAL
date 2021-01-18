
#include <cmath>
#include <functional>

#include "PROPOSAL/crosssection/CrossSection.h"
#include "PROPOSAL/crosssection/parametrization/Parametrization.h"
#include "PROPOSAL/math/InterpolantBuilder.h"
#include "PROPOSAL/propagation_utility/PropagationUtilityIntegral.h"

#include "PROPOSAL/Constants.h"
#include "PROPOSAL/Logging.h"

using namespace PROPOSAL;

UtilityIntegral::UtilityIntegral(
    std::function<double(double)> _func, double _lower_lim, size_t _hash)
    : integral(IROMB, IMAXS, IPREC2)
    , lower_lim(_lower_lim)
    , FunctionToIntegral(_func)
    , hash(_hash)
{
}

double UtilityIntegral::Calculate(double energy_initial, double energy_final)
{
    return integral.Integrate(
        energy_initial, energy_final, FunctionToIntegral, 4);
}

double UtilityIntegral::GetUpperLimit(double energy_initial, double rnd)
{
    auto sum = integral.IntegrateWithRandomRatio(
        energy_initial, lower_lim, FunctionToIntegral, 4, -rnd);

    assert(sum > rnd); // searched Energy is below lower_lim return lower_lim as
                       // a lower limit
    (void)sum;

    return integral.GetUpperLimit();
}
