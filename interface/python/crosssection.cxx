#include "PROPOSAL/crosssection/CrossSection.h"
#include "PROPOSAL/crosssection/CrossSectionInterpolantBase.h"
#include "PROPOSAL/math/InterpolantBuilder.h"
#include "PROPOSAL/crosssection/ParticleDefaultCrossSectionList.h"

#include "PROPOSAL/crosssection/parametrization/Parametrization.h"
#include "PROPOSAL/crosssection/parametrization/Annihilation.h"
#include "PROPOSAL/crosssection/parametrization/Bremsstrahlung.h"
#include "PROPOSAL/crosssection/parametrization/Compton.h"
#include "PROPOSAL/crosssection/parametrization/EpairProduction.h"
#include "PROPOSAL/crosssection/parametrization/Ionization.h"
#include "PROPOSAL/crosssection/parametrization/MupairProduction.h"
#include "PROPOSAL/crosssection/parametrization/Photonuclear.h"
#include "PROPOSAL/crosssection/parametrization/PhotoPairProduction.h"
#include "PROPOSAL/crosssection/parametrization/PhotoQ2Integration.h"
#include "PROPOSAL/crosssection/parametrization/PhotoRealPhotonAssumption.h"
#include "PROPOSAL/crosssection/parametrization/WeakInteraction.h"

#include "PROPOSAL/medium/Medium.h"
#include "PROPOSAL/particle/ParticleDef.h"
#include "PROPOSAL/particle/Particle.h"

#include "pyBindings.h"
#include <type_traits>

namespace py = pybind11;
using namespace PROPOSAL;

template <typename T> void build_crosssection(py::module& m_sub)
{
    m_sub.def("make_crosssection",
            [](T& param, ParticleDef& p, Medium& m,
                shared_ptr<const EnergyCutSettings> c, bool i) {
            return std::shared_ptr<CrossSection<ParticleDef, Medium>>(
                    make_crosssection(param, p, m, c, i));
            },
            py::arg("parametrization"), py::arg("particle_def"), py::arg("target"),
            py::arg("cuts"), py::arg("interpolate"));
}

template <typename T> void build_std_crosssection(py::module& m_sub)
{
    m_sub.def("make_std_crosssection",
            [](T& p, Medium& m, shared_ptr<const EnergyCutSettings> c, bool i) {
            return DefaultCrossSections<T>::template Get<std::false_type>(p, m, c, i);
            },
            py::arg("particle_def"), py::arg("target"), py::arg("cuts"), py::arg("interpolate"));
}

void init_crosssection(py::module& m)
{
    using CrossSectionContainer = CrossSection<ParticleDef, Medium>;
    py::module m_sub = m.def_submodule("crosssection");

    py::class_<CrossSectionBase, std::shared_ptr<CrossSectionBase>>(
            m_sub, "CrossSectionBase")
        .def_property_readonly("lower_energy_lim", &CrossSectionContainer::GetLowerEnergyLim)
        .def_property_readonly("type", &CrossSectionContainer::GetInteractionType);

    py::class_<CrossSectionInterpolantBase, std::shared_ptr<CrossSectionInterpolantBase>>(
            m_sub, "CrossSectionInterpolantBase")
        .def_readwrite_static("dNdx_def", &CrossSectionInterpolantBase::dNdx_def)
        .def_readwrite_static("dEdx_def", &CrossSectionInterpolantBase::dEdx_def)
        .def_readwrite_static("dE2dx_def", &CrossSectionInterpolantBase::dE2dx_def);

    py::class_<CrossSectionContainer, CrossSectionBase, std::shared_ptr<CrossSectionContainer>>(
            m_sub, "CrossSection",
            R"pbdoc(

            Virtual class for crosssections. The crosssection class provides
            all mathematical methods to process the theoretical, differential
            crosssections that are given by the parametrizations. A cross
            section class can be initialized with the following parameters

            Args:
                param (:meth:`~proposal.parametrization`): parametrization for
                    the crosssection, including the chosen theoretical model
                interpolation_def (:meth:`~proposal.InterpolationDef`): Only
                    needed by Interpolant parametrizations. Includes settings
                    for the interpolation

            The crosssection class can either work with interpolation tables
            or with exact intergration for every single calculation.  Since the
            usage of interpolation tables can improve the speed of the
            propagation by several orders of magnitude (with neglible decline
            in accuracy) it is highly recommended to use the interpolation
            methods.

            There are specific crosssection classes for every interaction that
            can be used.

            For propagation of massive leptons, there are the following
            crosssections:

            * AnnihilationIntegral / AnnihilationInterpolant
            * BremsIntegral / BremsInterpolant
            * EpairIntegral / EpairInterpolant
            * IonizIntegral / IonizInterpolant
            * MupairIntegral / MupairInterpolant
            * PhotoIntegral / PhotoInterpolant
            * WeakIntegral / WeakInterpolant

            For propagation of photons, there are the following crosssections:

            * ComptonIntegral / ComptonInterpolant
            * PhotoPairIntegral / PhotoPairInterpolant

            Example:
                To create a bremsstrahlung CrossSection

                >>> mu = proposal.particle.MuMinusDef.get()
                >>> medium = proposal.medium.StandardRock(1.0)
                >>> cuts = proposal.EnergyCutSettings(-1, -1)
                >>> interpol = proposal.InterpolationDef
                >>> param = proposal.parametrization.bremsstrahlung.SandrockSoedingreksoRhode(mu, medium, cuts, 1.0, False)
                >>> cross = proposal.crosssection.BremsInterpolant(param, interpol)
                >>> cross.calculate_dEdx(1e6) # exmaple usage of the created crosssection class...
                )pbdoc")
        /* .def("__str__", &py_print<CrossSectionContainer>) */
        .def("calculate_dEdx", py::vectorize(&CrossSectionContainer::CalculatedEdx),
                py::arg("energy"),
                R"pbdoc(

            Calculates the continous energy loss :math:`\langle \frac{dE}{dx} \rangle`,
            which equals to

                .. math:: \frac{N_A}{A} \cdot E \cdot \int_{v_{min}}^{v_{cut}} v \cdot \frac{d\sigma}{dv} dv

            with the particle energy E, the relative energy loss v and the
            crosssection :math:`\sigma`. The value :math:`v_{cut}` is the
            energy cut to differentiate between continous and stochastic losses
            in PROPOSAL, see :meth:`~proposal.EnergyCutSettings` for more
            information on the energy cuts.

            Args:
                energy (float): energy in MeV

                )pbdoc")
        .def("calculate_dE2dx", py::vectorize(&CrossSectionContainer::CalculatedE2dx),
                py::arg("energy"),
                R"pbdoc(

        Calculates the value

            .. math:: \frac{N_A}{A} \cdot E^2 \cdot \int_{v_{min}}^{v_{cut}} v^2 \cdot \frac{d\sigma}{dv} dv

        with the particle energy E, the relative energy loss v and the
        crosssection :math:`\sigma`. The value :math:`v_{cut}` is the
        energy cut to differentiate between continous and stochastic losses
        in PROPOSAL, see :meth:`~proposal.EnergyCutSettings` for more
        information on the energy cuts.

        The value is important for the calculation of the
        ContinuousRandomization (see :meth:`~proposal.ContinuousRandomizer`)

        Args:
            energy (float): energy in MeV

            )pbdoc")
        .def("calculate_dNdx", &CrossSectionContainer::CalculatedNdx,
                py::arg("energy"),
                py::arg("component"),
                R"pbdoc(

        Calculates the total cross section

            .. math:: \frac{N_A}{A} \cdot \int_{v_{cut}}^{v_{max}} \frac{d\sigma}{dv} dv

        with the particle energy E, the relative energy loss v and the
        crosssection :math:`\sigma`. The value v_{cut} is the energy cut to
        differentiate between continous and stochastic losses in PROPOSAL,
        see :meth:`~proposal.EnergyCutSettings` for more information on the
        energy cuts.

        Note that this integral only includes the v values about our cut,
        therefore this values represents only the total crosssection for the
        stochastic energy losses.

        Args:
            energy (float): energy in MeV

            )pbdoc")
        .def("calculate_stochastic_loss",
                &CrossSectionContainer::CalculateStochasticLoss,
                py::arg("component"), py::arg("energy"), py::arg("rate"),
                R"pbdoc(

        Samples a stochastic energy loss for a particle of the energy E.

        Args:
            energy (float): energy in MeV
            rnd1 (float): random number between 0 and 1, samples the energy
                loss fraction
            rnd2 (float): random number between 0 and 1, sampled the
                component where the energy loss in occuring

        Returns:
            sampled energy loss for the current particle in MeV

        With inverse transform sampling, using rnd1, the fraction of the
        energy loss v is determined from the differential crosssection.  By
        comparing the total cross sections for every medium, rnd2 is used
        to determine the component of the current medium for which the
        stochatic energy loss is calculated.

            )pbdoc");

    build_crosssection<crosssection::AnnihilationHeitler>(m_sub);

    build_crosssection<crosssection::BremsPetrukhinShestakov>(m_sub);
    build_crosssection<crosssection::BremsKelnerKokoulinPetrukhin>(m_sub);
    build_crosssection<crosssection::BremsCompleteScreening>(m_sub);
    build_crosssection<crosssection::BremsAndreevBezrukovBugaev>(m_sub);
    build_crosssection<crosssection::BremsSandrockSoedingreksoRhode>(m_sub);
    build_crosssection<crosssection::BremsElectronScreening>(m_sub);

    build_crosssection<crosssection::ComptonKleinNishina>(m_sub);

    build_crosssection<crosssection::EpairKelnerKokoulinPetrukhin>(m_sub);
    build_crosssection<crosssection::EpairSandrockSoedingreksoRhode>(m_sub);

    build_crosssection<crosssection::IonizBetheBlochRossi>(m_sub);
    build_crosssection<crosssection::IonizBergerSeltzerBhabha>(m_sub);
    build_crosssection<crosssection::IonizBergerSeltzerMoller>(m_sub);

    build_crosssection<crosssection::MupairKelnerKokoulinPetrukhin>(m_sub);

    build_crosssection<crosssection::PhotoPairTsai>(m_sub);

    build_crosssection<crosssection::PhotoAbramowiczLevinLevyMaor91>(m_sub);
    build_crosssection<crosssection::PhotoAbramowiczLevinLevyMaor97>(m_sub);
    build_crosssection<crosssection::PhotoButkevichMikheyev>(m_sub);
    build_crosssection<crosssection::PhotoRenoSarcevicSu>(m_sub);

    /* build_crosssection<crosssection::PhotoZeus>(m_sub); */
    /* build_crosssection<crosssection::PhotoBezrukovBugaev>(m_sub); */
    /* build_crosssection<crosssection::PhotoKokoulin>(m_sub); */
    /* build_crosssection<crosssection::PhotoRhode>(m_sub); */

    build_crosssection<crosssection::WeakCooperSarkarMertsch>(m_sub);

    build_std_crosssection<GammaDef>(m_sub);
    build_std_crosssection<EMinusDef>(m_sub);
    build_std_crosssection<EPlusDef>(m_sub);
    build_std_crosssection<MuMinusDef>(m_sub);
    build_std_crosssection<MuPlusDef>(m_sub);
    build_std_crosssection<TauMinusDef>(m_sub);
    build_std_crosssection<TauPlusDef>(m_sub);

}
