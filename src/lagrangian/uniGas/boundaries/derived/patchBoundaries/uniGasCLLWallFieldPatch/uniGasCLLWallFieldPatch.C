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

#include "uniGasCLLWallFieldPatch.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
defineTypeNameAndDebug(uniGasCLLWallFieldPatch, 0);

addToRunTimeSelectionTable
(
    uniGasPatchBoundary,
    uniGasCLLWallFieldPatch,
    dictionary
);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::uniGasCLLWallFieldPatch::uniGasCLLWallFieldPatch
(
    const polyMesh& mesh,
    uniGasCloud& cloud,
    const dictionary& dict
)
:
    uniGasPatchBoundary(mesh, cloud, dict),
    propsDict_(dict.subDict(typeName + "Properties")),
    normalAccommCoeff_(propsDict_.get<scalar>("normalAccommCoeff")),
    tangentialAccommCoeff_(propsDict_.get<scalar>("tangentialAccommCoeff")),
    rotEnergyAccommCoeff_(propsDict_.get<scalar>("rotEnergyAccommCoeff")),
    boundaryT_
    (
        IOobject
        (
            "boundaryT",
            mesh_.time().timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),
    boundaryU_
    (
        IOobject
        (
            "boundaryU",
            mesh_.time().timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    )
{
    writeInTimeDir_ = false;
    writeInCase_ = false;
    measurePropertiesAtWall_ = true;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::uniGasCLLWallFieldPatch::initialConfiguration()
{
    if ((normalAccommCoeff_ < VSMALL) && (tangentialAccommCoeff_ < VSMALL))
    {
        // Reduces to a specular wall, so no need to measure properties
        measurePropertiesAtWall_ = false;
    }
}


void Foam::uniGasCLLWallFieldPatch::calculateProperties()
{}


void Foam::uniGasCLLWallFieldPatch::controlParticle
(
    uniGasParcel& p,
    uniGasParcel::trackingData& td
)
{
    measurePropertiesBeforeControl(p);

    vector& U = p.U();

    label typeId = p.typeId();

    scalar& ERot = p.ERot();

    labelList& vibLevel = p.vibLevel();

    label wppIndex = p.patch();

    const polyPatch& patch = mesh_.boundaryMesh()[wppIndex];

    label wppLocalFace = patch.whichFace(p.face());

    vector nw = p.normal();
    nw /= mag(nw);

    // Normal velocity magnitude
    scalar U_dot_nw = U & nw;

    // Wall tangential velocity (flow direction)
    vector Ut = U - U_dot_nw*nw;

    Random& rndGen(cloud_.rndGen());

    while (mag(Ut) < SMALL)
    {
        // If the incident velocity is parallel to the face normal, no
        // tangential direction can be chosen.  Add a perturbation to the
        // incoming velocity and recalculate.

        U = vector
        (
            U.x()*(0.8 + 0.2*rndGen.sample01<scalar>()),
            U.y()*(0.8 + 0.2*rndGen.sample01<scalar>()),
            U.z()*(0.8 + 0.2*rndGen.sample01<scalar>())
        );

        U_dot_nw = U & nw;

        Ut = U - U_dot_nw*nw;
    }

    // Wall tangential unit vector
    vector tw1 = Ut/mag(Ut);

    // Other tangential unit vector
    vector tw2 = nw^tw1;

    const scalar T = boundaryT_.boundaryField()[wppIndex][wppLocalFace];

    const auto& ccp = cloud_.constProps(typeId);

    scalar mass = ccp.mass();

    label vibrationalDof = ccp.vibrationalDoF();

    const scalar alphaT = tangentialAccommCoeff_*(2.0 - tangentialAccommCoeff_);

    const scalar alphaN = normalAccommCoeff_;

    scalar mostProbableVelocity = sqrt(2.0*physicoChemical::k.value()*T/mass);


    // normalising the incident velocities

    vector normalisedTangentialVelocity = Ut/mostProbableVelocity;

    scalar normalisedNormalVelocity = U_dot_nw/mostProbableVelocity;


    // normal random number components

    scalar thetaNormal = 2.0*pi*rndGen.sample01<scalar>();

    scalar rNormal = sqrt(-alphaN*log(rndGen.sample01<scalar>()));


    // tangential random number components

    scalar thetaTangential1 = 2.0*pi*rndGen.sample01<scalar>();

    scalar rTangential1 = sqrt(-alphaT*log(rndGen.sample01<scalar>()));

    scalar thetaTangential2 = 2.0*pi*rndGen.sample01<scalar>();

    scalar rTangential2 = sqrt(-alphaT*log(rndGen.sample01<scalar>()));

    scalar normalisedIncidentTangentialVelocity1 =
        mag(normalisedTangentialVelocity);

    // selecting post - collision velocity components

    scalar um = sqrt(1.0 - alphaN)*normalisedNormalVelocity;

    vector normalVelocity =
        sqrt
        (
            (rNormal*rNormal)
          + (um*um)
          + 2.0*rNormal*um*cos(thetaNormal)
        )*nw;

    vector tangentialVelocity1 =
        (
            sqrt(1.0 - alphaT)*mag(normalisedIncidentTangentialVelocity1)
          + rTangential1*cos(thetaTangential1)
        )*tw1;

    vector tangentialVelocity2 = (rTangential2*cos(thetaTangential2))*tw2;


    // setting the post interaction velocity

    U =
        mostProbableVelocity
       *(
            tangentialVelocity1
          + tangentialVelocity2
          - normalVelocity
        );

    const vector& uw = boundaryU_.boundaryField()[wppIndex][wppLocalFace];
    vector uWallNormal = (uw & nw) * nw;
    vector uWallTangential1 = (uw & tw1) * tw1;
    vector uWallTangential2 = (uw & tw2) * tw2;

    vector UNormal =
        ((U & nw) * nw) + uWallNormal*normalAccommCoeff_;
    vector UTangential1 = (U & tw1) * tw1 + uWallTangential1*alphaT;
    vector UTangential2 = (U & tw2) * tw2 + uWallTangential2*alphaT;

    U = UNormal + UTangential1 + UTangential2;

    scalar om =
        sqrt
        (
            ERot*(1.0 - rotEnergyAccommCoeff_)
           /(physicoChemical::k.value()*T)
        );

    scalar rRot =
        sqrt
        (
           -rotEnergyAccommCoeff_
           *(
                log(max(1.0 - rndGen.sample01<scalar>(), VSMALL))
            )
        );

    scalar cosThetaRot = cos(2.0*pi*rndGen.sample01<scalar>());

    ERot =
        physicoChemical::k.value()*T
       *(
            sqr(rRot)
          + sqr(om)
          + 2.0*rRot*om*cosThetaRot
        );

    vibLevel =
        cloud_.equipartitionVibrationalEnergyLevel
        (
            T,
            vibrationalDof,
            typeId
        );

    measurePropertiesAfterControl(p, 0.0);
}


void Foam::uniGasCLLWallFieldPatch::output
(
    const fileName& fixedPathName,
    const fileName& timePath
)
{}


void Foam::uniGasCLLWallFieldPatch::updateProperties(const dictionary& dict)
{
    // The main properties should be updated first
    uniGasPatchBoundary::updateProperties(dict);
}

// ************************************************************************* //
