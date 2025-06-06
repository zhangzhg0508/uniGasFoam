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
    Foam::uniGasDynamicLoadBalancing

Description
    Class for measuring processor imbalance in a parallel simulation.

SourceFiles
    uniGasDynamicLoadBalancing.C

\*---------------------------------------------------------------------------*/

#ifndef uniGasDynamicLoadBalancing_H
#define uniGasDynamicLoadBalancing_H

#include "uniGasParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class uniGasDynamicLoadBalancing Declaration
\*---------------------------------------------------------------------------*/

class uniGasDynamicLoadBalancing
{
    // Private data

        const Time& time_;

        uniGasCloud& cloud_;

public:

    friend class uniGasParcel;

    // Constructors

         //- Constructor
        uniGasDynamicLoadBalancing
        (
            const Time& t,
            uniGasCloud& cloud
        );

    // Destructor

        virtual ~uniGasDynamicLoadBalancing() = default;

    // Member Functions
        
        void calculate
        (
        );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//#include "uniGasDynamicLoadBalancingI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //