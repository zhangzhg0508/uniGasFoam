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
    Foam::writeTimeData

Description
    Class for outputing field data.

Source Files
    writeTimeData.C


\*---------------------------------------------------------------------------*/

#ifndef writeTimeData_H
#define writeTimeData_H

#include "vector.H"
#include "complex.H"
#include "List.H"
#include "IOdictionary.H"
#include "dictionary.H"
#include "autoPtr.H"
#include "polyMesh.H"
#include "volFields.H"
#include "Time.H"
#include "OFstream.H"
#include "IFstream.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
                        Class writeTimeData Declaration
\*---------------------------------------------------------------------------*/

class writeTimeData
{
    // Private data

public:

    // Constructors

        writeTimeData
        (
            const fileName& pathName,
            const word& fileName,
            const List<Pair<scalar>>& data
        );

        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const List<Pair<scalar>>& data,
            const label dummy
        );

        //- scalar field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData
        );

        //- vector field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& xData
        );

        //- scalar field, scalar field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const scalarField& yData
        );

        //- vector field, scalar field,
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& xData,
            const scalarField& yData
        );

        //- scalar field, vector field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const vectorField& yData
        );

        //- scalar field, complex field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const List<complex>& yData
        );

        //- vector field, vector field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& xData,
            const vectorField& yData
        );

        //- scalar field, tensor field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const tensorField& yData
        );

        //- vector field, tensor field
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& xData,
            const tensorField& yData
        );

        // WITH APPEND POSSIBLE

        // two scalar fields (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const scalarField& yData,
            const bool dummy
        );

        // one scalar field (with append possible)[OLD]
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const bool dummy
        );


        // one scalar field -[NEW] - (with append possible)
        // options:
        // sidewaysAppend
        // append
        // once

        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const word& option,
            const bool dummy
        );


        // one vector field - NEW (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& xData,
            const word& option
        );

        // one scalar field one VECTOR field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const vectorField& yData,
            const bool dummy
        );

        // one VECTOR field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const vectorField& yData,
            const bool dummy
        );

        // one scalar field one SYMMTENSOR field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const symmTensorField& yData,
            const bool dummy
        );

        // one scalar field one TENSOR field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const tensorField& yData,
            const bool dummy
        );

        // one scalar field and one List<scalarField> (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const List<scalarField>& yData,
            const bool dummy
        );

        // one scalar field and one complex field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const scalarField& xData,
            const List<complex>& yData,
            const bool dummy
        );

        // one TENSOR field (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const tensorField& xData,
            const bool dummy
        );



        // write out List<scalarField>
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const List<scalarField>& data
        );

        // write out List<vectorField> component only
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const List<vectorField>& data,
            const word& option
        );

        // write out List<vectorField> component only (with append possible)
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const List<vectorField>& data,
            const word& option,
            const bool dummy
        );


        // write out List<tensorField> component only
        writeTimeData
        (
            const fileName& pathName,
            const word& nameFile,
            const List<tensorField>& data,
            const word& option
        );


        ~writeTimeData();


    // Member Functions


    // Ostream Operator


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
