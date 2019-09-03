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
#include <exception>
#include <functional>
#include <string>
#include "PROPOSAL/math/Vector3D.h"

namespace PROPOSAL {
class Axis {
   public:
    Axis();
    Axis(Vector3D fp0, Vector3D fAxis);
    virtual ~Axis(){};

    virtual Axis* clone() const = 0;

    virtual double GetDepth(Vector3D xi) const = 0;
    virtual double GetEffectiveDistance(Vector3D xi,
                                        Vector3D direction) const = 0;

    Vector3D GetAxis() { return fAxis_; };
    Vector3D GetFp0() { return fp0_; };

   protected:
    Vector3D fAxis_;
    Vector3D fp0_;
};
}  // namespace PROPOSAL

namespace PROPOSAL {
class RadialAxis : public Axis {
   public:
    RadialAxis();
    RadialAxis(Vector3D fAxis, Vector3D fp0);

    Axis* clone() const override { return new RadialAxis(*this); };
    ~RadialAxis(){};

    double GetDepth(Vector3D xi) const override;
    double GetEffectiveDistance(Vector3D xi, Vector3D direction) const override;
};
}  // namespace PROPOSAL

namespace PROPOSAL {
class CartesianAxis : public Axis {
   public:
    CartesianAxis();
    CartesianAxis(Vector3D fAxis, Vector3D fp0);
    ~CartesianAxis(){};

    Axis* clone() const override { return new CartesianAxis(*this); };

    double GetDepth(Vector3D xi) const override;
    double GetEffectiveDistance(Vector3D xi, Vector3D direction) const override;
};
}  // namespace PROPOSAL

namespace PROPOSAL {
class DensityException : public std::exception {
   public:
    DensityException(const char* m) : message_(m){};
    const char* what() const throw() { return message_.c_str(); };

   private:
    std::string message_;
};
}  // namespace PROPOSAL

namespace PROPOSAL {
class Density_distr {
   public:
    Density_distr();
    Density_distr(const Axis& axis);
    Density_distr(const Density_distr&);

    virtual ~Density_distr() { delete axis_; };

    virtual Density_distr* clone() const = 0;

    virtual double Correct(Vector3D xi,
                           Vector3D direction,
                           double res,
                           double distance_to_border) const = 0;
    virtual double Integrate(Vector3D xi,
                             Vector3D direction,
                             double l) const = 0;
    virtual double Calculate(Vector3D xi,
                             Vector3D direction,
                             double distance) const = 0;
    virtual double Evaluate(Vector3D xi,
                            Vector3D direction,
                            double distance) const = 0;

   protected:
    Axis* axis_;
};
}  // namespace PROPOSAL

