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

#include "uniGasReactions.H"
#include "uniGasCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::uniGasReactions::uniGasReactions(const Time& t, const polyMesh& mesh)
:
    time_(t),
    chemReactDict_
    (
        IOobject
        (
            "chemReactDict",
            time_.system(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        )
    ),
    nReactions_(-1),
    reactionsList_(),
    reactions_(),
    pairAddressing_(),
    counter_(0)
{}


Foam::uniGasReactions::uniGasReactions
(
    const Time& t,
    const polyMesh& mesh,
    uniGasCloud& cloud
)
:
    time_(t),
    chemReactDict_
    (
        IOobject
        (
            "chemReactDict",
            time_.system(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    ),
    nReactions_(0),
    reactionsList_(chemReactDict_.lookup("reactions")),
    reactionNames_(reactionsList_.size()),
    reactionIds_(reactionsList_.size()),
    reactions_(reactionsList_.size()),
    counter_(0)
{
    Info << nl << "Creating uniGasReactions" << nl << endl;

    // state uniGasReactions

    if (reactions_.size() > 0)
    {
        forAll(reactions_, r)
        {
            const entry& reactionEntry = reactionsList_[r];
            const dictionary& reactionDict = reactionEntry.dict();

            reactions_[r] = uniGasReaction::New(time_, cloud, reactionDict);
            reactionNames_[r] = reactions_[r]->type();
            reactionIds_[r] = r;

            ++nReactions_;
        }

        Info << "number of reactions created: " << nReactions_ << endl;
    }
    else
    {
        WarningInFunction
            << "No reactions provided" << endl;
    }

    pairAddressing_.setSize(cloud.typeIdList().size());

    forAll(pairAddressing_, p)
    {
        pairAddressing_[p].setSize(cloud.typeIdList().size(), -1);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::uniGasReactions::initialConfiguration()
{
    // Initial configuration
    // - call this function after the uniGasCloud is completely initialised

    for (auto& r : reactions_)
    {
        r->initialConfiguration();
    }

    // Set pair addressing

    forAll(pairAddressing_, i)
    {
        forAll(pairAddressing_[i], j)
        {
            label noOfReactionModelsPerPair = 0;

            forAll(reactions_, r)
            {
                if (reactions_[r]->tryReactMolecules(i, j))
                {
                    pairAddressing_[i][j] = r;

                    ++noOfReactionModelsPerPair;
                }
            }

            if (noOfReactionModelsPerPair > 1)
            {
                FatalErrorInFunction
                    << "There is more than one reaction model specified "
                    << "for the typeId pair: "
                    << i << " and " << j
                    << exit(FatalError);
            }
        }
    }

    Info << "reactions: " << reactionNames_ << endl;
}


void Foam::uniGasReactions::outputData()
{
    ++counter_;

    for (auto& r : reactions_)
    {
        r->outputResults(counter_);
    }
}


// ************************************************************************* //
