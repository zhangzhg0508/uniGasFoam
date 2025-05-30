/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019-2023 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

namespace Foam
{

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const List<List<scalarField>>& boundaryMeasurements::rhoNIntBF() const
{
    return rhoNIntBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::rhoNElecBF() const
{
    return rhoNElecBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::rhoNBF() const
{
    return rhoNBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::rhoMBF() const
{
    return rhoMBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::linearKEBF() const
{
    return linearKEBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::mccSpeciesBF() const
{
    return mccSpeciesBF_;
}


inline const List<List<vectorField>>& boundaryMeasurements::momentumBF() const
{
    return momentumBF_;
}


inline const List<List<vectorField>>& boundaryMeasurements::UMeanBF() const
{
    return UMeanBF_;
}


inline const List<List<scalarField>>&
boundaryMeasurements::rotationalEBF() const
{
    return rotationalEBF_;
}


inline const List<List<scalarField>>&
boundaryMeasurements::rotationalDofBF() const
{
    return rotationalDofBF_;
}


inline const List<List<scalarField>>&
boundaryMeasurements::vibrationalEBF() const
{
    return vibrationalEBF_;
}


inline const List<List<scalarField>>&
boundaryMeasurements::electronicEBF() const
{
    return electronicEBF_;
}


inline const List<List<scalarField>>& boundaryMeasurements::qBF() const
{
    return qBF_;
}


inline const List<List<vectorField>>& boundaryMeasurements::fDBF() const
{
    return fDBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rhoNIntBF()
{
    return rhoNIntBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rhoNElecBF()
{
    return rhoNElecBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rhoNBF()
{
    return rhoNBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rhoMBF()
{
    return rhoMBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::linearKEBF()
{
    return linearKEBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::mccSpeciesBF()
{
    return mccSpeciesBF_;
}


inline List<List<vectorField>>& boundaryMeasurements::momentumBF()
{
    return momentumBF_;
}


inline List<List<vectorField>>& boundaryMeasurements::UMeanBF()
{
    return UMeanBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rotationalEBF()
{
    return rotationalEBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::rotationalDofBF()
{
    return rotationalDofBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::vibrationalEBF()
{
    return vibrationalEBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::electronicEBF()
{
    return electronicEBF_;
}


inline List<List<scalarField>>& boundaryMeasurements::qBF()
{
    return qBF_;
}


inline List<List<vectorField>>& boundaryMeasurements::fDBF()
{
    return fDBF_;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
