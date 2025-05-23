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

\*----------------------------------------------------------------------------*/

#include "uniGasFaceTracker.H"
#include "processorPolyPatch.H"
#include "cyclicPolyPatch.H"
#include "wallPolyPatch.H"
#include "uniGasCloud.H"

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

uniGasFaceTracker::uniGasFaceTracker
(
    const polyMesh& mesh,
    uniGasCloud& cloud
)
:
    mesh_(refCast<const fvMesh>(mesh)),
    cloud_(cloud)
{}


uniGasFaceTracker::uniGasFaceTracker
(
    const polyMesh& mesh,
    uniGasCloud& cloud,
    const bool dummy
)
:
    mesh_(refCast<const fvMesh>(mesh)),
    cloud_(cloud),
    parcelIdFlux_(cloud_.typeIdList().size()),
    massIdFlux_(cloud_.typeIdList().size()),
    momentumIdFlux_(cloud_.typeIdList().size()),
    energyIdFlux_(cloud_.typeIdList().size())
{
    forAll(parcelIdFlux_, i)
    {
        parcelIdFlux_[i].setSize(mesh_.nFaces(), 0.0);
        massIdFlux_[i].setSize(mesh_.nFaces(), 0.0);
        momentumIdFlux_[i].setSize(mesh_.nFaces(), vector::zero);
        energyIdFlux_[i].setSize(mesh_.nFaces(), 0.0);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void uniGasFaceTracker::clean()
{
    // clean geometric fields
    forAll(parcelIdFlux_, i)
    {
        parcelIdFlux_[i] = scalar(0.0);
        massIdFlux_[i] = scalar(0.0);
        momentumIdFlux_[i] = vector::zero;
        energyIdFlux_[i] = scalar(0.0);
    }
}


void uniGasFaceTracker::updateFields
(
    uniGasParcel& p
)
{   
    const label& crossedFace = p.face();
    const label& typeId = p.typeId();
    const uniGasParcel::constantProperties& constProps = cloud_.constProps(typeId);
    const scalar& CWF = p.CWF();
    const scalar& RWF = cloud_.axiRWF(p.position());
    const scalar& mass = constProps.mass();
    const vector& U = p.U();
    
    scalar e = 0.5*mass*(U & U) + p.ERot() + constProps.electronicEnergyList()[p.ELevel()];
    if (constProps.vibrationalDoF() > 0)
    {
        forAll(p.vibLevel(), i)
        {
            e += p.vibLevel()[i]*constProps.thetaV()[i]*physicoChemical::k.value();
        }
    }

    // check which patch was hit
    const label patchId = mesh_.boundaryMesh().whichPatch(crossedFace);

    // direction of uniGasParcel trajectory with respect to the face normal
    scalar sgn = sign(U & mesh_.faceAreas()[crossedFace]) * 1.0;

    // geometric fields

    if (patchId != -1) // boundary face
    {
        const polyPatch& patch = mesh_.boundaryMesh()[patchId];

        const label faceIndex = crossedFace-patch.start();
 
        if (isA<cyclicPolyPatch>(patch)) // boundary cyclic faces
        {
            label coupledFace = refCast<const cyclicPolyPatch>(patch).neighbPatch().start() + faceIndex;

            parcelIdFlux_[typeId][coupledFace] += CWF*RWF;
            massIdFlux_[typeId][coupledFace] += mass*CWF*RWF;
            momentumIdFlux_[typeId][coupledFace] += mass*U*CWF*RWF;
            energyIdFlux_[typeId][coupledFace] += e*CWF*RWF;
            
        }
        else  // boundary non-cyclic faces
        {
            parcelIdFlux_[typeId][crossedFace] += sgn*CWF*RWF;
            massIdFlux_[typeId][crossedFace] += sgn*mass*CWF*RWF;
            momentumIdFlux_[typeId][crossedFace] += mass*U*CWF*RWF;
            energyIdFlux_[typeId][crossedFace] += sgn*e*CWF*RWF;
        }
    }
    else // internal face
    {
        parcelIdFlux_[typeId][crossedFace] += sgn*CWF*RWF;
        massIdFlux_[typeId][crossedFace] += sgn*mass*CWF*RWF;
        momentumIdFlux_[typeId][crossedFace] += mass*U*CWF*RWF;
        energyIdFlux_[typeId][crossedFace] += sgn*e*CWF*RWF;
    }

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
