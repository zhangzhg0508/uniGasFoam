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

#include "variableSoftSphere.H"
#include "constants.H"
#include "addToRunTimeSelectionTable.H"

using namespace Foam::constant::mathematical;

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
defineTypeNameAndDebug(variableSoftSphere, 0);
addToRunTimeSelectionTable
(
    dsmcCollisionModel,
    variableSoftSphere,
    dictionary
);
};

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::variableSoftSphere::variableSoftSphere
(
    const dictionary& dict,
    uniGasCloud& cloud
)
:
    dsmcCollisionModel(dict, cloud),
    propertiesDict_(dict.subDict("collisionProperties")),
    Tref_(propertiesDict_.get<scalar>("Tref"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::variableSoftSphere::active() const
{
    return true;
}


Foam::scalar Foam::variableSoftSphere::sigmaTcR
(
    const uniGasParcel& pP,
    const uniGasParcel& pQ
) const
{
    const scalar cR2 = magSqr(pP.U() - pQ.U());

    if (cR2 < VSMALL)
    {
        return 0;
    }

    const label typeIdP = pP.typeId();
    const label typeIdQ = pQ.typeId();

    const scalar dPQ =
        0.5
       *(
            cloud_.constProps(typeIdP).d()
          + cloud_.constProps(typeIdQ).d()
        );

    const scalar omegaPQ =
        0.5
       *(
            cloud_.constProps(typeIdP).omega()
          + cloud_.constProps(typeIdQ).omega()
        );

    const scalar mP = cloud_.constProps(typeIdP).mass();

    const scalar mQ = cloud_.constProps(typeIdQ).mass();

    const scalar mR = mP*mQ/(mP + mQ);

    // Calculating cross section = pi*dPQ^2, where dPQ is from Bird, eq. 4.79
    const scalar sigmaTPQ =
        pi*dPQ*dPQ
       *pow(2.0*physicoChemical::k.value()*Tref_/(mR*cR2), omegaPQ - 0.5)
       /exp(Foam::lgamma(2.5 - omegaPQ));

    return sigmaTPQ*Foam::sqrt(cR2);
}


void Foam::variableSoftSphere::collide
(
    uniGasParcel& pP,
    uniGasParcel& pQ,
    label& cellI
)
{
    const label typeIdP = pP.typeId();
    const label typeIdQ = pQ.typeId();
    vector& UP = pP.U();
    vector& UQ = pQ.U();

    const scalar alphaPQ =
        0.5
        *(
            cloud_.constProps(typeIdP).alpha()
          + cloud_.constProps(typeIdQ).alpha()
        );

    Random& rndGen(cloud_.rndGen());

    const scalar mP = cloud_.constProps(typeIdP).mass();

    const scalar mQ = cloud_.constProps(typeIdQ).mass();

    vector Ucm = (mP*UP + mQ*UQ)/(mP + mQ);

    scalar cR = mag(UP - UQ);

    vector cRComponents = UP - UQ;

    scalar cosTheta =
        2.0*(pow(rndGen.sample01<scalar>(), (1.0/alphaPQ))) - 1.0;

    scalar sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    scalar phi = twoPi*rndGen.sample01<scalar>();

    scalar D =
        sqrt(cRComponents.y()*cRComponents.y()
      + cRComponents.z()*cRComponents.z());

    // Bird, equation 2.22
    vector postCollisionRelU
    (
        cosTheta*cRComponents.x() + sinTheta*sin(phi)*D,
        cosTheta*cRComponents.y() + sinTheta
            *(cR*cRComponents.z()*cos(phi)
            - cRComponents.x()*cRComponents.y()
            *sin(phi))/D,
        cosTheta*cRComponents.z() - sinTheta
            *(cR*cRComponents.y()*cos(phi)
            + cRComponents.x()*cRComponents.z()
            *sin(phi))/D
    );

    UP = Ucm + postCollisionRelU*mQ/(mP + mQ);

    UQ = Ucm - postCollisionRelU*mP/(mP + mQ);

}


const Foam::dictionary& Foam::variableSoftSphere::propertiesDict() const
{
    return propertiesDict_;
}


// ************************************************************************* //
