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
    Foam::uniGasCloud

Description
    Base cloud class.

SourceFiles
    uniGasCloudI.H
    uniGasCloud.C

\*---------------------------------------------------------------------------*/

#ifndef uniGasCloud_H
#define uniGasCloud_H

#include "CloudWithModels.H"

#include "uniGasParcel.H"
#include "IOdictionary.H"
#include "autoPtr.H"
#include "boolVector.H"
#include "Random.H"
#include "fvMesh.H"
#include "volFields.H"
#include "scalarIOField.H"
#include "dsmcCollisionModel.H"
#include "bgkCollisionModel.H"

#include "uniGasBoundaries.H"
#include "uniGasAllConfigurations.H"
#include "uniGasFieldProperties.H"
#include "uniGasFaceTracker.H"
#include "uniGasDynamicLoadBalancing.H"
#include "uniGasDynamicAdapter.H"
#include "uniGasHybridDecomposition.H"

#include "uniGasReactions.H"
#include "dsmcCollisionPartner.H"
#include "boundaryMeasurements.H"
#include "cellMeasurements.H"

#include "CloudFunctionObjectList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
                          Class uniGasCloud Declaration
\*---------------------------------------------------------------------------*/


class uniGasCloud
:
    public CloudWithModels<uniGasParcel>
{
public:

    typedef uniGasCloud baseCloudType;

private:

    // Private data

        //- Variables for identifying different dsmc-bgk collision models
        const label dsmcCollModelId_;

        const label bgkCollModelId_;

        const word dsmcCollModel_;
        
        const word bgkCollModel_;
        
        const word hybridCollModel_;

        //- A variable holding the simulation collision type (dsmc, bgk, hybrid)
        word collisionModel_;

        //- A variable holding the simulation bgk scheme (required for correct USP heat flux vector and pressure tensor calculation)
        word bgkCollisionModelName_;

        // A boolean vector containing the solution dimensions
        boolVector solutionDimensions_;

        //- A list of unique instances of molecule types in the
        //- simulation.
        //  The position of an entry in the list maps to
        //  the label identifying the typeId, i.e. where typeIdList_ =
        //  (N2 O2 CO2) N2 has typeId label = 0, O2 = 1, CO2 = 2.
        List<word> typeIdList_;

        //- Number of real atoms/molecules represented by a parcel
        scalar nParticle_;

        //- Collision reference temperature
        scalar collTref_;

        // dynamic simulation set-ups

            //- Bool to identify is this is a subCell dynamic simulation
            bool adaptive_;

        // Cell weighted set-ups
            //- Bool to identify is this is a cell weighted simulation
            //  Movements dealt differently if true
            bool cellWeighted_;

            //- Desired number of particles per subCell
            label particlesPerSubCell_;

            //- Minimum allowed number of particles per subCell
            label minParticlesPerSubCell_;

            //- Maximum allowed cell weight ratio between adjacent cells
            const scalar maxCellWeightRatio_;

            //- Maximum allowed number of smoothing passes
            const label maxSmoothingPasses_;

        // Axisymmetric set-ups

            //- Bool to identify is this is an axisymmetric simulation
            //  Movements dealt with differently if true
            bool axisymmetric_;

            //- Radial extent of the axisymmetric domain
            scalar radialExtent_;

            //- Maximum radial scaling factor for axisymmetric simulations
            scalar maxRWF_;

        //- Write out the information to terminal every nTerminalOutputs
        label nTerminalOutputs_;

        //- Stack of previous write directories
       DynamicList<word> previousTimeDirs_;

        //- A data structure holding electron number density
        scalarList rhoNMeanElectron_;

        //- A data structure holding electron mass density
        scalarList rhoMMeanElectron_;

        //- A data structure holding all species mass density
        scalarList rhoMMean_;

        //- A data structure holding electron momentum
        List<vector> momentumMeanElectron_;

        //- A data structure holding all species momentum
        List<vector> momentumMean_;

        //- A data structure holding electron linearKE
        scalarList linearKEMeanElectron_;

        //- A data structure holding electron temperature
        scalarList electronTemperature_;

        //- A data structure holding cell velocity
        List<vector> cellVelocity_;

        //- A field holding the cell weights in case of cell weighted simulation
        volScalarField cellWeightFactor_;

        //- A field holding the value of (sigmaT * cR)max for each
        //- cell (see Bird p220).
        //  Initialised with the parcels, updated as required, and read in on
        //  start/restart.
        volScalarField sigmaTcRMax_;

        // A field containing the number of subCells in each directions
        volVectorField subCellLevels_;

        // A field containing the collision model for each cell 0:dsmc 1:bgk
        volScalarField cellCollisionModelId_;

        //- Parcel constant properties - one for each type
        List<uniGasParcel::constantProperties> constProps_;

        //- Random number generator
        Random rndGen_;

        //- load balancing
        uniGasDynamicLoadBalancing dynamicLoadBalancing_;

        //- dynamic adapter
        uniGasDynamicAdapter dynamicAdapter_;

        //- Fields
        uniGasFieldProperties fields_;

        //- Boundaries
        uniGasBoundaries boundaries_;

        //- Fluxes - face tracking
        uniGasFaceTracker trackingInfo_;


        // References to the cloud sub - models

            //- dsmc collision model
            autoPtr<dsmcCollisionModel> dsmcCollisionModel_;

            //- Collision selection routine model
            autoPtr<dsmcCollisionPartner> dsmcCollisionPartnerModel_;

            //- Collision selection routine model
            autoPtr<bgkCollisionModel> bgkCollisionModel_;

            //- Hybrid decomposer
            autoPtr<uniGasHybridDecomposition> hybridDecompositionModel_;

            //- Reactions
            uniGasReactions reactions_;

            //- Boundary measurements
            boundaryMeasurements boundaryMeas_;

            //- Used to calculate mean collision separation
            cellMeasurements cellMeas_;

            CloudFunctionObjectList<baseCloudType> functions_;


    // Private Member Functions

        //- Build the constant properties for all of the species
        void buildConstProps();

        //- Axisymmetric and cell weighting
        void weighting();

        //- Dynamically adapt the simulation
        void adaptation();

        //- Remove electrons after adding their velocities to a DynamicList
        void removeElectrons();

        //- Add an electron where an ion is found, after the move function
        void addElectrons();

        //- Calculate collisions between molecules
        void collisions();

        //- Decompose domain in hybrid simulation
        void decomposition();

        //- Disallow default bitwise copy construct
        uniGasCloud(const uniGasCloud&);

        //- Disallow default bitwise assignment
        void operator=(const uniGasCloud&);


public:

    // Constructors

        //- Construct given name, mesh and initialisation dictionary. If parcels exist
        //- reads them from file, else they are initialised
        uniGasCloud
        (
            const word& cloudName,
            const IOdictionary& uniGasInitialisationDict,
            Time& t,
            fvMesh& mesh
        );

    //- Destructor
    virtual ~uniGasCloud() = default;


    // Member Functions

        //- Return dsmc collision model ID
        inline label dsmcCollModelId() const;

        //- Return bgk collision model ID
        inline label bgkCollModelId() const;

        //- Return bgk collision model name
        inline word bgkCollisionModelName() const;

        //- Return the particle IDs corresponding to the particle names read
        //- from the input dictionary
        labelList getTypeIDs(const dictionary& dict) const;

        //- Return the solution dimensions vector.
        inline boolVector solutionDimensions() const;

        //- Return the idList
        inline const List<word>& typeIdList() const;

        //- Return the number of real particles represented by one parcel
        inline scalar nParticle() const;

        //- Return the number of real particles represented by one parcel
        inline scalar collTref() const;

        //- Return the bool to identify if this is an dynamic subCell simulation
        inline bool adaptive() const;

        //- Return the bool to identify if this is a cell weighted simulation
        //  Measurements, etc dealt differently if true
        inline bool cellWeighted() const;

        //- Return the desired number of particles per subCell
        inline label particlesPerSubCell() const;

        //- Return the minimum allowed number of particles per subCell
        inline label minParticlesPerSubCell() const;

        //- Return the maximum allowed cell weight ratio between adjacent cells
        inline scalar maxCellWeightRatio() const;

        //- Return the maximum allowed number of smoothing passes
        inline label maxSmoothingPasses() const;

        //- Return the cell weighting factor
        inline scalar cellWF(const label& cell) const;

        //- Return the radius of the domain for axisymmetric cases
        inline scalar radialExtent() const;

        //- Return the radial weighting factor for axisymmetric cases
        inline scalar maxRWF() const;

        //- Return the radial weighting factor for axisymmetric cases
        inline scalar axiRWF(const point& p) const;

        //- Return the bool to identify if this is an axisymmetric simulation
        //  Measurements, etc dealt differently if true
        inline bool axisymmetric() const;

        //- Return the number of timesteps to write out to terminal at
        inline label nTerminalOutputs() const;

        //- Return the subCellLevels field.  non - const access to allow
        //- definition during initialisation and adaptation.
        inline volVectorField& subCellLevels();

        //- Return the cellWeight field.  non - const access to allow
        //- definition during initialisation.
        inline volScalarField& cellWeightFactor();

        //- Return the sigmaTcRMax field.  non - const access to allow
        //- updating.
        inline volScalarField& sigmaTcRMax();

        //- Return the cellCollisionModel field.  non - const access to allow
        //- updating through decomposer class.
        inline volScalarField& cellCollModelId();

        //- Return all of the constant properties
        inline const List<uniGasParcel::constantProperties>& constProps() const;

        //- Return the constant properties of the given typeId
        inline const uniGasParcel::constantProperties& constProps
        (
            const label typeId
        ) const;

        //- Return refernce to the random object
        inline Random& rndGen();

        // Kinetic theory helper functions

        //- Generate a random velocity sampled from the Maxwellian speed
        //- distribution
        vector equipartitionLinearVelocity
        (
            const scalar temperature,
            const scalar mass
        );

        //- Generate a random velocity sampled from the Chapman-Enskog speed
        // distribution
        vector chapmanEnskogLinearVelocity
        (
            scalar maxMacroscopic,
            scalar pressure,
            scalar temperature,
            scalar mass,
            vector heatflux,
            tensor stress
        );

        //- Generate a random rotational energy, sampled from the
        //- equilibrium distribution (Bird eqn 11.22 and 11.23 and
        //- adapting code from DSMC3.FOR)
        scalar equipartitionRotationalEnergy
        (
            const scalar temperature,
            const scalar rotationalDoF
        );

        //- Generate a random vibrational energy
        labelList equipartitionVibrationalEnergyLevel
        (
            const scalar temperature,
            const scalar vibrationalDoF,
            const label typeId
        );

        //- Generate a random electronic energy level
        label equipartitionElectronicLevel
        (
            const scalar temperature,
            const List<label>& degeneracyList,
            const List<scalar>& electronicEnergyList,
            const label typeId
        );

        //- Generate a LB post - collision rotational energy
        scalar postCollisionRotationalEnergy
        (
            const scalar rotationalDof,
            const scalar ChiB
        );

        //- Generate a LB post - collision vibrational energy
        label postCollisionVibrationalEnergyLevel
        (
            const bool postReaction,
            const label vibLevel,
            const label iMax,
            const scalar thetaV,
            const scalar thetaD,
            const scalar refTempZv,
            const scalar omega,
            const scalar Zref,
            const scalar Ec
        );

        //- Generate a LB post-collision electronic energy level
        label postCollisionElectronicEnergyLevel
        (
            const scalar Ec,
            const label jMax,
            const scalar omega,
            const List<scalar>& EElist,
            const List<label>& gList
        );

        //- Average particle speed from the Maxwellian distribution
        inline scalar maxwellianAverageSpeed
        (
            const scalar temperature,
            const scalar mass
        ) const;

        inline tmp<scalarField> maxwellianAverageSpeed
        (
            const scalarField& temperature,
            const scalar mass
        ) const;

        //- RMS particle speed
        inline scalar maxwellianRMSSpeed
        (
            const scalar temperature,
            const scalar mass
        ) const;

        inline tmp<scalarField> maxwellianRMSSpeed
        (
            const scalarField& temperature,
            const scalar mass
        ) const;

        //- Most probable speed
        inline scalar maxwellianMostProbableSpeed
        (
            const scalar temperature,
            const scalar mass
        ) const;

        inline tmp<scalarField> maxwellianMostProbableSpeed
        (
            const scalarField& temperature,
            const scalar mass
        ) const;


    // Sub-models

        //- Return reference to dynamic adaptation model
        inline const uniGasDynamicAdapter& dynamicAdapter() const;
        inline uniGasDynamicAdapter& dynamicAdapter();

        //- Return reference to field models
        inline const uniGasFieldProperties& fields() const;
        inline uniGasFieldProperties& fields();

        //- Return reference to boundary models
        inline const uniGasBoundaries& boundaries() const;
        inline uniGasBoundaries& boundaries();

        //- Return reference to dynamic adaptation model
        inline const uniGasFaceTracker& tracker() const;
        inline uniGasFaceTracker& tracker();

        //- Return reference to dsmc collision model
        inline const dsmcCollisionModel& dsmcCollision() const;
        inline dsmcCollisionModel& dsmcCollision();

        //- Return reference to bgk collision model
        inline const bgkCollisionModel& bgkCollision() const;
        inline bgkCollisionModel& bgkCollision();

        //- Return reference to decomposition model
        inline const uniGasHybridDecomposition& hybridDecomposition() const;
        inline uniGasHybridDecomposition& hybridDecomposition();

        //- Return reference to reaction models
        inline const uniGasReactions& reactions() const;
        inline uniGasReactions& reactions();

        //- Return reference to boundary measurement models
        inline const boundaryMeasurements& boundaryFluxMeasurements() const;
        inline boundaryMeasurements& boundaryFluxMeasurements();

        //- Return reference to cell measurement models
        inline const cellMeasurements& cellPropMeasurements() const;
        inline cellMeasurements& cellPropMeasurements();

        // Check

            //- Total mass in system
            inline scalarList infoMeasurements() const;

            //- Print cloud information
            void info() const;

            //- Calculate and print maximum load imbalance
            void loadBalanceCheck();

            //- Dump particle positions to .obj file
            void dumpParticlePositions() const;

            //- Create or delete particles according to cell weighting;
            void cellWeighting();

            //- Create or delete particles according to axisymmetric weighting;
            void axisymmetricWeighting();

            //- Create or delete particles according to combined axisymmetric and cell weighting;
            void axisymmetricCellWeighting();


        // Cloud evolution functions

            //- Add new parcel
            void addNewParcel
            (
                const vector& position,
                const vector& U,
                const scalar CWF,
                const scalar RWF,
                const scalar ERot,
                const label ELevel,
                const label cellI,
                const label typeId,
                const label newParcel,
                const labelList vibLevel
            );

            //- Redistribute vibrational energy after a reaction
            scalar energyRatio
            (
                const scalar ChiA,
                const scalar ChiB
            );

            //- Redistribute vibrational energy after a reaction
            scalar PSIm
            (
                const scalar DOFm,
                const scalar DOFtot
            );

            //- Evolve the cloud (move, collide)
            void evolve();

            //- Retrun how often to write to terminal
            // (used for the solver to access this)
            label nTerminalOutputs();

            //- Clear the Cloud
            inline void clear();


            CloudFunctionObjectList<baseCloudType>& functions()
            {
                return functions_;
            }

        // System functions

            void cleanLagrangian();


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "uniGasCloudI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
