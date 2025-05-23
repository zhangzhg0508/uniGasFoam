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

Class
    Foam::uniGasField

Description
    Abstract class for macroscopic quantity fields.

SourceFiles
    uniGasField.C

\*---------------------------------------------------------------------------*/

#ifndef uniGasField_H
#define uniGasField_H

#include "autoPtr.H"
#include "vector.H"
#include "Random.H"
#include "uniGasParcel.H"
#include "writeTimeData.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
                          Class uniGasField Declaration
\*---------------------------------------------------------------------------*/

class uniGasField
{
protected:

    // Protected data

        const fvMesh& mesh_;

        uniGasCloud& cloud_;

        //- Time properties dictionary
        dictionary timeDict_;

        label sampleInterval_;

        bool resetFieldsAtOutput_;

        scalar resetFieldsAtOutputUntilTime_;

        fileName casePath_;

        fileName timePath_;


public:

    //- Runtime type information
    TypeName("uniGasField");

    // Declare runtime constructor selection table
    declareRunTimeSelectionTable
    (
        autoPtr,
        uniGasField,
        dictionary,
        (
            const Time& t,
            const polyMesh& mesh,
            uniGasCloud& cloud,
            const dictionary& dict
        ),
        (t, mesh, cloud, dict)
    );


    // Constructors

        //- Construct from components
        uniGasField
        (
            const Time& t,
            const polyMesh& mesh,
            uniGasCloud& cloud,
            const dictionary& dict
        );


    // Selectors

        static autoPtr<uniGasField> New
        (
            const Time& t,
            const polyMesh& mesh,
            uniGasCloud& cloud,
            const dictionary& dict
        );

    //- Destructor
    virtual ~uniGasField() = default;


    // Member Functions

        const label& sampleInterval() const;

        const bool& resetFieldsAtOutput() const;

        const scalar& resetFieldsAtOutputUntilTime() const;

        const fileName& casePath() const;
        fileName& casePath();

        const fileName& timePath() const;
        fileName& timePath();

        //- Create initial field
        virtual void createField() = 0;

        //- Calculate field
        virtual void calculateField() = 0;

        //- Write field
        virtual void writeField() = 0;

        virtual void updateProperties(const dictionary& dict);
        
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
