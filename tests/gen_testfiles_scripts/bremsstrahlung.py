import pyPROPOSAL as pp
import numpy as np

parametrizations = [
    pp.Parametrization.Bremsstrahlung.KelnerKokoulinPetrukhin,
    pp.Parametrization.Bremsstrahlung.PetrukhinShestakov,
    pp.Parametrization.Bremsstrahlung.CompleteScreening,
    pp.Parametrization.Bremsstrahlung.AndreevBezrukovBugaev
]

particle_defs = [
    pp.MuMinusDef.get(),
    pp.TauMinusDef.get(),
    pp.EMinusDef.get()
]

mediums = [
    pp.Medium.Ice(1.0),
    pp.Medium.Hydrogen(1.0),
    pp.Medium.Uranium(1.0)
]

cuts = [
    pp.EnergyCutSettings(-1, -1),
    pp.EnergyCutSettings(500, -1),
    pp.EnergyCutSettings(-1, 0.05),
    pp.EnergyCutSettings(500, 0.05)
]

multiplier = 1.

lpms = [0, 1]

energies = np.logspace(4, 13, num=10)

interpoldef = pp.InterpolationDef()


def create_table_dEdx():

    with open("Brems_dEdx.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Int = pp.CrossSection.BremsIntegral(brems)

                                buf = [""]

                                dEdx = Brems_Int.calculate_dEdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(dEdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx():

    with open("Brems_dNdx.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Int = pp.CrossSection.BremsIntegral(brems)

                                buf = [""]

                                dNdx = Brems_Int.calculate_dNdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(dNdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_rnd():

    pp.RandomGenerator.get().set_seed(1234)

    with open("Brems_dNdx_rnd.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            rnd = pp.RandomGenerator.get().random_double()
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Int = pp.CrossSection.BremsIntegral(brems)

                                buf = [""]

                                dNdx = Brems_Int.calculate_dNdx_rnd(energy, rnd)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(rnd))
                                buf.append(str(dNdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_stochastic_loss():

    pp.RandomGenerator.get().set_seed(0)

    with open("Brems_e.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            rnd1 = pp.RandomGenerator.get().random_double()
                            rnd2 = pp.RandomGenerator.get().random_double()
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Int = pp.CrossSection.BremsIntegral(brems)

                                buf = [""]

                                stochastic_loss = Brems_Int.calculate_stochastic_loss(energy, rnd1, rnd2)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(rnd1))
                                buf.append(str(rnd2))
                                buf.append(str(stochastic_loss))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dEdx_interpol():

    with open("Brems_dEdx_interpol.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Interpol = pp.CrossSection.BremsInterpolant(brems, interpoldef)

                                buf = [""]

                                dEdx = Brems_Interpol.calculate_dEdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(dEdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_interpol():

    with open("Brems_dNdx_interpol.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Interpol = pp.CrossSection.BremsInterpolant(brems, interpoldef)

                                buf = [""]

                                dNdx = Brems_Interpol.calculate_dNdx(energy)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(dNdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_dNdx_rnd_interpol():

    pp.RandomGenerator.get().set_seed(1234)

    with open("Brems_dNdx_rnd_interpol.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            rnd = pp.RandomGenerator.get().random_double()
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Interpol = pp.CrossSection.BremsInterpolant(brems, interpoldef)

                                buf = [""]

                                dNdx = Brems_Interpol.calculate_dNdx_rnd(energy, rnd)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(rnd))
                                buf.append(str(dNdx))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def create_table_stochastic_loss_interpol():

    pp.RandomGenerator.get().set_seed(1234)

    with open("Brems_e_interpol.txt", "a") as f:

        for particle in particle_defs:
            for medium in mediums:
                for cut in cuts:
                    for lpm  in lpms:
                        for energy in energies:
                            rnd1 = pp.RandomGenerator.get().random_double()
                            rnd2 = pp.RandomGenerator.get().random_double()
                            for parametrization in parametrizations:

                                brems = parametrization(
                                    particle,
                                    medium,
                                    cut,
                                    multiplier,
                                    lpm)

                                Brems_Interpol = pp.CrossSection.BremsInterpolant(brems, interpoldef)

                                buf = [""]

                                stochastic_loss = Brems_Interpol.calculate_stochastic_loss(energy, rnd1, rnd2)

                                buf.append(particle.name)
                                buf.append(medium.name)
                                buf.append(str(cut.ecut))
                                buf.append(str(cut.vcut))
                                buf.append(str(multiplier))
                                buf.append(str(lpm))
                                buf.append(str(energy))
                                buf.append(str(rnd1))
                                buf.append(str(rnd2))
                                buf.append(str(stochastic_loss))
                                buf.append(brems.name)
                                buf.append("\n")

                                # print(buf)
                                f.write("\t".join(buf))


def main():
    create_table_dEdx()
    create_table_dNdx()
    create_table_dNdx_rnd()
    create_table_stochastic_loss()
    create_table_dEdx_interpol()
    create_table_dNdx_interpol()
    create_table_dNdx_rnd_interpol()
    create_table_stochastic_loss_interpol()


if __name__ == "__main__":
    main()