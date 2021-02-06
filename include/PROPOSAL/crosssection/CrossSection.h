
/******************************************************************************
 *                                                                            *
 * This file is part of the simulation tool PROPOSAL.                         *
 *                                                                            *
 * Copyright (C) 2017 TU Dortmund University, Department of Physics,          *
 *                    Chair Experimental Physics 5b                           *
 *                                                                            *
 * This software may be modified and distributed under the terms of a         *
 * modified GNU Lesser General Public Licence version 3 (LGPL),               *
 * copied verbatim in the file "LICENSE".                                     *
 *                                                                            *
 * Modifcations to the LGPL License:                                          *
 *                                                                            *
 *      1. The user shall acknowledge the use of PROPOSAL by citing the       *
 *         following reference:                                               *
 *                                                                            *
 *         J.H. Koehne et al.  Comput.Phys.Commun. 184 (2013) 2070-2090 DOI:  *
 *         10.1016/j.cpc.2013.04.001                                          *
 *                                                                            *
 *      2. The user should report any bugs/errors or improvments to the       *
 *         current maintainer of PROPOSAL or open an issue on the             *
 *         GitHub webpage                                                     *
 *                                                                            *
 *         "https://github.com/tudo-astroparticlephysics/PROPOSAL"            *
 *                                                                            *
 ******************************************************************************/

#pragma once

#include "PROPOSAL/EnergyCutSettings.h"
#include "PROPOSAL/Logging.h"
#include "PROPOSAL/crosssection/CrossSectionDE2DX/CrossSectionDE2DXBuilder.h"
#include "PROPOSAL/crosssection/CrossSectionDEDX/CrossSectionDEDXBuilder.h"
#include "PROPOSAL/crosssection/CrossSectionDNDX/CrossSectionDNDXBuilder.h"
#include "PROPOSAL/crosssection/parametrization/Parametrization.hpp"
#include "PROPOSAL/medium/Components.h"
#include "PROPOSAL/medium/Medium.h"
#include "PROPOSAL/particle/ParticleDef.h"
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace PROPOSAL {

using rates_t = std::unordered_map<std::shared_ptr<const Component>, double>;

struct CrossSectionBase {

    virtual ~CrossSectionBase() = default;
    virtual double CalculatedEdx(double) = 0;
    virtual double CalculatedE2dx(double) = 0;
    virtual double CalculatedNdx(
        double, std::shared_ptr<const Component> = nullptr)
        = 0;
    virtual double CalculateStochasticLoss(
        std::shared_ptr<const Component> const&, double, double)
        = 0;
    virtual double GetLowerEnergyLim() const = 0;
    virtual size_t GetHash() const noexcept = 0;
    virtual InteractionType GetInteractionType() const noexcept = 0;
    virtual std::vector<std::shared_ptr<const Component>>
    GetTargets() const noexcept = 0;
};

namespace detail {
    inline auto weight_component(Medium const& m, Component const& c)
    {
        return m.GetSumNucleons() / (c.GetAtomInMolecule() * c.GetAtomicNum());
    }

    template <typename Param>
    inline auto build_dndx(std::false_type, bool interpol, Param param,
        ParticleDef p, Medium m, std::shared_ptr<const EnergyCutSettings> cut,
        size_t hash = 0)
    {
        using comp_ptr = std::shared_ptr<const Component>;
        using dndx_ptr = std::unique_ptr<CrossSectionDNDX>;
        auto dndx_map
            = std::unordered_map<comp_ptr, std::tuple<double, dndx_ptr>>();
        auto calc = make_dndx(interpol, param, p, m, cut, hash);
        dndx_map.emplace(
            comp_ptr(nullptr), std::make_tuple(1., std::move(calc)));
        return dndx_map;
    }

    template <typename Param>
    inline auto build_dndx(std::true_type, bool interpol, Param param,
        ParticleDef p, Medium m, std::shared_ptr<const EnergyCutSettings> cut,
        size_t hash = 0)
    {
        using comp_ptr = std::shared_ptr<const Component>;
        using dndx_ptr = std::unique_ptr<CrossSectionDNDX>;
        auto dndx_map
            = std::unordered_map<comp_ptr, std::tuple<double, dndx_ptr>>();
        for (auto& c : m.GetComponents()) {
            auto comp = std::make_shared<const Component>(c);
            auto weight = weight_component(m, c);
            auto calc = make_dndx(interpol, param, p, c, cut, hash);
            dndx_map.emplace(comp, std::make_tuple(weight, std::move(calc)));
        }
        return dndx_map;
    }

    template <typename Cont, typename T1, typename T2, typename T3,
        typename... Args>
    inline auto _build_dedx(Cont container, std::true_type, bool interpol,
        T1&& param, T2&& p_def, T3 const& target, Args&&... args)
    {
        for (auto& c : target.GetComponents())
            container->emplace_back(weight_component(target, c),
                make_dedx(interpol, std::forward<T1>(param),
                    std::forward<T2>(p_def), c, args...));
    }

    template <typename Cont, typename T1, typename T2, typename T3,
        typename... Args>
    inline auto _build_dedx(Cont container, std::false_type, bool interpol,
        T1&& param, T2&& p_def, T3 const& target, Args&&... args)
    {
        container->emplace_back(1.,
            make_dedx(interpol, std::forward<T1>(param),
                std::forward<T2>(p_def), target, args...));
    }

    template <typename Type, typename T1, typename T2, typename T3, typename T4,
        typename... Args>
    inline auto build_dedx(Type comp_wise, bool interpol, T1&& param,
        T2&& p_def, T3&& target, T4 cut_ptr, Args&&... args)
    {
        using dedx_ptr = std::unique_ptr<CrossSectionDEDX>;
        auto dedx = std::unique_ptr<std::vector<std::tuple<double, dedx_ptr>>>(
            nullptr);
        if (cut_ptr) {
            dedx
                = std::make_unique<std::vector<std::tuple<double, dedx_ptr>>>();
            _build_dedx(dedx.get(), comp_wise, interpol,
                std::forward<T1>(param), std::forward<T2>(p_def),
                std::forward<T3>(target), *cut_ptr,
                std::forward<Args>(args)...);
        }
        return dedx;
    }

    template <typename Cont, typename T1, typename T2, typename T3,
        typename... Args>
    inline auto _build_de2dx(Cont container, std::true_type, bool interpol,
        T1&& param, T2&& p_def, T3 const& target, Args&&... args)
    {
        for (auto& c : target.GetComponents())
            container->emplace_back(weight_component(target, c),
                make_de2dx(interpol, std::forward<T1>(param),
                    std::forward<T2>(p_def), c, args...));
    }

    template <typename Cont, typename T1, typename T2, typename T3,
        typename... Args>
    inline auto _build_de2dx(Cont container, std::false_type, bool interpol,
        T1&& param, T2&& p_def, T3 const& target, Args&&... args)
    {
        container->emplace_back(1.,
            make_de2dx(interpol, std::forward<T1>(param),
                std::forward<T2>(p_def), target, args...));
    }

    template <typename Type, typename T1, typename T2, typename T3, typename T4,
        typename... Args>
    inline auto build_de2dx(Type comp_wise, bool interpol, T1&& param,
        T2&& p_def, T3&& target, T4 cut_ptr, Args&&... args)
    {
        using de2dx_ptr = std::unique_ptr<CrossSectionDE2DX>;
        using de2dx_container = std::vector<std::tuple<double, de2dx_ptr>>;
        auto de2dx = std::unique_ptr<de2dx_container>(nullptr);
        if (!cut_ptr)
            return de2dx;
        if (cut_ptr->GetContRand()) {
            de2dx = std::make_unique<de2dx_container>();
            _build_de2dx(de2dx.get(), comp_wise, interpol,
                std::forward<T1>(param), std::forward<T2>(p_def),
                std::forward<T3>(target), *cut_ptr,
                std::forward<Args>(args)...);
        }
        return de2dx;
    }

}

template <typename comp_wise, typename only_stochastic>
class CrossSection : public CrossSectionBase {
    /* using param_t = std::remove_reference_t<std::remove_cv_t<Param>>; */
    /* using comp_wise = crosssection::is_component_wise<param_t>; */
    /* using only_stochastic = crosssection::is_only_stochastic<param_t>; */

    using comp_ptr = std::shared_ptr<const Component>;
    using dndx_ptr = std::unique_ptr<CrossSectionDNDX>;
    using dedx_ptr = std::unique_ptr<CrossSectionDEDX>;
    using de2dx_ptr = std::unique_ptr<CrossSectionDE2DX>;

protected:
    size_t hash;

    std::unordered_map<comp_ptr, std::tuple<double, dndx_ptr>> dndx;
    std::unique_ptr<std::vector<std::tuple<double, dedx_ptr>>> dedx;
    std::unique_ptr<std::vector<std::tuple<double, de2dx_ptr>>> de2dx;

    double lower_energy_lim;
    InteractionType interaction_type;

public:
    template <typename Param>
    CrossSection(Param param, ParticleDef p, Medium m,
        std::shared_ptr<const EnergyCutSettings> cut, bool interpol,
        size_t hash = 0)
        : hash(0)
        , dndx(detail::build_dndx(comp_wise {}, interpol, param, p, m, cut))
        , dedx(detail::build_dedx(comp_wise {}, interpol, param, p, m, cut))
        , de2dx(detail::build_de2dx(comp_wise {}, interpol, param, p, m, cut))
        , lower_energy_lim(param.GetLowerEnergyLim(p))
        , interaction_type(static_cast<InteractionType>(
              crosssection::ParametrizationId<Param>::value))
    {
    }

    virtual ~CrossSection() = default;

protected:
    double CalculatedNdx(
        double E, std::shared_ptr<const Component> c, std::true_type)
    {
        if (c)
            return std::get<1>(dndx[c])->Calculate(E) / std::get<0>(dndx[c]);
        auto dNdx_all = 0.;
        for (auto& it : dndx)
            dNdx_all += std::get<1>(dndx[it.first])->Calculate(E)
                / std::get<0>(dndx[it.first]);
        return dNdx_all;
    }

    double CalculatedNdx(
        double E, std::shared_ptr<const Component>, std::false_type)
    {
        return std::get<1>(dndx[nullptr])->Calculate(E);
    }

    double CalculateStochasticLoss_impl(
        std::shared_ptr<const Component> const& c, double E, double rate,
        std::true_type, std::false_type)
    {
        return std::get<1>(dndx[c])->GetUpperLimit(
            E, rate * std::get<0>(dndx[c]));
    }

    double CalculateStochasticLoss_impl(
        std::shared_ptr<const Component> const& c, double E, double rate,
        std::false_type, std::false_type)
    {
        return std::get<1>(dndx[c])->GetUpperLimit(E, rate);
    }

    double CalculateStochasticLoss_impl(std::shared_ptr<const Component> const&,
        double, double, bool, std::true_type)
    {
        return 1.;
    }

public:
    double CalculatedNdx(
        double E, std::shared_ptr<const Component> c = nullptr) final
    {
        return CalculatedNdx(E, c, comp_wise {});
    };

    double CalculateStochasticLoss(
        std::shared_ptr<const Component> const& c, double E, double rate)
    {
        return CalculateStochasticLoss_impl(
            c, E, rate, comp_wise {}, only_stochastic {});
    }

    inline double CalculatedEdx(double energy) override
    {
        auto loss = 0.;
        if (not dedx)
            return loss;
        for (auto& [weight, calc] : *dedx)
            loss += calc->Calculate(energy) / weight;
        return loss;
    }

    inline double CalculatedE2dx(double energy) override
    {
        auto loss = 0.;
        if (not de2dx)
            return loss;
        for (auto& [weight, calc] : *de2dx)
            loss += calc->Calculate(energy) / weight;
        return loss;
    }

    std::vector<std::shared_ptr<const Component>>
    GetTargets() const noexcept final
    {
        std::vector<std::shared_ptr<const Component>> targets;
        for (auto& it : this->dndx)
            targets.emplace_back(it.first);
        return targets;
    }

    size_t GetHash() const noexcept final { return hash; }

    inline double GetLowerEnergyLim() const override
    {
        return lower_energy_lim;
    }

    inline InteractionType GetInteractionType() const noexcept override
    {
        return interaction_type;
    }
};

using cross_t_ptr = std::unique_ptr<CrossSectionBase>;

template <typename P, typename M>
using crosssection_list_t = std::vector<std::shared_ptr<CrossSectionBase>>;
} // namespace PROPOSAL
